//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#if defined(MACOS)
#define GLFW_EXPOSE_NATIVE_COCOA
#else
#define GLFW_EXPOSE_NATIVE_X11
#endif

#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include "imgui_impl_glfw.h"
#include "window.h"

namespace vox {

namespace detail {

struct WindowImpl : public Window::IWindowImpl {
    GLFWwindow *window;
    Window::MouseButtonCallback _mouse_button_callback;
    Window::CursorPositionCallback _cursor_position_callback;
    Window::WindowSizeCallback _window_size_callback;
    Window::KeyCallback _key_callback;
    Window::ScrollCallback _scroll_callback;
    uint64_t window_handle{};

    WindowImpl(uint32_t width, uint32_t height, char const *name) noexcept {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        window = glfwCreateWindow(width, height, name, nullptr, nullptr);
        // TODO: other platform
#if defined(MACOS)
        window_handle = reinterpret_cast<uint64_t>(glfwGetCocoaWindow(window));
#else
        window_handle = reinterpret_cast<uint64_t>(glfwGetX11Window(window));
#endif
        glfwSetWindowUserPointer(window, this);
        // TODO: imgui
        glfwSetMouseButtonCallback(window, [](GLFWwindow *window, int button, int action, int mods) noexcept {
            // if (ImGui::GetIO().WantCaptureMouse) {// ImGui is handling the mouse
            //     ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
            // } else {
            auto self = static_cast<WindowImpl *>(glfwGetWindowUserPointer(window));
            auto x = 0.0;
            auto y = 0.0;
            glfwGetCursorPos(self->window, &x, &y);
            if (auto &&cb = self->_mouse_button_callback) {
                cb(static_cast<MouseButton>(button), static_cast<Action>(action),
                   static_cast<float>(x), static_cast<float>(y));
            }
            // }
        });
        glfwSetCursorPosCallback(window, [](GLFWwindow *window, double x, double y) noexcept {
            auto self = static_cast<WindowImpl *>(glfwGetWindowUserPointer(window));
            if (auto &&cb = self->_cursor_position_callback) { cb(static_cast<float>(x), static_cast<float>(y)); }
        });
        glfwSetWindowSizeCallback(window, [](GLFWwindow *window, int width, int height) noexcept {
            auto self = static_cast<WindowImpl *>(glfwGetWindowUserPointer(window));
            if (auto &&cb = self->_window_size_callback) { cb(width, height); }
        });
        glfwSetKeyCallback(window, [](GLFWwindow *window, int key, int scancode, int action, int mods) noexcept {
            // if (ImGui::GetIO().WantCaptureKeyboard) {// ImGui is handling the keyboard
            //     ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
            // } else {
            auto self = static_cast<WindowImpl *>(glfwGetWindowUserPointer(window));
            if (auto &&cb = self->_key_callback) {
                cb(static_cast<Key>(key), mods, static_cast<Action>(action));
            }
            // }
        });
        glfwSetScrollCallback(window, [](GLFWwindow *window, double dx, double dy) noexcept {
            // if (ImGui::GetIO().WantCaptureMouse) {// ImGui is handling the mouse
            //     ImGui_ImplGlfw_ScrollCallback(window, dx, dy);
            // } else {
            auto self = static_cast<WindowImpl *>(glfwGetWindowUserPointer(window));
            if (auto &&cb = self->_scroll_callback) {
                cb(static_cast<float>(dx), static_cast<float>(dy));
            }
            // }
        });
        // glfwSetCharCallback(window, ImGui_ImplGlfw_CharCallback);
    }
    ~WindowImpl() noexcept override {
        glfwDestroyWindow(window);
        glfwTerminate();
    }
};

}// namespace detail

Window::Window(std::string name, uint32_t width, uint32_t height) noexcept
    : _width{width}, _height{height},
      _name{std::move(name)} {
    _impl = std::make_unique<detail::WindowImpl>(_width, _height, _name.c_str());
}

Window::~Window() noexcept = default;

uint64_t Window::native_handle() const noexcept {
    return static_cast<detail::WindowImpl *>(_impl.get())->window_handle;
}

bool Window::should_close() const noexcept {
    return glfwWindowShouldClose(static_cast<detail::WindowImpl *>(_impl.get())->window);
}

Window &Window::set_mouse_callback(Window::MouseButtonCallback cb) noexcept {
    static_cast<detail::WindowImpl *>(_impl.get())->_mouse_button_callback = std::move(cb);
    return *this;
}

Window &Window::set_cursor_position_callback(Window::CursorPositionCallback cb) noexcept {
    static_cast<detail::WindowImpl *>(_impl.get())->_cursor_position_callback = std::move(cb);
    return *this;
}

Window &Window::set_window_size_callback(Window::WindowSizeCallback cb) noexcept {
    static_cast<detail::WindowImpl *>(_impl.get())->_window_size_callback = std::move(cb);
    return *this;
}

Window &Window::set_key_callback(Window::KeyCallback cb) noexcept {
    static_cast<detail::WindowImpl *>(_impl.get())->_key_callback = std::move(cb);
    return *this;
}

Window &Window::set_scroll_callback(Window::ScrollCallback cb) noexcept {
    static_cast<detail::WindowImpl *>(_impl.get())->_scroll_callback = std::move(cb);
    return *this;
}

void Window::poll_events() noexcept {
    glfwPollEvents();
}

void Window::set_should_close(bool should_close) noexcept {
    glfwSetWindowShouldClose(static_cast<detail::WindowImpl *>(_impl.get())->window, should_close);
}

bool Window::is_key_down(Key key) const noexcept {
    return glfwGetKey(static_cast<detail::WindowImpl *>(_impl.get())->window, static_cast<int>(key)) != GLFW_RELEASE;
}

bool Window::is_mouse_button_down(MouseButton mb) const noexcept {
    return glfwGetMouseButton(static_cast<detail::WindowImpl *>(_impl.get())->window, static_cast<int>(mb)) != GLFW_RELEASE;
}

}// namespace vox
