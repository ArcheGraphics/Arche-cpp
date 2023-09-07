//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "components/script.h"
#include "math/vector3.h"
#include "math/vector2.h"
#include "math/matrix4x4.h"
#include "spherical.h"

namespace vox {
namespace control {
/**
 * The camera's track controller, can rotate, zoom, pan, support mouse and touch events.
 */
class OrbitControl : public Script {
public:
    struct STATE {
        enum Enum {
            NONE = -1,
            ROTATE = 0,
            ZOOM = 1,
            PAN = 2,
            TOUCH_ROTATE = 3,
            TOUCH_ZOOM = 4,
            TOUCH_PAN = 5
        };
    };

    struct Keys {
        enum Enum {
            LEFT = 37,
            UP = 38,
            RIGHT = 39,
            BOTTOM = 40
        };
    };

    // Control keys.
    struct MouseButtons {
        enum Enum {
            ORBIT = 0,
            ZOOM = 1,
            PAN = 2
        };
    };

    struct TouchFingers {
        enum Enum {
            ORBIT = 1,
            ZOOM = 2,
            PAN = 3
        };
    };

    float fov = 45;
    // Target position.
    Point3F target = Point3F();
    // Up vector
    Vector3F up = Vector3F(0, 1, 0);
    /**
     * The minimum distance, the default is 0.1, should be greater than 0.
     */
    float minDistance = 0.1;
    /**
     * The maximum distance, the default is infinite, should be greater than the minimum distance
     */
    float maxDistance = std::numeric_limits<float>::infinity();
    /**
     * Minimum zoom speed, the default is 0.0.
     */
    float minZoom = 0;
    /**
     * Maximum zoom speed, the default is positive infinity.
     */
    float maxZoom = std::numeric_limits<float>::infinity();

    /**
     * Whether to enable camera damping, the default is true.
     */
    bool enableDamping = true;
    /**
     * Rotation damping parameter, default is 0.1 .
     */
    float dampingFactor = 0.1;
    /**
     * Whether to enable rotation, the default is true.
     */
    bool enableRotate = true;
    /**
     * Rotation speed, default is 1.0 .
     */
    float rotateSpeed = 1.0;
    /**
     * Whether to enable zoom, the default is true.
     */
    bool enableZoom = true;
    /**
     * Zoom damping parameter, default is 0.2 .
     */
    float zoomFactor = 0.2;
    /**
     * Camera zoom speed, the default is 1.0.
     */
    float zoomSpeed = 1.0;
    /**
     * Whether to enable translation, the default is true.
     */
    bool enablePan = true;
    /**
     * Keyboard translation speed, the default is 7.0 .
     */
    float keyPanSpeed = 7.0;
    /**
     * Whether to enable keyboard.
     */
    bool enableKeys = true;
    /**
     * The minimum radian in the vertical direction, the default is 0 radian, the value range is 0 - Math.PI.
     */
    float minPolarAngle = 0;
    /**
     * The maximum radian in the vertical direction, the default is Math.PI, and the value range is 0 - Math.PI.
     */
    float maxPolarAngle = M_PI;
    /**
     * The minimum radian in the horizontal direction, the default is negative infinity.
     */
    float minAzimuthAngle = -std::numeric_limits<float>::infinity();
    /**
     * The maximum radian in the horizontal direction, the default is positive infinity.
     */
    float maxAzimuthAngle = std::numeric_limits<float>::infinity();
    /**
     * Whether to automatically rotate the camera, the default is false.
     */
    bool autoRotate = false;
    /** The radian of automatic rotation per second. */
    float autoRotateSpeed = M_PI;

public:
    explicit OrbitControl(Entity *entity);

    void onDisable() override;

    void onEnable() override;

    void onDestroy() override;

    void onUpdate(float dtime) override;

    void inputEvent(const InputEvent &inputEvent) override;

    void resize(uint32_t win_width, uint32_t win_height,
                uint32_t fb_width, uint32_t fb_height) override;

public:
    /**
     * Get the radian of automatic rotation.
     */
    float autoRotationAngle(float dtime);

    float zoomScale();

