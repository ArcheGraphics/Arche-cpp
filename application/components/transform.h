//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "math/matrix4x4.h"
#include "math/quaternion.h"
#include "math/vector3.h"
#include "ecs/component.h"
#include "base/update_flag_manager.h"

namespace vox {
/**
 * Dirty flag of transform.
 */
enum TransformFlag {
    LOCAL_EULER = 0x1,
    LOCAL_QUAT = 0x2,
    WORLD_POSITION = 0x4,
    WORLD_EULER = 0x8,
    WORLD_QUAT = 0x10,
    WORLD_SCALE = 0x20,
    LOCAL_MATRIX = 0x40,
    WORLD_MATRIX = 0x80,

    /** WorldMatrix | WorldPosition */
    WM_WP = 0x84,
    /** WorldMatrix | WorldEuler | WorldQuat */
    WM_WE_WQ = 0x98,
    /** WorldMatrix | WorldPosition | WorldEuler | WorldQuat */
    WM_WP_WE_WQ = 0x9c,
    /** WorldMatrix | WorldScale */
    WM_WS = 0xa0,
    /** WorldMatrix | WorldPosition | WorldScale */
    WM_WP_WS = 0xa4,
    /** WorldMatrix | WorldPosition | WorldEuler | WorldQuat | WorldScale */
    WM_WP_WE_WQ_WS = 0xbc
};

/**
 * Used to implement transformation related functions.
 */
class Transform : public Component {
public:
    explicit Transform(Entity *entity);

    /**
     * Local position.
     * @remarks Need to re-assign after modification to ensure that the modification takes effect.
     */
    Point3F get_position();

    void set_position(const Point3F &value);

    /**
     * World position.
     * @remarks Need to re-assign after modification to ensure that the modification takes effect.
     */
    Point3F get_world_position();

    void set_world_position(const Point3F &value);

    /**
     * Local rotation, defining the rotation value in degrees.
     * Rotations are performed around the Y axis, the X axis, and the Z axis, in that order.
     * @remarks Need to re-assign after modification to ensure that the modification takes effect.
     */
    Vector3F get_rotation();

    void set_rotation(const Vector3F &value);

    /**
     * World rotation, defining the rotation value in degrees.
     * Rotations are performed around the Y axis, the X axis, and the Z axis, in that order.
     * @remarks Need to re-assign after modification to ensure that the modification takes effect.
     */
    Vector3F get_world_rotation();

    void set_world_rotation(const Vector3F &value);

    /**
     * Local rotation, defining the rotation by using a unit quaternion.
     * @remarks Need to re-assign after modification to ensure that the modification takes effect.
     */
    QuaternionF get_rotation_quaternion();

    void set_rotation_quaternion(const QuaternionF &value);

    /**
     * World rotation, defining the rotation by using a unit quaternion.
     * @remarks Need to re-assign after modification to ensure that the modification takes effect.
     */
    QuaternionF get_world_rotation_quaternion();

    void set_world_rotation_quaternion(const QuaternionF &value);

    /**
     * Local scaling.
     * @remarks Need to re-assign after modification to ensure that the modification takes effect.
     */
    Vector3F get_scale();

    void set_scale(const Vector3F &value);

    /**
     * Local lossy scaling.
     * @remarks The value obtained may not be correct under certain conditions(for example, the parent node has scaling,
     * and the child node has a rotation), the scaling will be tilted. Vector3 cannot be used to correctly represent the
     * scaling. Must use Matrix3x3.
     */
    Vector3F get_lossy_world_scale();

    /**
     * Local matrix.
     * @remarks Need to re-assign after modification to ensure that the modification takes effect.
     */
    Matrix4x4F get_local_matrix();

    void set_local_matrix(const Matrix4x4F &value);

    /**
     * World matrix.
     * @remarks Need to re-assign after modification to ensure that the modification takes effect.
     */
    Matrix4x4F get_world_matrix();

    void set_world_matrix(const Matrix4x4F &value);

    /**
     * Set local position by X, Y, Z value.
     * @param x - X coordinate
     * @param y - Y coordinate
     * @param z - Z coordinate
     */
    void set_position(float x, float y, float z);

