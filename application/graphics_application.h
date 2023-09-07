//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "application.h"
#include <Metal/Metal.hpp>
#include "rendering/render_pass.h"
#include "rendering/render_context.h"
#include "ecs/scene.h"
#include "gui/gui.h"

namespace vox {
class MetalApplication : public Application {
public:
    MetalApplication() = default;

    ~MetalApplication() override;

    /**
     * @brief Additional sample initialization
     */
    bool prepare(Platform &engine) override;

    /**
     * @brief Main loop sample events
     */
    void update(float delta_time) override;

    bool resize(uint32_t win_width, uint32_t win_height,
                uint32_t fb_width, uint32_t fb_height) override;

    void input_event(const InputEvent &inputEvent) override;

    void finish() override;

private:
    std::shared_ptr<MTL::Library> makeShaderLibrary();

protected:
    /**
     * @brief The Metal device
     */
    std::shared_ptr<MTL::Device> _device{nullptr};

    /**
     * @brief The Metal command queue
     */
    std::shared_ptr<MTL::CommandQueue> _commandQueue{nullptr};

    /**
     * @brief The Metal shader library
     */
    std::shared_ptr<MTL::Library> _library{nullptr};

    /**
     * @brief context used for rendering, it is responsible for managing the frames and their underlying images
     */
    std::unique_ptr<RenderContext> _renderContext{nullptr};

    /**
     * @brief Pipeline used for rendering, it should be set up by the concrete sample
     */
    std::unique_ptr<RenderPass> _renderPass{nullptr};
    std::shared_ptr<MTL::RenderPassDescriptor> _renderPassDescriptor{nullptr};

    std::unique_ptr<GUI> _gui{nullptr};
};

}// namespace vox