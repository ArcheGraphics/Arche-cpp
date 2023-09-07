//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "graphics_application.h"
#include "platform/platform.h"

#include "framework/common/metal_helpers.h"

namespace vox {
MetalApplication::~MetalApplication() {
    _library.reset();
    _device.reset();
}

bool MetalApplication::prepare(Platform &engine) {
    if (!Application::prepare(engine)) {
        return false;
    }

    LOGI("Initializing Metal Application");

    _device = CLONE_METAL_CUSTOM_DELETER(MTL::Device, MTL::CreateSystemDefaultDevice());
    printf("Selected Device: %s\n", _device->name()->cString(NS::StringEncoding::UTF8StringEncoding));

    _library = makeShaderLibrary();

    _commandQueue = CLONE_METAL_CUSTOM_DELETER(MTL::CommandQueue, _device->newCommandQueue());

    _renderContext = window->createRenderContext(*_device);

    // Create a render pass descriptor for thelighting and composition pass
    // Whatever rendered in the final pass needs to be stored so it can be displayed
    _renderPassDescriptor = CLONE_METAL_CUSTOM_DELETER(MTL::RenderPassDescriptor, MTL::RenderPassDescriptor::alloc()->init());
    _renderPassDescriptor->colorAttachments()->object(0)->setStoreAction(MTL::StoreActionStore);
    _renderPassDescriptor->colorAttachments()->object(0)->setLoadAction(MTL::LoadActionClear);
    _renderPassDescriptor->depthAttachment()->setLoadAction(MTL::LoadActionClear);
    _renderPassDescriptor->depthAttachment()->setTexture(_renderContext->depthStencilTexture());
    _renderPassDescriptor->stencilAttachment()->setLoadAction(MTL::LoadActionClear);
    _renderPassDescriptor->stencilAttachment()->setTexture(_renderContext->depthStencilTexture());
    _renderPass = std::make_unique<RenderPass>(*_library, *_renderPassDescriptor);
    return true;
}

void MetalApplication::update(float delta_time) {
    _renderContext->nextDrawable();

    auto commandBuffer = CLONE_METAL_CUSTOM_DELETER(MTL::CommandBuffer, _commandQueue->commandBuffer());

    // The final pass can only render if a drawable is available, otherwise it needs to skip
    // rendering this frame.
    if (_renderContext->currentDrawable()) {
        // Render the lighting and composition pass
        _renderPassDescriptor->colorAttachments()->object(0)->setTexture(_renderContext->currentDrawableTexture());
        _renderPassDescriptor->depthAttachment()->setTexture(_renderContext->depthStencilTexture());
        _renderPassDescriptor->stencilAttachment()->setTexture(_renderContext->depthStencilTexture());
        _renderPass->draw(*commandBuffer, "Lighting & Composition Pass");
    }
    // Finalize rendering here & push the command buffer to the GPU
    commandBuffer->presentDrawable(_renderContext->currentDrawable());
    commandBuffer->commit();
}

bool MetalApplication::resize(uint32_t win_width, uint32_t win_height,
                              uint32_t fb_width, uint32_t fb_height) {
    Application::resize(win_width, win_height, fb_width, fb_height);
    _renderContext->resize(fb_width, fb_height);
    return true;
}

void MetalApplication::input_event(const InputEvent &inputEvent) {
    Application::input_event(inputEvent);
}

void MetalApplication::finish() {
}

std::shared_ptr<MTL::Library> MetalApplication::makeShaderLibrary() {
    NS::Error *error;
    CFURLRef libraryURL = CFBundleCopyResourceURL(CFBundleGetMainBundle(), CFSTR("vox.shader"), CFSTR("metallib"), nullptr);

    std::shared_ptr<MTL::Library> shaderLibrary =
        CLONE_METAL_CUSTOM_DELETER(MTL::Library, _device->newLibrary((NS::URL *)libraryURL, &error));

    if (error != nullptr) {
        LOGE("Error: could not load Metal shader library: {}",
             error->description()->cString(NS::StringEncoding::UTF8StringEncoding));
    }

    CFRelease(libraryURL);
    return shaderLibrary;
}

}// namespace vox
