//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "math/matrix4x4.h"
#include "math/vector2.h"
#include "math/vector3.h"
#include "controls/spherical.h"
#include "components/script.h"

namespace vox::control {
/**
 * The camera's track controller, can rotate, zoom, pan, support mouse and touch events.
 */
class OrbitControl : public Script {
public:
    struct STATE {
        enum Enum { NONE = -1,
                    ROTATE = 0,
                    ZOOM = 1,
                    PAN = 2,
                    TOUCH_ROTATE = 3,
                    TOUCH_ZOOM = 4,
                    TOUCH_PAN = 5 };
    };

    struct Keys {
        enum Enum { LEFT = 37,
                    UP = 38,
                    RIGHT = 39,
                    BOTTOM = 40 };
    };

    // Control keys.
    struct MouseButtons {
        enum Enum { ORBIT = 0,
                    ZOOM = 1,
                    PAN = 2 };
    };

    struct TouchFingers {
        enum Enum { ORBIT = 1,
                    ZOOM = 2,
                    PAN = 3 };
    };

    float fov_ = 45;
    // Target position.
    Point3F target_ = Point3F();
    // Up vector
    Vector3F up_ = Vector3F(0, 1, 0);
    /**
     * The minimum distance, the default is 0.1, should be greater than 0.
     */
    float min_distance_ = 0.1;
    /**
     * The maximum distance, the default is infinite, should be greater than the minimum distance
     */
    float max_distance_ = std::numeric_limits<float>::infinity();
    /**
     * Minimum zoom speed, the default is 0.0.
     */
    float min_zoom_ = 0;
    /**
     * Maximum zoom speed, the default is positive infinity.
     */
    float max_zoom_ = std::numeric_limits<float>::infinity();

    /**
     * Whether to enable camera damping, the default is true.
     */
    bool enable_damping_ = true;
    /**
     * Rotation damping parameter, default is 0.1 .
     */
    float damping_factor_ = 0.1;
    /**
     * Whether to enable rotation, the default is true.
     */
    bool enable_rotate_ = true;
    /**
     * Rotation speed, default is 1.0 .
     */
    float rotate_speed_ = 1.0;
    /**
     * Whether to enable zoom, the default is true.
     */
    bool enable_zoom_ = true;
    /**
     * Zoom damping parameter, default is 0.2 .
     */
    float zoom_factor_ = 0.2;
    /**
     * Camera zoom speed, the default is 1.0.
     */
    float zoom_speed_ = 1.0;
    /**
     * Whether to enable translation, the default is true.
     */
    bool enable_pan_ = true;
    /**
     * Keyboard translation speed, the default is 7.0 .
     */
    float key_pan_speed_ = 7.0;
    /**
     * Whether to enable keyboard.
     */
    bool enable_keys_ = true;
    /**
     * The minimum radian in the vertical direction, the default is 0 radian, the value range is 0 - Math.PI.
     */
    float min_polar_angle_ = 0;
    /**
     * The maximum radian in the vertical direction, the default is Math.PI, and the value range is 0 - Math.PI.
     */
    float max_polar_angle_ = M_PI;
    /**
     * The minimum radian in the horizontal direction, the default is negative infinity.
     */
    float min_azimuth_angle_ = -std::numeric_limits<float>::infinity();
    /**
     * The maximum radian in the horizontal direction, the default is positive infinity.
     */
    float max_azimuth_angle_ = std::numeric_limits<float>::infinity();
    /**
     * Whether to automatically rotate the camera, the default is false.
     */
    bool auto_rotate_ = false;
    /** The radian of automatic rotation per second. */
    float auto_rotate_speed_ = M_PI;

public:
    explicit OrbitControl(Entity *entity);

    void on_script_disable() override;

    void on_script_enable() override;

    void on_destroy() override;

    void on_update(float delta_time) override;

    void input_event(const vox::InputEvent &input_event) override;

    void resize(uint32_t win_width, uint32_t win_height, uint32_t fb_width, uint32_t fb_height) override;

public:
    /**
     * Get the radian of automatic rotation.
     */
    [[nodiscard]] float AutoRotationAngle(float delta_time) const;

    [[nodiscard]] float ZoomScale() const;

