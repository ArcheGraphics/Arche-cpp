//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <cstddef>
#include <cstdint>

namespace vox {
class Platform;

enum class EventSource {
    Keyboard,
    Mouse,
    Touchscreen,
    Scroll,
};

class InputEvent {
public:
    explicit InputEvent(EventSource source);

    [[nodiscard]] EventSource get_source() const;

private:
    EventSource source;
};

enum class KeyCode {
    Unknown,
    Space,
    Apostrophe, /* ' */
    Comma,      /* , */
    Minus,      /* - */
    Period,     /* . */
    Slash,      /* / */
    _0,
    _1,
    _2,
    _3,
    _4,
    _5,
    _6,
    _7,
    _8,
    _9,
    Semicolon, /* ; */
    Equal,     /* = */
    A,
    B,
    C,
    D,
    E,
    F,
    G,
    H,
    I,
    J,
    K,
    L,
    M,
    N,
    O,
    P,
    Q,
    R,
    S,
    T,
    U,
    V,
    W,
    X,
    Y,
    Z,
    LeftBracket,  /* [ */
    Backslash,    /* \ */
    RightBracket, /* ] */
    GraveAccent,  /* ` */
    Escape,
    Enter,
    Tab,
    Backspace,
    Insert,
    DelKey,
    Right,
    Left,
    Down,
    Up,
    PageUp,
    PageDown,
    Home,
    End,
    Back,
    CapsLock,
    ScrollLock,
    NumLock,
    PrintScreen,
    Pause,
    F1,
    F2,
    F3,
    F4,
    F5,
    F6,
    F7,
    F8,
    F9,
    F10,
    F11,
    F12,
    KP_0,
    KP_1,
    KP_2,
    KP_3,
    KP_4,
    KP_5,
    KP_6,
    KP_7,
    KP_8,
    KP_9,
    KP_Decimal,
    KP_Divide,
    KP_Multiply,
    KP_Subtract,
    KP_Add,
    KP_Enter,
    KP_Equal,
    LeftShift,
    LeftControl,
    LeftAlt,
    RightShift,
    RightControl,
    RightAlt
};

enum class KeyAction {
    Down,
    Up,
    Repeat,
    Unknown
};

class KeyInputEvent : public InputEvent {
public:
    KeyInputEvent(KeyCode code, KeyAction action);

    [[nodiscard]] KeyCode get_code() const;

    [[nodiscard]] KeyAction get_action() const;

private:
    KeyCode code;

    KeyAction action;
};

enum class MouseButton {
    Left,
    Right,
    Middle,
    Back,
    Forward,
    Unknown
};

enum class MouseAction {
    Down,
    Up,
    Move,
    Unknown
};

class MouseButtonInputEvent : public InputEvent {
public:
    MouseButtonInputEvent(MouseButton button, MouseAction action, float pos_x, float pos_y);

    [[nodiscard]] MouseButton get_button() const;

    [[nodiscard]] MouseAction get_action() const;

    [[nodiscard]] float get_pos_x() const;

    [[nodiscard]] float get_pos_y() const;

private:
    MouseButton button;

    MouseAction action;

    float pos_x;

    float pos_y;
};

// MARK: - Scroll Event
class ScrollInputEvent : public InputEvent {
public:
    ScrollInputEvent(float offset_x, float offset_y);

    [[nodiscard]] float get_offset_x() const;

    [[nodiscard]] float get_offset_y() const;

private:
    float offset_x_;

    float offset_y_;
};

enum class TouchAction {
    Down,
    Up,
    Move,
    Cancel,
    PointerDown,
    PointerUp,
    Unknown
};

class TouchInputEvent : public InputEvent {
public:
    TouchInputEvent(int32_t pointer_id, size_t pointer_count, TouchAction action, float pos_x, float pos_y);

    [[nodiscard]] TouchAction get_action() const;

    [[nodiscard]] int32_t get_pointer_id() const;

    [[nodiscard]] size_t get_touch_points() const;

    [[nodiscard]] float get_pos_x() const;

    [[nodiscard]] float get_pos_y() const;

private:
    TouchAction action;

    int32_t pointer_id;

    size_t touch_points;

    float pos_x;

    float pos_y;
};
}// namespace vox
