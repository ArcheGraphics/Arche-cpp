//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "forward_application.h"
#include "rendering/subpasses/forward_subpass.h"
#include "platform/platform.h"
#include "components/camera.h"
#include "framework/common/metal_helpers.h"

namespace vox {
ForwardApplication::~ForwardApplication() {
    _renderPass.reset();
}

bool ForwardApplication::prepare(Platform &engine) {
    MetalApplication::prepare(engine);

    _scene = std::make_unique<Scene>(*_device);
    _lightManager = std::make_unique<LightManager>(_scene.get());
    {
        loadScene();
        auto extent = engine.get_window().get_extent();
        auto factor = engine.get_window().get_content_scale_factor();
        _mainCamera->resize(extent.width, extent.height, factor * extent.width, factor * extent.height);
    }

    // Create a render pass descriptor for thelighting and composition pass
    // Whatever rendered in the final pass needs to be stored so it can be displayed
    _renderPassDescriptor = CLONE_METAL_CUSTOM_DELETER(MTL::RenderPassDescriptor, MTL::RenderPassDescriptor::alloc()->init());
    _renderPassDescriptor->colorAttachments()->object(0)->setStoreAction(MTL::StoreActionStore);
    _renderPassDescriptor->colorAttachments()->object(0)->setLoadAction(MTL::LoadActionClear);
    auto &color = _scene->background.solid_color;
    _renderPassDescriptor->colorAttachments()->object(0)->setClearColor(MTL::ClearColor(color.r, color.g, color.b, color.a));
    _renderPassDescriptor->depthAttachment()->setLoadAction(MTL::LoadActionClear);
    _renderPassDescriptor->depthAttachment()->setTexture(_renderContext->depthStencilTexture());
    _renderPassDescriptor->stencilAttachment()->setLoadAction(MTL::LoadActionClear);
    _renderPassDescriptor->stencilAttachment()->setTexture(_renderContext->depthStencilTexture());
    _renderPass = std::make_unique<RenderPass>(*_library, *_renderPassDescriptor);
    _renderPass->addSubpass(std::make_unique<ForwardSubpass>(_renderContext.get(), _scene.get(), _mainCamera));
    if (_gui) {
        _renderPass->setGUI(_gui.get());
    }

    return true;
}

void ForwardApplication::update(float delta_time) {
    MetalApplication::update(delta_time);
    //    _scene->update(delta_time);

    auto commandBuffer = CLONE_METAL_CUSTOM_DELETER(MTL::CommandBuffer, _commandQueue->commandBuffer());
    updateGPUTask(*commandBuffer);

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
    commandBuffer->waitUntilCompleted();
}

void ForwardApplication::updateGPUTask(MTL::CommandBuffer &commandBuffer) {
}

bool ForwardApplication::resize(uint32_t win_width, uint32_t win_height,
                                uint32_t fb_width, uint32_t fb_height) {
    MetalApplication::resize(win_width, win_height, fb_width, fb_height);
    _mainCamera->resize(win_width, win_height, fb_width, fb_height);
    return true;
}

void ForwardApplication::inputEvent(const InputEvent &inputEvent) {
    MetalApplication::inputEvent(inputEvent);
}

}// namespace vox
