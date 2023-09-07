//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <array>

#include "math/vector3.h"
#include "controls/spherical.h"
#include "components/script.h"

namespace vox::control {
/**
 * The camera's roaming controller, can move up and down, left and right, and rotate the viewing angle.
 */
class FreeControl : public Script {
public:
    explicit FreeControl(Entity *entity);

    void on_script_disable() override;

    void on_script_enable() override;

    void on_destroy() override;

    void on_update(float delta) override;

    void input_event(const vox::InputEvent &input_event) override;

    void resize(uint32_t win_width, uint32_t win_height, uint32_t fb_width, uint32_t fb_height) override;

public:
    /**
     * Keyboard press event.
     */
    void OnKeyDown(KeyCode key);

    /**
     * Keyboard up event.
     */
    void OnKeyUp(KeyCode key);

public:
    /**
     * Mouse press event.
     */
    void OnMouseDown(double xpos, double ypos);

    /**
     * Mouse up event.
     */
    void OnMouseUp();

    /**
     * Mouse movement event.
     */
    void OnMouseMove(double client_x, double client_y);

    /**
     * The angle of rotation around the y axis and the x axis respectively.
     * @param alpha - Radian to rotate around the y axis
     * @param beta - Radian to rotate around the x axis
     */
    void Rotate(float alpha = 0, float beta = 0);

    /**
     * must UpdateSpherical after quaternion has been changed
     * @example
     * Entity#lookAt([0,1,0],[0,1,0]);
     * AFreeControls#UpdateSpherical();
     */
    void UpdateSpherical();

private:
    Vector3F forward_;
    Vector3F right_;

    /**
     * Movement distance per second, the unit is the unit before MVP conversion.
     */
    float movement_speed_ = 1.0;

    /**
     * Rotate speed.
     */
    float rotate_speed_ = 1.0;

    /**
     * Simulate a ground.
     */
    bool floor_mock_ = false;

    /**
     * Simulated ground height.
     */
    float floor_y_ = 0;

    /**
     * Only rotate when press=true
     */
    bool press_ = false;

    /**
     * Radian of spherical.theta.
     */
    float theta_ = 0;

    /**
     * Radian of spherical.phi.
     */
    float phi_ = 0;

    bool move_forward_ = false;
    bool move_backward_ = false;
    bool move_left_ = false;
    bool move_right_ = false;

    Vector3F v3_cache_;
    Spherical spherical_;
    Vector2F rotate_{};

    bool enable_event_ = true;
    uint32_t width_ = 1000;
    uint32_t height_ = 1000;
};

}// namespace vox::control