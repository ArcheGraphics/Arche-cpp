//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "orbit_control.h"
#include "../entity.h"

namespace vox {
namespace control {
OrbitControl::OrbitControl(Entity *entity) : Script(entity),
                                             _cameraEntity(entity) {
}

void OrbitControl::onDisable() {
    _enableEvent = false;
    _enableMove = false;
}

void OrbitControl::onEnable() {
    _enableEvent = true;
}

void OrbitControl::onDestroy() {
    onDisable();
}

void OrbitControl::resize(uint32_t win_width, uint32_t win_height,
                          uint32_t fb_width, uint32_t fb_height) {
    _width = win_width;
    _height = win_height;
}

void OrbitControl::inputEvent(const InputEvent &inputEvent) {
    if (_enableEvent) {
        if (inputEvent.source() == EventSource::Keyboard) {
            const auto &key_event = static_cast<const KeyInputEvent &>(inputEvent);
            onKeyDown(key_event.code());
        } else if (inputEvent.source() == EventSource::Mouse) {
            const auto &mouse_button = static_cast<const MouseButtonInputEvent &>(inputEvent);
            if (mouse_button.action() == MouseAction::Down) {
                onMouseDown(mouse_button.button(), mouse_button.pos_x(), mouse_button.pos_y());
                _enableMove = true;
            } else if (mouse_button.action() == MouseAction::Up) {
                onMouseUp();
                _enableMove = false;
            }

            if (_enableMove && mouse_button.action() == MouseAction::Move) {
                onMouseMove(mouse_button.pos_x(), mouse_button.pos_y());
            }
        } else if (inputEvent.source() == EventSource::Scroll) {
            const auto &scroll_event = static_cast<const ScrollInputEvent &>(inputEvent);
            onMouseWheel(scroll_event.offset_x(), scroll_event.offset_y());
        } else if (inputEvent.source() == EventSource::Touchscreen) {
            // TODO
        }
    }
}

void OrbitControl::onUpdate(float dtime) {
    if (!enabled()) return;

    const auto &position = _cameraEntity->transform->position();
    _offset = position - target;
    _spherical.setFromVec3(_offset);

    if (autoRotate && _state == STATE::NONE) {
        rotateLeft(autoRotationAngle(dtime));
    }

    _spherical._theta += _sphericalDelta._theta;
    _spherical._phi += _sphericalDelta._phi;

    _spherical._theta = std::max(minAzimuthAngle, std::min(maxAzimuthAngle, _spherical._theta));
    _spherical._phi = std::max(minPolarAngle, std::min(maxPolarAngle, _spherical._phi));
    _spherical.makeSafe();

    if (_scale != 1) {
        _zoomFrag = _spherical._radius * (_scale - 1);
    }

    _spherical._radius += _zoomFrag;
    _spherical._radius = std::max(minDistance, std::min(maxDistance, _spherical._radius));

    target = target + _panOffset;
    _spherical.setToVec3(_offset);
    _position = target;
    _position = _position + _offset;

    _cameraEntity->transform->setPosition(_position);
    _cameraEntity->transform->lookAt(target, up);

    if (enableDamping == true) {
        _sphericalDump._theta *= 1 - dampingFactor;
        _sphericalDump._phi *= 1 - dampingFactor;
        _zoomFrag *= 1 - zoomFactor;

        if (_isMouseUp) {
            _sphericalDelta._theta = _sphericalDump._theta;
            _sphericalDelta._phi = _sphericalDump._phi;
        } else {
            _sphericalDelta.set(0, 0, 0);
        }
    } else {
        _sphericalDelta.set(0, 0, 0);
        _zoomFrag = 0;
    }

    _scale = 1;
    _panOffset = Vector3F(0, 0, 0);
}

float OrbitControl::autoRotationAngle(float dtime) {
    return (autoRotateSpeed / 1000) * dtime;
}

float OrbitControl::zoomScale() {
    return std::pow(0.95, zoomSpeed);
}

void OrbitControl::rotateLeft(float radian) {
    _sphericalDelta._theta -= radian;
    if (enableDamping) {
        _sphericalDump._theta = -radian;
    }
}

void OrbitControl::rotateUp(float radian) {
    _sphericalDelta._phi -= radian;
    if (enableDamping) {
        _sphericalDump._phi = -radian;
    }
}

void OrbitControl::panLeft(float distance, const Matrix4x4F &worldMatrix) {
    _vPan = Vector3F(worldMatrix[0], worldMatrix[1], worldMatrix[2]);
    _vPan = _vPan * distance;
    _panOffset = _panOffset + _vPan;
}

void OrbitControl::panUp(float distance, const Matrix4x4F &worldMatrix) {
    _vPan = Vector3F(worldMatrix[4], worldMatrix[5], worldMatrix[6]);
    _vPan = _vPan * distance;
    _panOffset = _panOffset + _vPan;
}

void OrbitControl::pan(float deltaX, float deltaY) {
    // perspective only
    Point3F position = _cameraEntity->transform->position();
    _vPan = position - target;
    auto targetDistance = _vPan.length();

    targetDistance *= (fov / 2) * (M_PI / 180);

    panLeft(-2 * deltaX * (targetDistance / float(_width)), _cameraEntity->transform->worldMatrix());
    panUp(2 * deltaY * (targetDistance / float(_height)), _cameraEntity->transform->worldMatrix());
}

void OrbitControl::zoomIn(float zoomScale) {
    _scale *= zoomScale;
}

void OrbitControl::zoomOut(float zoomScale) {
    _scale /= zoomScale;
}

//MARK: - Mouse
void OrbitControl::handleMouseDownRotate(double xpos, double ypos) {
    _rotateStart = Vector2F(xpos, ypos);
}

void OrbitControl::handleMouseDownZoom(double xpos, double ypos) {
    _zoomStart = Vector2F(xpos, ypos);
}

void OrbitControl::handleMouseDownPan(double xpos, double ypos) {
    _panStart = Vector2F(xpos, ypos);
}

void OrbitControl::handleMouseMoveRotate(double xpos, double ypos) {
    _rotateEnd = Vector2F(xpos, ypos);
    _rotateDelta = _rotateEnd - _rotateStart;

    rotateLeft(2 * M_PI * (_rotateDelta.x / float(_width)) * rotateSpeed);
    rotateUp(2 * M_PI * (_rotateDelta.y / float(_height)) * rotateSpeed);

    _rotateStart = _rotateEnd;
}

void OrbitControl::handleMouseMoveZoom(double xpos, double ypos) {
    _zoomEnd = Vector2F(xpos, ypos);
    _zoomDelta = _zoomEnd - _zoomStart;

    if (_zoomDelta.y > 0) {
        zoomOut(zoomScale());
    } else if (_zoomDelta.y < 0) {
        zoomIn(zoomScale());
    }

    _zoomStart = _zoomEnd;
}

void OrbitControl::handleMouseMovePan(double xpos, double ypos) {
    _panEnd = Vector2F(xpos, ypos);
    _panDelta = _panEnd - _panStart;

    pan(_panDelta.x, _panDelta.y);

    _panStart = _panEnd;
}

void OrbitControl::handleMouseWheel(double xoffset, double yoffset) {
    if (yoffset < 0) {
        zoomIn(zoomScale());
    } else if (yoffset > 0) {
        zoomOut(zoomScale());
    }
}

void OrbitControl::onMouseDown(MouseButton button, double xpos, double ypos) {
    if (enabled() == false) return;

    _isMouseUp = false;

    switch (button) {
        case MouseButton::Right:
            if (enableRotate == false) return;

            handleMouseDownRotate(xpos, ypos);
            _state = STATE::ROTATE;
            break;
        case MouseButton::Middle:
            if (enableZoom == false) return;

            handleMouseDownZoom(xpos, ypos);
            _state = STATE::ZOOM;
            break;

            // Pan turn to be GUI buttom
        // case MouseButton::Right:
        //     if (enablePan == false) return;
        //
        //    handleMouseDownPan(xpos, ypos);
        //    _state = STATE::PAN;
        //    break;
        default:
            break;
    }
}

void OrbitControl::onMouseMove(double xpos, double ypos) {
    if (enabled() == false) return;

    switch (_state) {
        case STATE::ROTATE:
            if (enableRotate == false) return;

            handleMouseMoveRotate(xpos, ypos);
            break;

        case STATE::ZOOM:
            if (enableZoom == false) return;

            handleMouseMoveZoom(xpos, ypos);
            break;

        case STATE::PAN:
            if (enablePan == false) return;

            handleMouseMovePan(xpos, ypos);
            break;
        default:
            break;
            ;
    }
}

void OrbitControl::onMouseUp() {
    if (enabled() == false) return;

    _isMouseUp = true;
    _state = STATE::NONE;
}

void OrbitControl::onMouseWheel(double xoffset, double yoffset) {
    if (enabled() == false || enableZoom == false ||
        (_state != STATE::NONE && _state != STATE::ROTATE))
        return;

    handleMouseWheel(xoffset, yoffset);
}

//MARK: - KeyBoard
void OrbitControl::handleKeyDown(KeyCode key) {
    switch (key) {
        case KeyCode::Up:
            pan(0, keyPanSpeed);
            break;
        case KeyCode::Down:
            pan(0, -keyPanSpeed);
            break;
        case KeyCode::Left:
            pan(keyPanSpeed, 0);
            break;
        case KeyCode::Right:
            pan(-keyPanSpeed, 0);
            break;
        default:
            break;
    }
}

void OrbitControl::onKeyDown(KeyCode key) {
    if (enabled() == false || enableKeys == false || enablePan == false) return;

    handleKeyDown(key);
}

//MARK: - Touch
void OrbitControl::handleTouchStartRotate() {
}

void OrbitControl::handleTouchStartZoom() {
}

void OrbitControl::handleTouchStartPan() {
}

void OrbitControl::handleTouchMoveRotate() {
}

void OrbitControl::handleTouchMoveZoom() {
}

void OrbitControl::handleTouchMovePan() {
}

void OrbitControl::onTouchStart() {
}

void OrbitControl::onTouchMove() {
}

void OrbitControl::onTouchEnd() {
}

}
}// namespace vox::control
