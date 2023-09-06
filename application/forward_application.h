//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "graphics_application.h"
#include "shadow/shadow_manager.h"
#include "lighting/light_manager.h"
#include "particle/particle_manager.h"

namespace vox {
class ForwardApplication : public MetalApplication {
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

    virtual void updateGPUTask(MTL::CommandBuffer &commandBuffer);

protected:
    Camera *_mainCamera{nullptr};

    /**
     * @brief Holds all scene information
     */
    std::unique_ptr<Scene> _scene{nullptr};

    std::shared_ptr<MTL::RenderPassDescriptor> _renderPassDescriptor{nullptr};

    /**
     * @brief Pipeline used for rendering, it should be set up by the concrete sample
     */
    std::unique_ptr<RenderPass> _renderPass{nullptr};

    std::unique_ptr<ShadowManager> _shadowManager{nullptr};
    std::unique_ptr<LightManager> _lightManager{nullptr};
    std::unique_ptr<ParticleManager> _particleManager{nullptr};
};

}// namespace vox