    /**
     * Set local rotation by the X, Y, Z components of the euler angle, unit in degrees.
     * Rotations are performed around the Y axis, the X axis, and the Z axis, in that order.
     * @param x - The angle of rotation around the X axis
     * @param y - The angle of rotation around the Y axis
     * @param z - The angle of rotation around the Z axis
     */
    void set_rotation(float x, float y, float z);

    /**
     * Set local rotation by the X, Y, Z, and W components of the quaternion.
     * @param x - X component of quaternion
     * @param y - Y component of quaternion
     * @param z - Z component of quaternion
     * @param w - W component of quaternion
     */
    void set_rotation_quaternion(float x, float y, float z, float w);

    /**
     * Set local scaling by scaling values along X, Y, Z axis.
     * @param x - Scaling along X axis
     * @param y - Scaling along Y axis
     * @param z - Scaling along Z axis
     */
    void set_scale(float x, float y, float z);

    /**
     * Set world position by X, Y, Z value.
     * @param x - X coordinate
     * @param y - Y coordinate
     * @param z - Z coordinate
     */
    void set_world_position(float x, float y, float z);

    /**
     * Set world rotation by the X, Y, Z components of the euler angle, unit in degrees, Yaw/Pitch/Roll sequence.
     * @param x - The angle of rotation around the X axis
     * @param y - The angle of rotation around the Y axis
     * @param z - The angle of rotation around the Z axis
     */
    void set_world_rotation(float x, float y, float z);

    /**
     * Set local rotation by the X, Y, Z, and W components of the quaternion.
     * @param x - X component of quaternion
     * @param y - Y component of quaternion
     * @param z - Z component of quaternion
     * @param w - W component of quaternion
     */
    void set_world_rotation_quaternion(float x, float y, float z, float w);

    /**
     * Get the forward direction in world space.
     * @returns Forward vector
     */
    Vector3F get_world_forward();

    /**
     * Get the right direction in world space.
     * @returns Right vector
     */
    Vector3F get_world_right();

    /**
     * Get the up direction in world space.
     * @returns Up vector
     */
    Vector3F get_world_up();

    /**
     * Translate along the passed Vector3.
     * @param translation - Direction and distance of translation
     * @param relative_to_local - Relative to local space
     */
    void translate(const Vector3F &translation, bool relative_to_local = true);

    /**
     * Translate along the passed X, Y, Z value.
     * @param x - Translate direction and distance along x axis
     * @param y - Translate direction and distance along y axis
     * @param z - Translate direction and distance along z axis
     * @param relative_to_local - Relative to local space
     */
    void translate(float x, float y, float z, bool relative_to_local = true);

    /**
     * Rotate around the passed Vector3.
     * @param rotation - Euler angle in degrees
     * @param relative_to_local - Relative to local space
     */
    void rotate(const Vector3F &rotation, bool relative_to_local = true);

    /**
     * Rotate around the passed Vector3.
     * @param x - Rotation along x axis, in degrees
     * @param y - Rotation along y axis, in degrees
     * @param z - Rotation along z axis, in degrees
     * @param relative_to_local - Relative to local space
     */
    void rotate(float x, float y, float z, bool relative_to_local = true);

    /**
     * Rotate around the specified axis according to the specified angle.
     * @param axis - Rotate axis
     * @param angle - Rotate angle in degrees
     * @param relative_to_local - Relative to local space
     */
    void rotate_by_axis(const Vector3F &axis, float angle, bool relative_to_local = true);

    /**
     * Rotate and ensure that the world front vector points to the target world position.
     * @param world_position - Target world position
     * @param world_up - Up direction in world space, default is Vector3(0, 1, 0)
     */
    void look_at(const Point3F &world_position, const Vector3F &world_up = Vector3F(0, 1, 0));

    /**
     * Register world transform change flag.
     * @returns Change flag
     */
    std::unique_ptr<UpdateFlag> register_world_change_flag();

private:
    friend class Entity;

    void parent_change();

