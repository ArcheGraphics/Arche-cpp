//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "component.h"
#include "update_flag_manager.h"
#include "vector3.h"
#include "matrix4x4.h"
#include "quaternion.h"

namespace vox {
/**
 * Dirty flag of transform.
 */
enum TransformFlag {
    LocalEuler = 0x1,
    LocalQuat = 0x2,
    WorldPosition = 0x4,
    WorldEuler = 0x8,
    WorldQuat = 0x10,
    WorldScale = 0x20,
    LocalMatrix = 0x40,
    WorldMatrix = 0x80,

    /** WorldMatrix | WorldPosition */
    WmWp = 0x84,
    /** WorldMatrix | WorldEuler | WorldQuat */
    WmWeWq = 0x98,
    /** WorldMatrix | WorldPosition | WorldEuler | WorldQuat */
    WmWpWeWq = 0x9c,
    /** WorldMatrix | WorldScale */
    WmWs = 0xa0,
    /** WorldMatrix | WorldPosition | WorldScale */
    WmWpWs = 0xa4,
    /** WorldMatrix | WorldPosition | WorldEuler | WorldQuat | WorldScale */
    WmWpWeWqWs = 0xbc
};

/**
 * Used to implement transformation related functions.
 */
class Transform : public Component {
public:
    Transform(Entity *entity);

    /**
     * Local position.
     * @remarks Need to re-assign after modification to ensure that the modification takes effect.
     */
    Point3F position();

    void setPosition(const Point3F &value);

    /**
     * World position.
     * @remarks Need to re-assign after modification to ensure that the modification takes effect.
     */
    Point3F worldPosition();

    void setWorldPosition(const Point3F &value);

    /**
     * Local rotation, defining the rotation value in degrees.
     * Rotations are performed around the Y axis, the X axis, and the Z axis, in that order.
     * @remarks Need to re-assign after modification to ensure that the modification takes effect.
     */
    Vector3F rotation();

    void setRotation(const Vector3F &value);

    /**
     * World rotation, defining the rotation value in degrees.
     * Rotations are performed around the Y axis, the X axis, and the Z axis, in that order.
     * @remarks Need to re-assign after modification to ensure that the modification takes effect.
     */
    Vector3F worldRotation();

    void setWorldRotation(const Vector3F &value);

    /**
     * Local rotation, defining the rotation by using a unit quaternion.
     * @remarks Need to re-assign after modification to ensure that the modification takes effect.
     */
    QuaternionF rotationQuaternion();

    void setRotationQuaternion(const QuaternionF &value);

    /**
     * World rotation, defining the rotation by using a unit quaternion.
     * @remarks Need to re-assign after modification to ensure that the modification takes effect.
     */
    QuaternionF worldRotationQuaternion();

    void setWorldRotationQuaternion(const QuaternionF &value);

    /**
     * Local scaling.
     * @remarks Need to re-assign after modification to ensure that the modification takes effect.
     */
    Vector3F scale();

    void setScale(const Vector3F &value);

    /**
     * Local lossy scaling.
     * @remarks The value obtained may not be correct under certain conditions(for example, the parent node has scaling,
     * and the child node has a rotation), the scaling will be tilted. Vector3 cannot be used to correctly represent the scaling. Must use Matrix3x3.
     */
    Vector3F lossyWorldScale();

    /**
     * Local matrix.
     * @remarks Need to re-assign after modification to ensure that the modification takes effect.
     */
    Matrix4x4F localMatrix();

    void setLocalMatrix(const Matrix4x4F &value);

    /**
     * World matrix.
     * @remarks Need to re-assign after modification to ensure that the modification takes effect.
     */
    Matrix4x4F worldMatrix();

    void setWorldMatrix(const Matrix4x4F &value);

    /**
     * Set local position by X, Y, Z value.
     * @param x - X coordinate
     * @param y - Y coordinate
     * @param z - Z coordinate
     */
    void setPosition(float x, float y, float z);

