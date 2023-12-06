//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#define CA_PRIVATE_IMPLEMENTATION

#include "metal_swapchain.h"
#include "common/logging.h"
#include "common/metal_helpers.h"
#include "common/filesystem.h"

namespace vox::compute::metal {
MetalSwapchain::MetalSwapchain(MTL::Device* device, uint64_t window_handle,
                               uint width, uint height, bool allow_hdr,
                               bool vsync, uint back_buffer_size) noexcept
    : _layer{metal_backend_create_layer(
          device, window_handle,
          width, height, allow_hdr,
          vsync, back_buffer_size)},
      _render_pass_desc{make_shared(MTL::RenderPassDescriptor::alloc()->init())} {
    _layer->retain();
    auto attachment_desc = _render_pass_desc->colorAttachments()->object(0);
    attachment_desc->setLoadAction(MTL::LoadActionClear);
    attachment_desc->setStoreAction(MTL::StoreActionStore);
    attachment_desc->setClearColor(MTL::ClearColor(1.0, 1.0, 1.0, 1.0));
    _format = allow_hdr ? MTL::PixelFormatRGBA16Float : MTL::PixelFormatBGRA8Unorm;

    create_pso(device);
}

MetalSwapchain::~MetalSwapchain() noexcept {
    if (_command_label) { _command_label->release(); }
    _render_pass_desc.reset();
    _pipeline.reset();
    _layer->release();
}

void MetalSwapchain::create_pso(MTL::Device *device) {
    auto raw_source = fs::read_shader("usd_blit.metal");
    auto source = NS::String::string(raw_source.c_str(), NS::UTF8StringEncoding);
    NS::Error *error{nullptr};
    auto option = make_shared(MTL::CompileOptions::alloc()->init());
    auto defaultLibrary = make_shared(device->newLibrary(source, option.get(), &error));
    if (error != nullptr) {
        LOGE("Error: could not load Metal shader library: {}",
             error->description()->cString(NS::StringEncoding::UTF8StringEncoding))
    }

    auto functionName = NS::String::string("vtxBlit", NS::UTF8StringEncoding);
    auto vertexFunction = make_shared(defaultLibrary->newFunction(functionName));
    functionName = NS::String::string("fragBlitLinear", NS::UTF8StringEncoding);
    auto fragmentFunction = make_shared(defaultLibrary->newFunction(functionName));

    // Set up the pipeline state object.
    auto pipelineStateDescriptor = make_shared(MTL::RenderPipelineDescriptor::alloc()->init());
    pipelineStateDescriptor->setRasterSampleCount(1);
    pipelineStateDescriptor->setVertexFunction(vertexFunction.get());
    pipelineStateDescriptor->setFragmentFunction(fragmentFunction.get());
    pipelineStateDescriptor->setDepthAttachmentPixelFormat(MTL::PixelFormatInvalid);

    // Configure the color attachment for blending.
    MTL::RenderPipelineColorAttachmentDescriptor *colorDescriptor = pipelineStateDescriptor->colorAttachments()->object(0);
    colorDescriptor->setPixelFormat(_format);
    colorDescriptor->setBlendingEnabled(true);
    colorDescriptor->setRgbBlendOperation(MTL::BlendOperationAdd);
    colorDescriptor->setAlphaBlendOperation(MTL::BlendOperationAdd);
    colorDescriptor->setSourceRGBBlendFactor(MTL::BlendFactorOne);
    colorDescriptor->setSourceAlphaBlendFactor(MTL::BlendFactorOne);
    colorDescriptor->setDestinationRGBBlendFactor(MTL::BlendFactorOneMinusSourceAlpha);
    colorDescriptor->setDestinationAlphaBlendFactor(MTL::BlendFactorZero);

    error = nullptr;
    _pipeline = make_shared(device->newRenderPipelineState(pipelineStateDescriptor.get(), &error));
    if (!_pipeline) {
        LOGE("Failed to created pipeline state, error {}", error->description()->cString(NS::StringEncoding::UTF8StringEncoding))
    }
}

void MetalSwapchain::present(MTL::CommandBuffer *command_buffer, MTL::Texture *image) noexcept {
    if (auto drawable = _layer->nextDrawable()) {
        auto attachment_desc = _render_pass_desc->colorAttachments()->object(0);
        attachment_desc->setTexture(drawable->texture());
        auto command_encoder = command_buffer->renderCommandEncoder(_render_pass_desc.get());

        // Blit the texture to the view.
        command_encoder->pushDebugGroup(NS::String::string("FinalBlit", NS::UTF8StringEncoding));
        command_encoder->setFragmentTexture(image, 0);
        command_encoder->setRenderPipelineState(_pipeline.get());
        command_encoder->drawPrimitives(MTL::PrimitiveTypeTriangle, NS::UInteger(0), NS::UInteger(3));
        command_encoder->popDebugGroup();

        command_encoder->endEncoding();
        command_buffer->presentDrawable(drawable);
        if (_command_label) { command_buffer->setLabel(_command_label); }
    } else {
        LOGW("Failed to acquire next drawable from swapchain.");
    }
}

void MetalSwapchain::set_name(std::string_view name) noexcept {
    if (_command_label) {
        _command_label->release();
        _command_label = nullptr;
    }
    if (!name.empty()) {
        auto label = fmt::format("{}::present", name);
        _command_label = NS::String::alloc()->init(
            label.data(), label.size(),
            NS::UTF8StringEncoding, false);
    }
}

}// namespace vox::compute::metal