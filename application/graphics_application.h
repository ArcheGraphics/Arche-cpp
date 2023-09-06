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
#include "scene.h"
#include "gui.h"

namespace vox {
class MetalApplication : public Application {
public:
    MetalApplication();

    virtual ~MetalApplication() = default;

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

    std::unique_ptr<GUI> _gui{nullptr};
};

}// namespace vox