    /**
     * Set local rotation by the X, Y, Z components of the euler angle, unit in degrees.
     * Rotations are performed around the Y axis, the X axis, and the Z axis, in that order.
     * @param x - The angle of rotation around the X axis
     * @param y - The angle of rotation around the Y axis
     * @param z - The angle of rotation around the Z axis
     */
    void setRotation(float x, float y, float z);

    /**
     * Set local rotation by the X, Y, Z, and W components of the quaternion.
     * @param x - X component of quaternion
     * @param y - Y component of quaternion
     * @param z - Z component of quaternion
     * @param w - W component of quaternion
     */
    void setRotationQuaternion(float x, float y, float z, float w);

    /**
     * Set local scaling by scaling values along X, Y, Z axis.
     * @param x - Scaling along X axis
     * @param y - Scaling along Y axis
     * @param z - Scaling along Z axis
     */
    void setScale(float x, float y, float z);

    /**
     * Set world position by X, Y, Z value.
     * @param x - X coordinate
     * @param y - Y coordinate
     * @param z - Z coordinate
     */
    void setWorldPosition(float x, float y, float z);

    /**
     * Set world rotation by the X, Y, Z components of the euler angle, unit in degrees, Yaw/Pitch/Roll sequence.
     * @param x - The angle of rotation around the X axis
     * @param y - The angle of rotation around the Y axis
     * @param z - The angle of rotation around the Z axis
     */
    void setWorldRotation(float x, float y, float z);

    /**
     * Set local rotation by the X, Y, Z, and W components of the quaternion.
     * @param x - X component of quaternion
     * @param y - Y component of quaternion
     * @param z - Z component of quaternion
     * @param w - W component of quaternion
     */
    void setWorldRotationQuaternion(float x, float y, float z, float w);

    /**
     * Get the forward direction in world space.
     * @returns Forward vector
     */
    Vector3F worldForward();

    /**
     * Get the right direction in world space.
     * @returns Right vector
     */
    Vector3F worldRight();

    /**
     * Get the up direction in world space.
     * @returns Up vector
     */
    Vector3F worldUp();

    /**
     * Translate along the passed Vector3.
     * @param translation - Direction and distance of translation
     * @param relativeToLocal - Relative to local space
     */
    void translate(const Vector3F &translation, bool relativeToLocal = true);

    /**
     * Translate along the passed X, Y, Z value.
     * @param x - Translate direction and distance along x axis
     * @param y - Translate direction and distance along y axis
     * @param z - Translate direction and distance along z axis
     * @param relativeToLocal - Relative to local space
     */
    void translate(float x, float y, float z, bool relativeToLocal = true);

    /**
     * Rotate around the passed Vector3.
     * @param rotation - Euler angle in degrees
     * @param relativeToLocal - Relative to local space
     */
    void rotate(const Vector3F &rotation, bool relativeToLocal = true);

    /**
     * Rotate around the passed Vector3.
     * @param x - Rotation along x axis, in degrees
     * @param y - Rotation along y axis, in degrees
     * @param z - Rotation along z axis, in degrees
     * @param relativeToLocal - Relative to local space
     */
    void rotate(float x, float y, float z, bool relativeToLocal = true);

    /**
     * Rotate around the specified axis according to the specified angle.
     * @param axis - Rotate axis
     * @param angle - Rotate angle in degrees
     * @param relativeToLocal - Relative to local space
     */
    void rotateByAxis(const Vector3F &axis, float angle, bool relativeToLocal = true);

    /**
     * Rotate and ensure that the world front vector points to the target world position.
     * @param worldPosition - Target world position
     * @param worldUp - Up direction in world space, default is Vector3(0, 1, 0)
     */
    void lookAt(const Point3F &worldPosition, const Vector3F &worldUp = Vector3F(0, 1, 0));

    /**
     * Register world transform change flag.
     * @returns Change flag
     */
    std::unique_ptr<UpdateFlag> registerWorldChangeFlag();


private:
    friend class Entity;

    void _parentChange();