    /**
     * Rotate to the left by a certain radian.
     * @param radian - Radian value of rotation
     */
    void RotateLeft(float radian);

    /**
     * Rotate to the right by a certain radian.
     * @param radian - Radian value of rotation
     */
    void RotateUp(float radian);

    /**
     * Pan left.
     */
    void PanLeft(float distance, const Matrix4x4F &world_matrix);

    /**
     * Pan right.
     */
    void PanUp(float distance, const Matrix4x4F &world_matrix);

    /**
     * Pan.
     * @param delta_x - The amount of translation from the screen distance in the x direction
     * @param delta_y - The amount of translation from the screen distance in the y direction
     */
    void Pan(float delta_x, float delta_y);

    /**
     * Zoom in.
     */
    void ZoomIn(float zoom_scale);

    /**
     * Zoom out.
     */
    void ZoomOut(float zoom_scale);

public:
    /**
     * Rotation parameter update on mouse click.
     */
    void HandleMouseDownRotate(double xpos, double ypos);

    /**
     * Zoom parameter update on mouse click.
     */
    void HandleMouseDownZoom(double xpos, double ypos);

    /**
     * Pan parameter update on mouse click.
     */
    void HandleMouseDownPan(double xpos, double ypos);

    /**
     * Rotation parameter update when the mouse moves.
     */
    void HandleMouseMoveRotate(double xpos, double ypos);

    /**
     * Zoom parameters update when the mouse moves.
     */
    void HandleMouseMoveZoom(double xpos, double ypos);

    /**
     * Pan parameters update when the mouse moves.
     */
    void HandleMouseMovePan(double xpos, double ypos);

    /**
     * Zoom parameter update when the mouse wheel is scrolled.
     */
    void HandleMouseWheel(double xoffset, double yoffset);

    /**
     * Total handling of mouse down events.
     */
    void OnMouseDown(MouseButton button, double xpos, double ypos);

    /**
     * Total handling of mouse movement events.
     */
    void OnMouseMove(double xpos, double ypos);

    /**
     * Total handling of mouse up events.
     */
    void OnMouseUp();

    /**
     * Total handling of mouse wheel events.
     */
    void OnMouseWheel(double xoffset, double yoffset);

public:
    /**
     * Pan parameter update when keyboard is pressed.
     */
    void HandleKeyDown(KeyCode key);

    /**
     * Total handling of keyboard down events.
     */
    void OnKeyDown(KeyCode key);

public:
    /**
     * Rotation parameter update when touch is dropped.
     */
    void HandleTouchStartRotate();

    /**
     * Zoom parameter update when touch down.
     */
    void HandleTouchStartZoom();

    /**
     * Update the translation parameter when touch down.
     */
    void HandleTouchStartPan();

    /**
     * Rotation parameter update when touch to move.
     */
    void HandleTouchMoveRotate();

    /**
     * Zoom parameter update when touch to move.
     */
    void HandleTouchMoveZoom();

    /**
     * Pan parameter update when touch moves.
     */
    void HandleTouchMovePan();

    /**
     * Total handling of touch start events.
     */
    void OnTouchStart();

    /**
     * Total handling of touch movement events.
     */
    void OnTouchMove();

    /**
     * Total handling of touch end events.
     */
    void OnTouchEnd();

private:
    Entity *camera_entity_;

    Point3F position_;
    Vector3F offset_;
    Spherical spherical_;
    Spherical spherical_delta_;
    Spherical spherical_dump_;
    float zoom_frag_ = 0;
    float scale_ = 1;
    Vector3F pan_offset_;
    bool is_mouse_up_ = true;
    Vector3F v_pan_;
    STATE::Enum state_ = STATE::Enum::NONE;
    Vector2F rotate_start_;
    Vector2F rotate_end_;
    Vector2F rotate_delta_;
    Vector2F pan_start_;
    Vector2F pan_end_;
    Vector2F pan_delta_;
    Vector2F zoom_start_;
    Vector2F zoom_end_;
    Vector2F zoom_delta_;

    bool enable_event_ = true;
    bool enable_move_ = false;
    uint32_t width_ = 1000;
    uint32_t height_ = 1000;
};

}// namespace vox::control