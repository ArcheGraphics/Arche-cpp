//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "forward_application.h"
#include "rendering/subpasses/forward_subpass.h"
#include "engine.h"
#include "camera.h"

namespace vox {
Camera* ForwardApplication::mainCamera() {
    return _mainCamera;
}

ForwardApplication::~ForwardApplication() {
    _renderPass.reset();
    // release first
    _sceneManager.reset();

    _componentsManager.reset();
    _physicsManager.reset();
    _lightManager.reset();
    _shadowManager.reset();
    _particleManager.reset();
}

bool ForwardApplication::prepare(Engine &engine) {
    GraphicsApplication::prepare(engine);
    
    _componentsManager = std::make_unique<ComponentsManager>();
    _physicsManager = std::make_unique<physics::PhysicsManager>();
    _sceneManager = std::make_unique<SceneManager>(_device);
    auto scene = _sceneManager->currentScene();
    
    _particleManager = std::make_unique<ParticleManager>(_device);
    _lightManager = std::make_unique<LightManager>(scene);
    {
        loadScene();
        auto extent = engine.window().extent();
        auto factor = engine.window().contentScaleFactor();
        _componentsManager->callScriptResize(extent.width, extent.height, factor * extent.width, factor * extent.height);
        _mainCamera->resize(extent.width, extent.height, factor * extent.width, factor * extent.height);
        _depthStencilTexture = _createDepthStencilView(factor * extent.width, factor * extent.height);
    }
    _lightManager->setCamera(_mainCamera);
    _shadowManager = std::make_unique<ShadowManager>(scene, _mainCamera);
    
    // Create a render pass descriptor for thelighting and composition pass
    // Whatever rendered in the final pass needs to be stored so it can be displayed
    _renderPassDescriptor.colorAttachmentCount = 1;
    _renderPassDescriptor.colorAttachments = &_colorAttachments;
    _renderPassDescriptor.depthStencilAttachment = &_depthStencilAttachment;
    
    _colorAttachments.storeOp = wgpu::StoreOp::Store;
    _colorAttachments.loadOp = wgpu::LoadOp::Clear;
    auto& color = scene->background.solidColor;
    _colorAttachments.clearValue = wgpu::Color{color.r, color.g, color.b, color.a};
    _depthStencilAttachment.depthLoadOp = wgpu::LoadOp::Clear;
    _depthStencilAttachment.depthClearValue = 1.0;
    _depthStencilAttachment.depthStoreOp = wgpu::StoreOp::Discard;
    _depthStencilAttachment.stencilLoadOp = wgpu::LoadOp::Clear;
    _depthStencilAttachment.stencilStoreOp = wgpu::StoreOp::Discard;
    _renderPass = std::make_unique<RenderPass>(_device, _renderPassDescriptor);
    _renderPass->addSubpass(std::make_unique<ForwardSubpass>(_renderContext.get(), _depthStencilTextureFormat,
                                                             scene, _mainCamera));
    
    return true;
}

void ForwardApplication::update(float deltaTime) {
    GraphicsApplication::update(deltaTime);
    {
        _componentsManager->callScriptOnStart();
        
        _physicsManager->callColliderOnUpdate();
        _physicsManager->update(deltaTime);
        _physicsManager->callColliderOnLateUpdate();
        _physicsManager->callCharacterControllerOnLateUpdate();
        
        _componentsManager->callScriptOnUpdate(deltaTime);
        _componentsManager->callAnimatorUpdate(deltaTime);
        _componentsManager->callSceneAnimatorUpdate(deltaTime);
        _componentsManager->callScriptOnLateUpdate(deltaTime);
        
        _componentsManager->callRendererOnUpdate(deltaTime);
        _sceneManager->currentScene()->updateShaderData();
    }
    
    wgpu::CommandEncoder commandEncoder = _device.CreateCommandEncoder();
    updateGPUTask(commandEncoder);
    
    // Render the lighting and composition pass
    _colorAttachments.view = _renderContext->currentDrawableTexture();
    _depthStencilAttachment.view = _depthStencilTexture;
    
    _renderPass->draw(commandEncoder, "Lighting & Composition Pass");
    // Finalize rendering here & push the command buffer to the GPU
    wgpu::CommandBuffer commands = commandEncoder.Finish();
    _device.GetQueue().Submit(1, &commands);
    _renderContext->present();
}

void ForwardApplication::updateGPUTask(wgpu::CommandEncoder& commandEncoder) {
    _shadowManager->draw(commandEncoder);
    _lightManager->draw(commandEncoder);
    _particleManager->draw(commandEncoder);
}

bool ForwardApplication::resize(uint32_t win_width, uint32_t win_height,
                                uint32_t fb_width, uint32_t fb_height) {
    GraphicsApplication::resize(win_width, win_height, fb_width, fb_height);
    _depthStencilTexture = _createDepthStencilView(fb_width, fb_height);

    _componentsManager->callScriptResize(win_width, win_height, fb_width, fb_height);    
    _mainCamera->resize(win_width, win_height, fb_width, fb_height);
    return true;
}

void ForwardApplication::inputEvent(const InputEvent &inputEvent) {
    GraphicsApplication::inputEvent(inputEvent);
    _componentsManager->callScriptInputEvent(inputEvent);
}

wgpu::TextureView ForwardApplication::_createDepthStencilView(uint32_t width, uint32_t height) {
    wgpu::TextureDescriptor descriptor;
    descriptor.dimension = wgpu::TextureDimension::e2D;
    descriptor.size.width = width;
    descriptor.size.height = height;
    descriptor.size.depthOrArrayLayers = 1;
    descriptor.sampleCount = 1;
    descriptor.format = _depthStencilTextureFormat;
    descriptor.mipLevelCount = 1;
    descriptor.usage = wgpu::TextureUsage::RenderAttachment;
    auto depthStencilTexture = _device.CreateTexture(&descriptor);
    return depthStencilTexture.CreateView();
}

}
