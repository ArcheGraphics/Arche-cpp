//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "input_events.h"

namespace vox {
InputEvent::InputEvent(EventSource source) : _source{source} {
}

EventSource InputEvent::source() const {
    return _source;
}

//MARK: - Key Event
KeyInputEvent::KeyInputEvent(KeyCode code, KeyAction action) : InputEvent{EventSource::Keyboard},
                                                               _code{code},
                                                               _action{action} {
}

KeyCode KeyInputEvent::code() const {
    return _code;
}

KeyAction KeyInputEvent::action() const {
    return _action;
}

//MARK: - Mouse Event
MouseButtonInputEvent::MouseButtonInputEvent(MouseButton button, MouseAction action, float pos_x, float pos_y) : InputEvent{EventSource::Mouse},
                                                                                                                 _button{button},
                                                                                                                 _action{action},
                                                                                                                 _pos_x{pos_x},
                                                                                                                 _pos_y{pos_y} {
}

MouseButton MouseButtonInputEvent::button() const {
    return _button;
}

MouseAction MouseButtonInputEvent::action() const {
    return _action;
}

float MouseButtonInputEvent::pos_x() const {
    return _pos_x;
}

float MouseButtonInputEvent::pos_y() const {
    return _pos_y;
}

//MARK: - Scroll Event
ScrollInputEvent::ScrollInputEvent(float offset_x, float offset_y) : InputEvent{EventSource::Scroll},
                                                                     _offset_x(offset_x),
                                                                     _offset_y(offset_y) {
}

float ScrollInputEvent::offset_x() const {
    return _offset_x;
}

float ScrollInputEvent::offset_y() const {
    return _offset_y;
}

//MARK: - Touch Event
TouchInputEvent::TouchInputEvent(int32_t pointer_id, std::size_t touch_points, TouchAction action, float pos_x, float pos_y)
    : InputEvent{EventSource::Touchscreen},
      _action{action},
      _pointer_id{pointer_id},
      _touch_points{touch_points},
      _pos_x{pos_x},
      _pos_y{pos_y} {
}

TouchAction TouchInputEvent::action() const {
    return _action;
}

int32_t TouchInputEvent::pointer_id() const {
    return _pointer_id;
}

std::size_t TouchInputEvent::touch_points() const {
    return _touch_points;
}

float TouchInputEvent::pos_x() const {
    return _pos_x;
}

float TouchInputEvent::pos_y() const {
    return _pos_y;
}
}// namespace vox
