//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "components/transform.h"

#include "math/matrix_utils.h"
#include "ecs/entity.h"

namespace vox {
Transform::Transform(Entity *entity) : Component(entity) {}

Point3F Transform::get_position() { return position_; }

void Transform::set_position(const Point3F &value) {
    position_ = value;
    set_dirty_flag_true(TransformFlag::LOCAL_MATRIX);
    update_world_position_flag();
}

Point3F Transform::get_world_position() {
    if (is_contain_dirty_flag(TransformFlag::WORLD_POSITION)) {
        if (get_parent_transform()) {
            world_position_ = getTranslation(get_world_matrix());
        } else {
            world_position_ = position_;
        }
        set_dirty_flag_false(TransformFlag::WORLD_POSITION);
    }
    return world_position_;
}

void Transform::set_world_position(const Point3F &value) {
    world_position_ = value;
    const auto kParent = get_parent_transform();
    if (kParent) {
        position_ = kParent->get_world_matrix().inverse() * value;
    } else {
        position_ = value;
    }
    set_position(position_);
    set_dirty_flag_false(TransformFlag::WORLD_POSITION);
}

Vector3F Transform::get_rotation() {
    if (is_contain_dirty_flag(TransformFlag::LOCAL_EULER)) {
        rotation_ = rotation_quaternion_.toEuler();
        rotation_ *= kRadianToDegree;// radians to degrees

        set_dirty_flag_false(TransformFlag::LOCAL_EULER);
    }
    return rotation_;
}

void Transform::set_rotation(const Vector3F &value) {
    rotation_ = value;
    set_dirty_flag_true(TransformFlag::LOCAL_MATRIX | TransformFlag::LOCAL_QUAT);
    set_dirty_flag_false(TransformFlag::LOCAL_EULER);
    update_world_rotation_flag();
}

Vector3F Transform::get_world_rotation() {
    if (is_contain_dirty_flag(TransformFlag::WORLD_EULER)) {
        world_rotation_ = get_world_rotation_quaternion().toEuler();
        world_rotation_ *= kRadianToDegree;// Radian to angle
        set_dirty_flag_false(TransformFlag::WORLD_EULER);
    }
    return world_rotation_;
}

void Transform::set_world_rotation(const Vector3F &value) {
    world_rotation_ = value;
    world_rotation_quaternion_ = QuaternionF::makeRotationEuler(degreesToRadians(value.x), degreesToRadians(value.y),
                                                                degreesToRadians(value.z));
    set_world_rotation_quaternion(world_rotation_quaternion_);
    set_dirty_flag_false(TransformFlag::WORLD_EULER);
}

QuaternionF Transform::get_rotation_quaternion() {
    if (is_contain_dirty_flag(TransformFlag::LOCAL_QUAT)) {
        rotation_quaternion_ = QuaternionF::makeRotationEuler(
            degreesToRadians(rotation_.x), degreesToRadians(rotation_.y), degreesToRadians(rotation_.z));
        set_dirty_flag_false(TransformFlag::LOCAL_QUAT);
    }
    return rotation_quaternion_;
}

void Transform::set_rotation_quaternion(const QuaternionF &value) {
    rotation_quaternion_ = value.normalized();
    set_dirty_flag_true(TransformFlag::LOCAL_MATRIX | TransformFlag::LOCAL_EULER);
    set_dirty_flag_false(TransformFlag::LOCAL_QUAT);
    update_world_rotation_flag();
}

QuaternionF Transform::get_world_rotation_quaternion() {
    if (is_contain_dirty_flag(TransformFlag::WORLD_QUAT)) {
        const auto kParent = get_parent_transform();
        if (kParent) {
            world_rotation_quaternion_ = kParent->get_world_rotation_quaternion() * get_rotation_quaternion();
        } else {
            world_rotation_quaternion_ = get_rotation_quaternion();
        }
        set_dirty_flag_false(TransformFlag::WORLD_QUAT);
    }
    return world_rotation_quaternion_;
}

void Transform::set_world_rotation_quaternion(const QuaternionF &value) {
    world_rotation_quaternion_ = value.normalized();
    const auto kParent = get_parent_transform();
    if (kParent) {
        auto temp_quat_0 = kParent->get_world_rotation_quaternion().inverse();
        rotation_quaternion_ = world_rotation_quaternion_ * temp_quat_0;
    } else {
        rotation_quaternion_ = world_rotation_quaternion_;
    }
    set_rotation_quaternion(rotation_quaternion_);
    set_dirty_flag_false(TransformFlag::WORLD_QUAT);
}

Vector3F Transform::get_scale() { return scale_; }

void Transform::set_scale(const Vector3F &value) {
    scale_ = value;
    set_dirty_flag_true(TransformFlag::LOCAL_MATRIX);
    update_world_scale_flag();
}

Vector3F Transform::get_lossy_world_scale() {
    if (is_contain_dirty_flag(TransformFlag::WORLD_SCALE)) {
        if (get_parent_transform()) {
            const auto kScaleMat = get_scale_matrix();
            lossy_world_scale_ = Vector3F(kScaleMat[0], kScaleMat[4], kScaleMat[8]);
        } else {
            lossy_world_scale_ = scale_;
        }
        set_dirty_flag_false(TransformFlag::WORLD_SCALE);
    }
    return lossy_world_scale_;
}

Matrix4x4F Transform::get_local_matrix() {
    if (is_contain_dirty_flag(TransformFlag::LOCAL_MATRIX)) {
        local_matrix_ = makeAffineMatrix(scale_, get_rotation_quaternion(), position_);
        set_dirty_flag_false(TransformFlag::LOCAL_MATRIX);
    }
    return local_matrix_;
}

void Transform::set_local_matrix(const Matrix4x4F &value) {
    local_matrix_ = value;
    decompose(local_matrix_, position_, rotation_quaternion_, scale_);
    set_dirty_flag_true(TransformFlag::LOCAL_EULER);
    set_dirty_flag_false(TransformFlag::LOCAL_MATRIX);
    update_all_world_flag();
}

Matrix4x4F Transform::get_world_matrix() {
    if (is_contain_dirty_flag(TransformFlag::WORLD_MATRIX)) {
        const auto kParent = get_parent_transform();
        if (kParent) {
            world_matrix_ = kParent->get_world_matrix() * get_local_matrix();
        } else {
            world_matrix_ = get_local_matrix();
        }
        set_dirty_flag_false(TransformFlag::WORLD_MATRIX);
    }
    return world_matrix_;
}

void Transform::set_world_matrix(const Matrix4x4F &value) {
    world_matrix_ = value;
    const auto kParent = get_parent_transform();
    if (kParent) {
        auto temp_mat_42 = kParent->get_world_matrix().inverse();
        local_matrix_ = value * temp_mat_42;
    } else {
        local_matrix_ = value;
    }
    set_local_matrix(local_matrix_);
    set_dirty_flag_false(TransformFlag::WORLD_MATRIX);
}

void Transform::set_position(float x, float y, float z) {
    position_ = Point3F(x, y, z);
    set_position(position_);
}

void Transform::set_rotation(float x, float y, float z) {
    rotation_ = Vector3F(x, y, z);
    set_rotation(rotation_);
}

void Transform::set_rotation_quaternion(float x, float y, float z, float w) {
    rotation_quaternion_ = QuaternionF(x, y, z, w);
    set_rotation_quaternion(rotation_quaternion_);
}

void Transform::set_scale(float x, float y, float z) {
    scale_ = Vector3F(x, y, z);
    set_scale(scale_);
}

void Transform::set_world_position(float x, float y, float z) {
    world_position_ = Point3F(x, y, z);
    set_world_position(world_position_);
}

void Transform::set_world_rotation(float x, float y, float z) {
    world_rotation_ = Vector3F(x, y, z);
    set_world_rotation(world_rotation_);
}

void Transform::set_world_rotation_quaternion(float x, float y, float z, float w) {
    world_rotation_quaternion_ = QuaternionF(x, y, z, w);
    set_world_rotation_quaternion(world_rotation_quaternion_);
}

Vector3F Transform::get_world_forward() {
    const auto &e = get_world_matrix();
    auto forward = Vector3F(-e[8], -e[9], -e[10]);
    return forward.normalized();
}

Vector3F Transform::get_world_right() {
    const auto &e = get_world_matrix();
    auto right = Vector3F(e[0], e[1], e[2]);
    return right.normalized();
}

Vector3F Transform::get_world_up() {
    const auto &e = get_world_matrix();
    auto up = Vector3F(e[4], e[5], e[6]);
    return up.normalized();
}

void Transform::translate(const Vector3F &translation, bool relative_to_local) {
    if (relative_to_local) {
        position_ = position_ + translation;
        set_position(position_);
    } else {
        world_position_ = world_position_ + translation;
        set_world_position(world_position_);
    }
}

void Transform::translate(float x, float y, float z, bool relative_to_local) {
    translate(Vector3F(x, y, z), relative_to_local);
}

void Transform::rotate(const Vector3F &rotation, bool relative_to_local) {
    rotate_xyz(rotation.x, rotation.y, rotation.z, relative_to_local);
}

void Transform::rotate(float x, float y, float z, bool relative_to_local) { rotate_xyz(x, y, z, relative_to_local); }

void Transform::rotate_by_axis(const Vector3F &axis, float angle, bool relative_to_local) {
    const auto kRad = angle * kDegreeToRadian;
    QuaternionF temp_quat_0(axis, kRad);
    rotate_by_quat(temp_quat_0, relative_to_local);
}

void Transform::look_at(const Point3F &world_position, const Vector3F &world_up) {
    const auto kPosition = this->get_world_position();
    if (std::abs(kPosition.x - world_position.x) < std::numeric_limits<float>::epsilon() &&
        std::abs(kPosition.y - world_position.y) < std::numeric_limits<float>::epsilon() &&
        std::abs(kPosition.z - world_position.z) < std::numeric_limits<float>::epsilon()) {
        return;
    }
    Matrix4x4F rot_mat = makeLookAtMatrix(kPosition, world_position, world_up);
    auto world_rotation_quaternion = getRotation(rot_mat);
    world_rotation_quaternion = world_rotation_quaternion.inverse();
    set_world_rotation_quaternion(world_rotation_quaternion);
}

std::unique_ptr<UpdateFlag> Transform::register_world_change_flag() { return update_flag_manager_.registration(); }

void Transform::parent_change() {
    is_parent_dirty_ = true;
    update_all_world_flag();
}

void Transform::update_world_position_flag() {
    if (!is_contain_dirty_flags(TransformFlag::WM_WP)) {
        world_associated_change(TransformFlag::WM_WP);
        const auto &node_children = entity_->children;
        for (const auto &i : node_children) {
            i->transform->update_world_position_flag();
        }
    }
}

void Transform::update_world_rotation_flag() {
    if (!is_contain_dirty_flags(TransformFlag::WM_WE_WQ)) {
        world_associated_change(TransformFlag::WM_WE_WQ);
        const auto &node_children = entity_->children;
        for (const auto &i : node_children) {
            // Rotation update of parent entity will trigger world position and rotation update of all child entity.
            i->transform->update_world_position_and_rotation_flag();
        }
    }
}

void Transform::update_world_position_and_rotation_flag() {
    if (!is_contain_dirty_flags(TransformFlag::WM_WP_WE_WQ)) {
        world_associated_change(TransformFlag::WM_WP_WE_WQ);
        const auto &node_children = entity_->children;
        for (const auto &i : node_children) {
            i->transform->update_world_position_and_rotation_flag();
        }
    }
}

void Transform::update_world_scale_flag() {
    if (!is_contain_dirty_flags(TransformFlag::WM_WS)) {
        world_associated_change(TransformFlag::WM_WS);
        const auto &node_children = entity_->children;
        for (const auto &i : node_children) {
            i->transform->update_world_position_and_scale_flag();
        }
    }
}

void Transform::update_world_position_and_scale_flag() {
    if (!is_contain_dirty_flags(TransformFlag::WM_WP_WS)) {
        world_associated_change(TransformFlag::WM_WP_WS);
        const auto &node_children = entity_->children;
        for (const auto &i : node_children) {
            i->transform->update_world_position_and_scale_flag();
        }
    }
}

void Transform::update_all_world_flag() {
    if (!is_contain_dirty_flags(TransformFlag::WM_WP_WE_WQ_WS)) {
        world_associated_change(TransformFlag::WM_WP_WE_WQ_WS);
        const auto &node_children = entity_->children;
        for (const auto &i : node_children) {
            i->transform->update_all_world_flag();
        }
    }
}

Transform *Transform::get_parent_transform() {
    if (!is_parent_dirty_) {
        return parent_transform_cache_;
    }
    Transform *parent_cache = nullptr;
    auto parent = entity_->get_parent();
    while (parent) {
        const auto &transform = parent->transform;
        if (transform) {
            parent_cache = transform;
            break;
        } else {
            parent = parent->get_parent();
        }
    }
    parent_transform_cache_ = parent_cache;
    is_parent_dirty_ = false;
    return parent_cache;
}

Matrix3x3F Transform::get_scale_matrix() {
    Matrix3x3F world_rot_sca_mat = get_world_matrix().matrix3();
    Quaternion inv_rotation = get_world_rotation_quaternion().inverse();
    Matrix3x3F inv_rotation_mat = inv_rotation.matrix3();
    return inv_rotation_mat * world_rot_sca_mat;
}

bool Transform::is_contain_dirty_flags(int target_dirty_flags) const {
    // todo
    return (dirty_flag_ & target_dirty_flags) == target_dirty_flags;
}

bool Transform::is_contain_dirty_flag(int type) const { return (dirty_flag_ & type) != 0; }

void Transform::set_dirty_flag_true(int type) { dirty_flag_ |= type; }

void Transform::set_dirty_flag_false(int type) { dirty_flag_ &= ~type; }

void Transform::world_associated_change(int type) {
    dirty_flag_ |= type;
    update_flag_manager_.distribute();
}

void Transform::rotate_by_quat(const QuaternionF &rotate_quat, bool relative_to_local) {
    if (relative_to_local) {
        rotation_quaternion_ = get_rotation_quaternion() * rotate_quat;
        set_rotation_quaternion(rotation_quaternion_);
    } else {
        world_rotation_quaternion_ = get_world_rotation_quaternion() * rotate_quat;
        set_world_rotation_quaternion(world_rotation_quaternion_);
    }
}

void Transform::rotate_xyz(float x, float y, float z, bool relative_to_local) {
    const auto kRotQuat = QuaternionF::makeRotationEuler(x * kDegreeToRadian, y * kDegreeToRadian, z * kDegreeToRadian);
    rotate_by_quat(kRotQuat, relative_to_local);
}

}// namespace vox
