//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <functional>
#include <string>
#include "input.h"

namespace vox {

class Window {

public:
    using MouseButtonCallback = std::function<void(MouseButton button, Action action, float x, float y)>;
    using CursorPositionCallback = std::function<void(float x, float y)>;
    using WindowSizeCallback = std::function<void(uint32_t width, uint32_t height)>;
    using KeyCallback = std::function<void(Key key, KeyModifiers modifiers, Action action)>;
    using ScrollCallback = std::function<void(float dx, float dy)>;
    struct IWindowImpl {
        virtual ~IWindowImpl() noexcept = default;
    };

private:
    std::string _name;
    std::unique_ptr<IWindowImpl> _impl;
    uint32_t _width;
    uint32_t _height;

public:
    Window(std::string name, uint32_t width, uint32_t height) noexcept;
    ~Window() noexcept;
    Window(const Window &) = delete;
    Window(Window &&) = default;
    Window &operator=(Window &&) noexcept = default;
    Window &operator=(const Window &) noexcept = delete;

    [[nodiscard]] uint64_t native_handle() const noexcept;
    [[nodiscard]] bool should_close() const noexcept;
    void set_should_close(bool should_close) noexcept;
    [[nodiscard]] auto width() const noexcept { return _width; }
    [[nodiscard]] auto height() const noexcept { return _height; }
    [[nodiscard]] auto name() const noexcept { return std::string_view{_name}; }

    Window &set_mouse_callback(MouseButtonCallback cb) noexcept;
    Window &set_cursor_position_callback(CursorPositionCallback cb) noexcept;
    Window &set_window_size_callback(WindowSizeCallback cb) noexcept;
    Window &set_key_callback(KeyCallback cb) noexcept;
    Window &set_scroll_callback(ScrollCallback cb) noexcept;
    void poll_events() noexcept;
    [[nodiscard]] bool is_key_down(Key key) const noexcept;
    [[nodiscard]] bool is_mouse_button_down(MouseButton mb) const noexcept;
    [[nodiscard]] explicit operator bool() const noexcept { return !should_close(); }
};

}// namespace vox