    /**
     * Get worldMatrix: Will trigger the WorldMatrix update of itself and all parent entities.
     * Get WorldPosition: Will trigger the worldMatrix, local position update of itself and the WorldMatrix update of
     * all parent entities. In summary, any update of related variables will cause the dirty mark of one of the full
     * process (WorldMatrix or WorldRotationQuaternion) to be false.
     */
    void update_world_position_flag();

    /**
     * Get worldMatrix: Will trigger the WorldMatrix update of itself and all parent entities.
     * Get WorldPosition: Will trigger the worldMatrix, local position update of itself and the WorldMatrix update of
     * all parent entities. Get WorldRotationQuaternion: Will trigger the world rotation (in quaternion) update of
     * itself and all parent entities. Get WorldRotation: Will trigger the world rotation(in euler and quaternion)
     * update of itself and world rotation(in quaternion) update of all parent entities. In summary, any update of
     * related variables will cause the dirty mark of one of the full process (WorldMatrix or WorldRotationQuaternion)
     * to be false.
     */
    void update_world_rotation_flag();

    /**
     * Get worldMatrix: Will trigger the WorldMatrix update of itself and all parent entities.
     * Get WorldPosition: Will trigger the WorldMatrix, local position update of itself and the worldMatrix update of
     * all parent entities. Get WorldRotationQuaternion: Will trigger the world rotation (in quaternion) update of
     * itself and all parent entities. Get WorldRotation: Will trigger the world rotation(in euler and quaternion)
     * update of itself and world rotation(in quaternion) update of all parent entities. In summary, any update of
     * related variables will cause the dirty mark of one of the full process (WorldMatrix or WorldRotationQuaternion)
     * to be false.
     */
    void update_world_position_and_rotation_flag();

    /**
     * Get WorldMatrix: Will trigger the worldMatrix update of itself and all parent entities.
     * Get WorldPosition: Will trigger the worldMatrix, local position update of itself and the WorldMatrix update of
     * all parent entities. Get worldScale: Will trigger the scaling update of itself and all parent entities. In
     * summary, any update of related variables will cause the dirty mark of one of the full process (WorldMatrix) to be
     * false.
     */
    void update_world_scale_flag();

    /**
     * Get worldMatrix: Will trigger the WorldMatrix update of itself and all parent entities.
     * Get WorldPosition: Will trigger the WorldMatrix, local position update of itself and the worldMatrix update of
     * all parent entities. Get worldScale: Will trigger the scaling update of itself and all parent entities. In
     * summary, any update of related variables will cause the dirty mark of one of the full process (WorldMatrix) to be
     * false.
     */
    void update_world_position_and_scale_flag();

    /**
     * Update all world transform property dirty flag, the principle is the same as above.
     */
    void update_all_world_flag();

    Transform *get_parent_transform();

    Matrix3x3F get_scale_matrix();

    [[nodiscard]] bool is_contain_dirty_flags(int target_dirty_flags) const;

    [[nodiscard]] bool is_contain_dirty_flag(int type) const;

    void set_dirty_flag_true(int type);

    void set_dirty_flag_false(int type);

    void world_associated_change(int type);

    void rotate_by_quat(const QuaternionF &rotate_quat, bool relative_to_local);

    void rotate_xyz(float x, float y, float z, bool relative_to_local = true);

    Point3F position_ = Point3F();
    Vector3F rotation_ = Vector3F();
    QuaternionF rotation_quaternion_;
    Vector3F scale_ = Vector3F(1, 1, 1);
    Point3F world_position_ = Point3F();
    Vector3F world_rotation_ = Vector3F();
    QuaternionF world_rotation_quaternion_;
    Vector3F lossy_world_scale_ = Vector3F(1, 1, 1);
    Matrix4x4F local_matrix_;
    Matrix4x4F world_matrix_;
    UpdateFlagManager update_flag_manager_;
    bool is_parent_dirty_ = true;
    Transform *parent_transform_cache_ = nullptr;
    int dirty_flag_ = TransformFlag::WM_WP_WE_WQ_WS;
};

}// namespace vox