    /**
     * Rotate to the left by a certain radian.
     * @param radian - Radian value of rotation
     */
    void rotateLeft(float radian);

    /**
     * Rotate to the right by a certain radian.
     * @param radian - Radian value of rotation
     */
    void rotateUp(float radian);

    /**
     * Pan left.
     */
    void panLeft(float distance, const Matrix4x4F &worldMatrix);

    /**
     * Pan right.
     */
    void panUp(float distance, const Matrix4x4F &worldMatrix);

    /**
     * Pan.
     * @param deltaX - The amount of translation from the screen distance in the x direction
     * @param deltaY - The amount of translation from the screen distance in the y direction
     */
    void pan(float deltaX, float deltaY);

    /**
     * Zoom in.
     */
    void zoomIn(float zoomScale);

    /**
     * Zoom out.
     */
    void zoomOut(float zoomScale);

public:
    /**
     * Rotation parameter update on mouse click.
     */
    void handleMouseDownRotate(double xpos, double ypos);

    /**
     * Zoom parameter update on mouse click.
     */
    void handleMouseDownZoom(double xpos, double ypos);

    /**
     * Pan parameter update on mouse click.
     */
    void handleMouseDownPan(double xpos, double ypos);

    /**
     * Rotation parameter update when the mouse moves.
     */
    void handleMouseMoveRotate(double xpos, double ypos);

    /**
     * Zoom parameters update when the mouse moves.
     */
    void handleMouseMoveZoom(double xpos, double ypos);

    /**
     * Pan parameters update when the mouse moves.
     */
    void handleMouseMovePan(double xpos, double ypos);

    /**
     * Zoom parameter update when the mouse wheel is scrolled.
     */
    void handleMouseWheel(double xoffset, double yoffset);

    /**
     * Total handling of mouse down events.
     */
    void onMouseDown(MouseButton button, double xpos, double ypos);

    /**
     * Total handling of mouse movement events.
     */
    void onMouseMove(double xpos, double ypos);

    /**
     * Total handling of mouse up events.
     */
    void onMouseUp();

    /**
     * Total handling of mouse wheel events.
     */
    void onMouseWheel(double xoffset, double yoffset);

public:
    /**
     * Pan parameter update when keyboard is pressed.
     */
    void handleKeyDown(KeyCode key);

    /**
     * Total handling of keyboard down events.
     */
    void onKeyDown(KeyCode key);

public:
    /**
     * Rotation parameter update when touch is dropped.
     */
    void handleTouchStartRotate();

    /**
     * Zoom parameter update when touch down.
     */
    void handleTouchStartZoom();

    /**
     * Update the translation parameter when touch down.
     */
    void handleTouchStartPan();

    /**
     * Rotation parameter update when touch to move.
     */
    void handleTouchMoveRotate();

    /**
     * Zoom parameter update when touch to move.
     */
    void handleTouchMoveZoom();

    /**
     * Pan parameter update when touch moves.
     */
    void handleTouchMovePan();

    /**
     * Total handling of touch start events.
     */
    void onTouchStart();

    /**
     * Total handling of touch movement events.
     */
    void onTouchMove();

    /**
     * Total handling of touch end events.
     */
    void onTouchEnd();

private:
    Entity *_cameraEntity;

    Point3F _position;
    Vector3F _offset;
    Spherical _spherical;
    Spherical _sphericalDelta;
    Spherical _sphericalDump;
    float _zoomFrag = 0;
    float _scale = 1;
    Vector3F _panOffset;
    bool _isMouseUp = true;
    Vector3F _vPan;
    STATE::Enum _state = STATE::Enum::NONE;
    Vector2F _rotateStart;
    Vector2F _rotateEnd;
    Vector2F _rotateDelta;
    Vector2F _panStart;
    Vector2F _panEnd;
    Vector2F _panDelta;
    Vector2F _zoomStart;
    Vector2F _zoomEnd;
    Vector2F _zoomDelta;

    bool _enableEvent = true;
    bool _enableMove = false;
    uint32_t _width = 1000;
    uint32_t _height = 1000;
};

}
}// namespace vox::control