    /**
     * Get worldMatrix: Will trigger the worldMatrix update of itself and all parent entities.
     * Get worldPosition: Will trigger the worldMatrix, local position update of itself and the worldMatrix update of all parent entities.
     * In summary, any update of related variables will cause the dirty mark of one of the full process (worldMatrix or worldRotationQuaternion) to be false.
     */
    void _updateWorldPositionFlag();

    /**
     * Get worldMatrix: Will trigger the worldMatrix update of itself and all parent entities.
     * Get worldPosition: Will trigger the worldMatrix, local position update of itself and the worldMatrix update of all parent entities.
     * Get worldRotationQuaternion: Will trigger the world rotation (in quaternion) update of itself and all parent entities.
     * Get worldRotation: Will trigger the world rotation(in euler and quaternion) update of itself and world rotation(in quaternion) update of all parent entities.
     * In summary, any update of related variables will cause the dirty mark of one of the full process (worldMatrix or worldRotationQuaternion) to be false.
     */
    void _updateWorldRotationFlag();

    /**
     * Get worldMatrix: Will trigger the worldMatrix update of itself and all parent entities.
     * Get worldPosition: Will trigger the worldMatrix, local position update of itself and the worldMatrix update of all parent entities.
     * Get worldRotationQuaternion: Will trigger the world rotation (in quaternion) update of itself and all parent entities.
     * Get worldRotation: Will trigger the world rotation(in euler and quaternion) update of itself and world rotation(in quaternion) update of all parent entities.
     * In summary, any update of related variables will cause the dirty mark of one of the full process (worldMatrix or worldRotationQuaternion) to be false.
     */
    void _updateWorldPositionAndRotationFlag();

    /**
     * Get worldMatrix: Will trigger the worldMatrix update of itself and all parent entities.
     * Get worldPosition: Will trigger the worldMatrix, local position update of itself and the worldMatrix update of all parent entities.
     * Get worldScale: Will trigger the scaling update of itself and all parent entities.
     * In summary, any update of related variables will cause the dirty mark of one of the full process (worldMatrix) to be false.
     */
    void _updateWorldScaleFlag();

    /**
     * Get worldMatrix: Will trigger the worldMatrix update of itself and all parent entities.
     * Get worldPosition: Will trigger the worldMatrix, local position update of itself and the worldMatrix update of all parent entities.
     * Get worldScale: Will trigger the scaling update of itself and all parent entities.
     * In summary, any update of related variables will cause the dirty mark of one of the full process (worldMatrix) to be false.
     */
    void _updateWorldPositionAndScaleFlag();

    /**
     * Update all world transform property dirty flag, the principle is the same as above.
     */
    void _updateAllWorldFlag();

    Transform *_getParentTransform();

    Matrix3x3F _getScaleMatrix();

    bool _isContainDirtyFlags(int targetDirtyFlags);

    bool _isContainDirtyFlag(int type);

    void _setDirtyFlagTrue(int type);

    void _setDirtyFlagFalse(int type);

    void _worldAssociatedChange(int type);

    void _rotateByQuat(const QuaternionF &rotateQuat, bool relativeToLocal);

    void _translate(const Vector3F &translation, bool relativeToLocal = true);

    void _rotateXYZ(float x, float y, float z, bool relativeToLocal = true);

    Point3F _position = Point3F();
    Vector3F _rotation = Vector3F();
    QuaternionF _rotationQuaternion;
    Vector3F _scale = Vector3F(1, 1, 1);
    Point3F _worldPosition = Point3F();
    Vector3F _worldRotation = Vector3F();
    QuaternionF _worldRotationQuaternion;
    Vector3F _lossyWorldScale = Vector3F(1, 1, 1);
    Matrix4x4F _localMatrix;
    Matrix4x4F _worldMatrix;
    UpdateFlagManager _updateFlagManager;
    bool _isParentDirty = true;
    Transform *_parentTransformCache = nullptr;
    int _dirtyFlag = TransformFlag::WmWpWeWqWs;
};

}// namespace vox