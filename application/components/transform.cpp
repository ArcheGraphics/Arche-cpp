//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "transform.h"
#include "math_utils.h"
#include "matrix_utils.h"
#include "entity.h"

namespace vox {
Transform::Transform(Entity *entity) : Component(entity) {
}

Point3F Transform::position() {
    return _position;
}

void Transform::setPosition(const Point3F &value) {
    _position = value;
    _setDirtyFlagTrue(TransformFlag::LocalMatrix);
    _updateWorldPositionFlag();
}

Point3F Transform::worldPosition() {
    if (_isContainDirtyFlag(TransformFlag::WorldPosition)) {
        if (_getParentTransform()) {
            _worldPosition = getTranslation(worldMatrix());
        } else {
            _worldPosition = _position;
        }
        _setDirtyFlagFalse(TransformFlag::WorldPosition);
    }
    return _worldPosition;
}

void Transform::setWorldPosition(const Point3F &value) {
    _worldPosition = value;
    const auto parent = _getParentTransform();
    if (parent) {
        _position = parent->worldMatrix().inverse() * value;
    } else {
        _position = value;
    }
    setPosition(_position);
    _setDirtyFlagFalse(TransformFlag::WorldPosition);
}

Vector3F Transform::rotation() {
    if (_isContainDirtyFlag(TransformFlag::LocalEuler)) {
        _rotation = _rotationQuaternion.toEuler();
        _rotation *= kRadianToDegree;// radians to degrees

        _setDirtyFlagFalse(TransformFlag::LocalEuler);
    }
    return _rotation;
}

void Transform::setRotation(const Vector3F &value) {
    _rotation = value;
    _setDirtyFlagTrue(TransformFlag::LocalMatrix | TransformFlag::LocalQuat);
    _setDirtyFlagFalse(TransformFlag::LocalEuler);
    _updateWorldRotationFlag();
}

Vector3F Transform::worldRotation() {
    if (_isContainDirtyFlag(TransformFlag::WorldEuler)) {
        _worldRotation = worldRotationQuaternion().toEuler();
        _worldRotation *= kRadianToDegree;// Radian to angle
        _setDirtyFlagFalse(TransformFlag::WorldEuler);
    }
    return _worldRotation;
}

void Transform::setWorldRotation(const Vector3F &value) {
    _worldRotation = value;
    _worldRotationQuaternion = QuaternionF::makeRotationEuler(degreesToRadians(value.x),
                                                              degreesToRadians(value.y),
                                                              degreesToRadians(value.z));
    setWorldRotationQuaternion(_worldRotationQuaternion);
    _setDirtyFlagFalse(TransformFlag::WorldEuler);
}

QuaternionF Transform::rotationQuaternion() {
    if (_isContainDirtyFlag(TransformFlag::LocalQuat)) {
        _rotationQuaternion = QuaternionF::makeRotationEuler(degreesToRadians(_rotation.x),
                                                             degreesToRadians(_rotation.y),
                                                             degreesToRadians(_rotation.z));
        _setDirtyFlagFalse(TransformFlag::LocalQuat);
    }
    return _rotationQuaternion;
}

void Transform::setRotationQuaternion(const QuaternionF &value) {
    _rotationQuaternion = value;
    _setDirtyFlagTrue(TransformFlag::LocalMatrix | TransformFlag::LocalEuler);
    _setDirtyFlagFalse(TransformFlag::LocalQuat);
    _updateWorldRotationFlag();
}

QuaternionF Transform::worldRotationQuaternion() {
    if (_isContainDirtyFlag(TransformFlag::WorldQuat)) {
        const auto parent = _getParentTransform();
        if (parent) {
            _worldRotationQuaternion = parent->worldRotationQuaternion() * rotationQuaternion();
        } else {
            _worldRotationQuaternion = rotationQuaternion();
        }
        _setDirtyFlagFalse(TransformFlag::WorldQuat);
    }
    return _worldRotationQuaternion;
}

void Transform::setWorldRotationQuaternion(const QuaternionF &value) {
    _worldRotationQuaternion = value;
    const auto parent = _getParentTransform();
    if (parent) {
        auto _tempQuat0 = parent->worldRotationQuaternion().inverse();
        _rotationQuaternion = value * _tempQuat0;
    } else {
        _rotationQuaternion = value;
    }
    setRotationQuaternion(_rotationQuaternion);
    _setDirtyFlagFalse(TransformFlag::WorldQuat);
}

Vector3F Transform::scale() {
    return _scale;
}

void Transform::setScale(const Vector3F &value) {
    _scale = value;
    _setDirtyFlagTrue(TransformFlag::LocalMatrix);
    _updateWorldScaleFlag();
}

Vector3F Transform::lossyWorldScale() {
    if (_isContainDirtyFlag(TransformFlag::WorldScale)) {
        if (_getParentTransform()) {
            const auto scaleMat = _getScaleMatrix();
            _lossyWorldScale = Vector3F(scaleMat[0], scaleMat[4], scaleMat[8]);
        } else {
            _lossyWorldScale = _scale;
        }
        _setDirtyFlagFalse(TransformFlag::WorldScale);
    }
    return _lossyWorldScale;
}

Matrix4x4F Transform::localMatrix() {
    if (_isContainDirtyFlag(TransformFlag::LocalMatrix)) {
        _localMatrix = makeAffineMatrix(_scale, rotationQuaternion(), _position);
        _setDirtyFlagFalse(TransformFlag::LocalMatrix);
    }
    return _localMatrix;
}

void Transform::setLocalMatrix(const Matrix4x4F &value) {
    _localMatrix = value;
    decompose(_localMatrix, _position, _rotationQuaternion, _scale);
    _setDirtyFlagTrue(TransformFlag::LocalEuler);
    _setDirtyFlagFalse(TransformFlag::LocalMatrix);
    _updateAllWorldFlag();
}

Matrix4x4F Transform::worldMatrix() {
    if (_isContainDirtyFlag(TransformFlag::WorldMatrix)) {
        const auto parent = _getParentTransform();
        if (parent) {
            _worldMatrix = parent->worldMatrix() * localMatrix();
        } else {
            _worldMatrix = localMatrix();
        }
        _setDirtyFlagFalse(TransformFlag::WorldMatrix);
    }
    return _worldMatrix;
}

void Transform::setWorldMatrix(const Matrix4x4F &value) {
    _worldMatrix = value;
    const auto parent = _getParentTransform();
    if (parent) {
        auto _tempMat42 = parent->worldMatrix().inverse();
        _localMatrix = value * _tempMat42;
    } else {
        _localMatrix = value;
    }
    setLocalMatrix(_localMatrix);
    _setDirtyFlagFalse(TransformFlag::WorldMatrix);
}

void Transform::setPosition(float x, float y, float z) {
    _position = Point3F(x, y, z);
    setPosition(_position);
}

void Transform::setRotation(float x, float y, float z) {
    _rotation = Vector3F(x, y, z);
    setRotation(_rotation);
}

void Transform::setRotationQuaternion(float x, float y, float z, float w) {
    _rotationQuaternion = QuaternionF(x, y, z, w);
    setRotationQuaternion(_rotationQuaternion);
}

void Transform::setScale(float x, float y, float z) {
    _scale = Vector3F(x, y, z);
    setScale(_scale);
}

void Transform::setWorldPosition(float x, float y, float z) {
    _worldPosition = Point3F(x, y, z);
    setWorldPosition(_worldPosition);
}

void Transform::setWorldRotation(float x, float y, float z) {
    _worldRotation = Vector3F(x, y, z);
    setWorldRotation(_worldRotation);
}

void Transform::setWorldRotationQuaternion(float x, float y, float z, float w) {
    _worldRotationQuaternion = QuaternionF(x, y, z, w);
    setWorldRotationQuaternion(_worldRotationQuaternion);
}

Vector3F Transform::worldForward() {
    const auto &e = worldMatrix();
    auto forward = Vector3F(-e[8], -e[9], -e[10]);
    return forward.normalized();
}

Vector3F Transform::worldRight() {
    const auto &e = worldMatrix();
    auto right = Vector3F(e[0], e[1], e[2]);
    return right.normalized();
}

Vector3F Transform::worldUp() {
    const auto &e = worldMatrix();
    auto up = Vector3F(e[4], e[5], e[6]);
    return up.normalized();
}

void Transform::translate(const Vector3F &translation, bool relativeToLocal) {
    _translate(translation, relativeToLocal);
}

void Transform::translate(float x, float y, float z, bool relativeToLocal) {
    Vector3F translate = Vector3F(x, y, z);
    _translate(translate, relativeToLocal);
}

void Transform::rotate(const Vector3F &rotation, bool relativeToLocal) {
    _rotateXYZ(rotation.x, rotation.y, rotation.z, relativeToLocal);
}

void Transform::rotate(float x, float y, float z, bool relativeToLocal) {
    _rotateXYZ(x, y, z, relativeToLocal);
}

void Transform::rotateByAxis(const Vector3F &axis, float angle, bool relativeToLocal) {
    const auto rad = angle * kDegreeToRadian;
    QuaternionF _tempQuat0(axis, rad);
    _rotateByQuat(_tempQuat0, relativeToLocal);
}

void Transform::lookAt(const Point3F &worldPosition, const Vector3F &worldUp) {
    const auto position = this->worldPosition();
    if (std::abs(position.x - worldPosition.x) < std::numeric_limits<float>::epsilon() &&
        std::abs(position.y - worldPosition.y) < std::numeric_limits<float>::epsilon() &&
        std::abs(position.z - worldPosition.z) < std::numeric_limits<float>::epsilon()) {
        return;
    }
    Matrix4x4F rotMat = makeLookAtMatrix(position, worldPosition, worldUp);
    auto worldRotationQuaternion = getRotation(rotMat);
    worldRotationQuaternion = worldRotationQuaternion.inverse();
    setWorldRotationQuaternion(worldRotationQuaternion);
}

std::unique_ptr<UpdateFlag> Transform::registerWorldChangeFlag() {
    return _updateFlagManager.registration();
}

void Transform::_parentChange() {
    _isParentDirty = true;
    _updateAllWorldFlag();
}

void Transform::_updateWorldPositionFlag() {
    if (!_isContainDirtyFlags(TransformFlag::WmWp)) {
        _worldAssociatedChange(TransformFlag::WmWp);
        const auto &nodeChildren = _entity->_children;
        for (size_t i = 0, n = nodeChildren.size(); i < n; i++) {
            nodeChildren[i]->transform->_updateWorldPositionFlag();
        }
    }
}

void Transform::_updateWorldRotationFlag() {
    if (!_isContainDirtyFlags(TransformFlag::WmWeWq)) {
        _worldAssociatedChange(TransformFlag::WmWeWq);
        const auto &nodeChildren = _entity->_children;
        for (size_t i = 0, n = nodeChildren.size(); i < n; i++) {
            // Rotation update of parent entity will trigger world position and rotation update of all child entity.
            nodeChildren[i]->transform->_updateWorldPositionAndRotationFlag();
        }
    }
}

void Transform::_updateWorldPositionAndRotationFlag() {
    if (!_isContainDirtyFlags(TransformFlag::WmWpWeWq)) {
        _worldAssociatedChange(TransformFlag::WmWpWeWq);
        const auto &nodeChildren = _entity->_children;
        for (size_t i = 0, n = nodeChildren.size(); i < n; i++) {
            nodeChildren[i]->transform->_updateWorldPositionAndRotationFlag();
        }
    }
}

void Transform::_updateWorldScaleFlag() {
    if (!_isContainDirtyFlags(TransformFlag::WmWs)) {
        _worldAssociatedChange(TransformFlag::WmWs);
        const auto &nodeChildren = _entity->_children;
        for (size_t i = 0, n = nodeChildren.size(); i < n; i++) {
            nodeChildren[i]->transform->_updateWorldPositionAndScaleFlag();
        }
    }
}

void Transform::_updateWorldPositionAndScaleFlag() {
    if (!_isContainDirtyFlags(TransformFlag::WmWpWs)) {
        _worldAssociatedChange(TransformFlag::WmWpWs);
        const auto &nodeChildren = _entity->_children;
        for (size_t i = 0, n = nodeChildren.size(); i < n; i++) {
            nodeChildren[i]->transform->_updateWorldPositionAndScaleFlag();
        }
    }
}

void Transform::_updateAllWorldFlag() {
    if (!_isContainDirtyFlags(TransformFlag::WmWpWeWqWs)) {
        _worldAssociatedChange(TransformFlag::WmWpWeWqWs);
        const auto &nodeChildren = _entity->_children;
        for (size_t i = 0, n = nodeChildren.size(); i < n; i++) {
            nodeChildren[i]->transform->_updateAllWorldFlag();
        }
    }
}

Transform *Transform::_getParentTransform() {
    if (!_isParentDirty) {
        return _parentTransformCache;
    }
    Transform *parentCache = nullptr;
    auto parent = _entity->parent();
    while (parent) {
        const auto &transform = parent->transform;
        if (transform) {
            parentCache = transform;
            break;
        } else {
            parent = parent->parent();
        }
    }
    _parentTransformCache = parentCache;
    _isParentDirty = false;
    return parentCache;
}

Matrix3x3F Transform::_getScaleMatrix() {
    Matrix3x3F worldRotScaMat = worldMatrix().matrix3();
    Quaternion invRotation = worldRotationQuaternion().inverse();
    Matrix3x3F invRotationMat = invRotation.matrix3();
    return invRotationMat * worldRotScaMat;
}

bool Transform::_isContainDirtyFlags(int targetDirtyFlags) {
    return (_dirtyFlag & targetDirtyFlags) == targetDirtyFlags;
}

bool Transform::_isContainDirtyFlag(int type) {
    return (_dirtyFlag & type) != 0;
}

void Transform::_setDirtyFlagTrue(int type) {
    _dirtyFlag |= type;
}

void Transform::_setDirtyFlagFalse(int type) {
    _dirtyFlag &= ~type;
}

void Transform::_worldAssociatedChange(int type) {
    _dirtyFlag |= type;
    _updateFlagManager.distribute();
}

void Transform::_rotateByQuat(const QuaternionF &rotateQuat, bool relativeToLocal) {
    if (relativeToLocal) {
        _rotationQuaternion = rotationQuaternion() * rotateQuat;
        setRotationQuaternion(_rotationQuaternion);
    } else {
        _worldRotationQuaternion = worldRotationQuaternion() * rotateQuat;
        setWorldRotationQuaternion(_worldRotationQuaternion);
    }
}

void Transform::_translate(const Vector3F &translation, bool relativeToLocal) {
    if (relativeToLocal) {
        _position = _position + translation;
        setPosition(_position);
    } else {
        _worldPosition = _worldPosition + translation;
        setWorldPosition(_worldPosition);
    }
}

void Transform::_rotateXYZ(float x, float y, float z, bool relativeToLocal) {
    const auto rotQuat = QuaternionF::makeRotationEuler(x * kDegreeToRadian, y * kDegreeToRadian, z * kDegreeToRadian);
    _rotateByQuat(rotQuat, relativeToLocal);
}

}// namespace vox
