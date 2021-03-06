//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef forward_hpp
#define forward_hpp

#include "graphics_application.h"
#include "components_manager.h"
#include "physics/physics_manager.h"
#include "shadow/shadow_manager.h"
#include "lighting/light_manager.h"
#include "particle/particle_manager.h"
#include "scene_manager.h"

namespace vox {
class ForwardApplication: public GraphicsApplication {
public:
    ForwardApplication() = default;
    
    virtual ~ForwardApplication();
    
    /**
     * @brief Additional sample initialization
     */
    bool prepare(Engine &engine) override;
    
    /**
     * @brief Main loop sample events
     */
    void update(float delta_time) override;
    
    bool resize(uint32_t win_width, uint32_t win_height,
                uint32_t fb_width, uint32_t fb_height) override;
    
    void inputEvent(const InputEvent &inputEvent) override;
    
    virtual void loadScene() = 0;
    
    virtual void updateGPUTask(wgpu::CommandEncoder& commandEncoder);
    
    Camera* mainCamera();
    
protected:
    Camera* _mainCamera{nullptr};
    
    wgpu::RenderPassDescriptor _renderPassDescriptor;
    wgpu::RenderPassColorAttachment _colorAttachments;
    wgpu::RenderPassDepthStencilAttachment _depthStencilAttachment;
    
    /**
     * @brief Pipeline used for rendering, it should be set up by the concrete sample
     */
    std::unique_ptr<RenderPass> _renderPass{nullptr};
    
    /**
     * @brief Holds all scene information
     */
    std::unique_ptr<ComponentsManager> _componentsManager{nullptr};
    std::unique_ptr<physics::PhysicsManager> _physicsManager{nullptr};
    std::unique_ptr<SceneManager> _sceneManager{nullptr};
    std::unique_ptr<ShadowManager> _shadowManager{nullptr};
    std::unique_ptr<LightManager> _lightManager{nullptr};
    std::unique_ptr<ParticleManager> _particleManager{nullptr};
    
protected:
    wgpu::TextureView _depthStencilTexture;
    wgpu::TextureFormat _depthStencilTextureFormat = wgpu::TextureFormat::Depth24PlusStencil8;
    wgpu::TextureView _createDepthStencilView(uint32_t width, uint32_t height);
};

}

#endif /* forward_hpp */
