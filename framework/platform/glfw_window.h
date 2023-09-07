//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "platform/window.h"

struct GLFWwindow;

namespace vox {
class Platform;

/**
 * @brief An implementation of GLFW, inheriting the behaviour of the Window interface
 */
class GlfwWindow : public Window {
public:
    GlfwWindow(Platform *platform, const Window::Properties &properties);

    ~GlfwWindow() override;

    std::unique_ptr<RenderContext> createRenderContext(MTL::Device &device) override;

    bool should_close() override;

    void process_events() override;

    void close() override;

    [[nodiscard]] float get_dpi_factor() const override;

    [[nodiscard]] float get_content_scale_factor() const override;

    GLFWwindow *get_handle() {
        return handle;
    }

private:
    GLFWwindow *handle = nullptr;
};
}// namespace vox
