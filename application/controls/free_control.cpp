//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "controls/free_control.h"

#include "math/math_utils.h"
#include "ecs/entity.h"

namespace vox::control {
FreeControl::FreeControl(Entity *entity) : Script(entity) {
    // init spherical
    UpdateSpherical();
}

void FreeControl::on_script_disable() { enable_event_ = false; }

void FreeControl::on_script_enable() { enable_event_ = true; }

void FreeControl::on_destroy() { on_script_disable(); }

void FreeControl::resize(uint32_t win_width, uint32_t win_height, uint32_t fb_width, uint32_t fb_height) {
    width_ = win_width;
    height_ = win_height;
}

void FreeControl::input_event(const vox::InputEvent &input_event) {
    if (enable_event_) {
        if (input_event.get_source() == EventSource::Keyboard) {
            const auto &key_event = static_cast<const KeyInputEvent &>(input_event);
            if (key_event.get_action() == KeyAction::Down) {
                OnKeyDown(key_event.get_code());
            } else if (key_event.get_action() == KeyAction::Up) {
                OnKeyUp(key_event.get_code());
            }
        } else if (input_event.get_source() == EventSource::Mouse) {
            const auto &mouse_button = static_cast<const MouseButtonInputEvent &>(input_event);
            if (mouse_button.get_action() == MouseAction::Down) {
                OnMouseDown(mouse_button.get_pos_x(), mouse_button.get_pos_y());
            } else if (mouse_button.get_action() == MouseAction::Up) {
                OnMouseUp();
            } else if (mouse_button.get_action() == MouseAction::Move) {
                OnMouseMove(mouse_button.get_pos_x(), mouse_button.get_pos_y());
            }
        } else if (input_event.get_source() == EventSource::Scroll) {
        } else if (input_event.get_source() == EventSource::Touchscreen) {
            // TODO
        }
    }
}

void FreeControl::OnKeyDown(KeyCode key) {
    switch (key) {
        case KeyCode::W:
        case KeyCode::Up:
            move_forward_ = true;
            break;

        case KeyCode::S:
        case KeyCode::Down:
            move_backward_ = true;
            break;

        case KeyCode::A:
        case KeyCode::Left:
            move_left_ = true;
            break;

        case KeyCode::D:
        case KeyCode::Right:
            move_right_ = true;
            break;

        default:
            break;
    }
}

void FreeControl::OnKeyUp(KeyCode key) {
    switch (key) {
        case KeyCode::W:
        case KeyCode::Up:
            move_forward_ = false;
            break;

        case KeyCode::S:
        case KeyCode::Down:
            move_backward_ = false;
            break;

        case KeyCode::A:
        case KeyCode::Left:
            move_left_ = false;
            break;

        case KeyCode::D:
        case KeyCode::Right:
            move_right_ = false;
            break;

        default:
            break;
    }
}

void FreeControl::OnMouseDown(double xpos, double ypos) {
    press_ = true;
    rotate_.x = static_cast<float>(xpos);
    rotate_.y = static_cast<float>(ypos);
}

void FreeControl::OnMouseUp() { press_ = false; }

void FreeControl::OnMouseMove(double client_x, double client_y) {
    if (!press_) return;
    if (!is_enabled()) return;

    const auto kMovementX = client_x - rotate_.x;
    const auto kMovementY = client_y - rotate_.y;
    rotate_.x = static_cast<float>(client_x);
    rotate_.y = static_cast<float>(client_y);
    const auto kFactorX = 180.0 / width_;
    const auto kFactorY = 180.0 / height_;
    const auto kActualX = kMovementX * kFactorX;
    const auto kActualY = kMovementY * kFactorY;

    Rotate(-static_cast<float>(kActualX), static_cast<float>(kActualY));
}

void FreeControl::Rotate(float alpha, float beta) {
    theta_ += degreesToRadians(alpha);
    phi_ += degreesToRadians(beta);
    phi_ = clamp<float>(phi_, 1e-6, M_PI - 1e-6);
    spherical_.theta_ = theta_;
    spherical_.phi_ = phi_;
    spherical_.SetToVec3(v3_cache_);
    Point3F offset = get_entity()->transform->get_position() + v3_cache_;
    v3_cache_ = Vector3F(offset.x, offset.y, offset.y);
    get_entity()->transform->look_at(offset, Vector3F(0, 1, 0));
}

void FreeControl::on_update(float delta) {
    if (!is_enabled()) return;

    const auto kActualMoveSpeed = delta * movement_speed_;
    forward_ = get_entity()->transform->get_world_forward();
    right_ = get_entity()->transform->get_world_right();

    if (move_forward_) {
        get_entity()->transform->translate(forward_ * kActualMoveSpeed, false);
    }
    if (move_backward_) {
        get_entity()->transform->translate(forward_ * (-kActualMoveSpeed), false);
    }
    if (move_left_) {
        get_entity()->transform->translate(right_ * (-kActualMoveSpeed), false);
    }
    if (move_right_) {
        get_entity()->transform->translate(right_ * kActualMoveSpeed, false);
    }

    if (floor_mock_) {
        const auto kPosition = get_entity()->transform->get_position();
        if (kPosition.y != floor_y_) {
            get_entity()->transform->set_position(kPosition.x, floor_y_, kPosition.z);
        }
    }
}

void FreeControl::UpdateSpherical() {
    v3_cache_ = get_entity()->transform->get_rotation_quaternion() * Vector3F(0, 0, -1);
    spherical_.SetFromVec3(v3_cache_);
    theta_ = spherical_.theta_;
    phi_ = spherical_.phi_;
}

}// namespace vox::control
