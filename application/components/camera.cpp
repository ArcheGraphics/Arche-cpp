//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "camera.h"
#include "ecs/entity.h"
#include "ecs/scene.h"
#include "math/matrix_utils.h"
#include "shader/shader.h"

namespace vox {
Camera::Camera(Entity *entity) : Component(entity),
                                 _viewMatrixProperty(Shader::createProperty("u_viewMat", ShaderDataGroup::Camera)),
                                 _projectionMatrixProperty(Shader::createProperty("u_projMat", ShaderDataGroup::Camera)),
                                 _vpMatrixProperty(Shader::createProperty("u_VPMat", ShaderDataGroup::Camera)),
                                 _inverseViewMatrixProperty(Shader::createProperty("u_viewInvMat", ShaderDataGroup::Camera)),
                                 _inverseProjectionMatrixProperty(Shader::createProperty("u_projInvMat", ShaderDataGroup::Camera)),
                                 _cameraPositionProperty(Shader::createProperty("u_cameraPos", ShaderDataGroup::Camera)) {
    auto transform = entity->transform;
    _transform = transform;
    _isViewMatrixDirty = transform->registerWorldChangeFlag();
    _isInvViewProjDirty = transform->registerWorldChangeFlag();
    _frustumViewChangeFlag = transform->registerWorldChangeFlag();
}

float Camera::nearClipPlane() const {
    return _nearClipPlane;
}

void Camera::setNearClipPlane(float value) {
    _nearClipPlane = value;
    _projMatChange();
}

float Camera::farClipPlane() const {
    return _farClipPlane;
}

void Camera::setFarClipPlane(float value) {
    _farClipPlane = value;
    _projMatChange();
}

float Camera::fieldOfView() const {
    return _fieldOfView;
}

void Camera::setFieldOfView(float value) {
    _fieldOfView = value;
    _projMatChange();
}

float Camera::aspectRatio() const {
    if (_customAspectRatio == std::nullopt) {
        return (_width * _viewport.z) / (_height * _viewport.w);
    } else {
        return _customAspectRatio.value();
    }
}

void Camera::setAspectRatio(float value) {
    _customAspectRatio = value;
    _projMatChange();
}

Vector4F Camera::viewport() const {
    return _viewport;
}

void Camera::setViewport(const Vector4F &value) {
    _viewport = value;
    _projMatChange();
}

bool Camera::isOrthographic() const {
    return _isOrthographic;
}

void Camera::setIsOrthographic(bool value) {
    _isOrthographic = value;
    _projMatChange();
}

float Camera::orthographicSize() const {
    return _orthographicSize;
}

void Camera::setOrthographicSize(float value) {
    _orthographicSize = value;
    _projMatChange();
}

Matrix4x4F Camera::viewMatrix() {
    // Remove scale
    if (_isViewMatrixDirty->flag) {
        _isViewMatrixDirty->flag = false;
        _viewMatrix = _transform->worldMatrix().inverse();
    }
    return _viewMatrix;
}

void Camera::setProjectionMatrix(const Matrix4x4F &value) {
    _projectionMatrix = value;
    _isProjMatSetting = true;
    _projMatChange();
}

Matrix4x4F Camera::projectionMatrix() {
    if ((!_isProjectionDirty || _isProjMatSetting) &&
        _lastAspectSize.x == _width &&
        _lastAspectSize.y == _height) {
        return _projectionMatrix;
    }
    _isProjectionDirty = false;
    _lastAspectSize.x = _width;
    _lastAspectSize.y = _height;
    if (!_isOrthographic) {
        _projectionMatrix = makepPerspective(degreesToRadians(_fieldOfView),
                                             aspectRatio(),
                                             _nearClipPlane,
                                             _farClipPlane);
    } else {
        const auto width = _orthographicSize * aspectRatio();
        const auto height = _orthographicSize;
        _projectionMatrix = makeOrtho(-width, width, -height, height, _nearClipPlane, _farClipPlane);
    }
    return _projectionMatrix;
}

bool Camera::enableHDR() {
    return false;
}

void Camera::setEnableHDR(bool value) {
    assert(false && "not implementation");
}

void Camera::resetProjectionMatrix() {
    _isProjMatSetting = false;
    _projMatChange();
}

void Camera::resetAspectRatio() {
    _customAspectRatio = std::nullopt;
    _projMatChange();
}

Vector4F Camera::worldToViewportPoint(const Point3F &point) {
    auto tempMat4 = projectionMatrix() * viewMatrix();
    auto tempVec4 = Vector4F(point.x, point.y, point.z, 1.0);
    tempVec4 = tempMat4 * tempVec4;

    const auto w = tempVec4.w;
    const auto nx = tempVec4.x / w;
    const auto ny = tempVec4.y / w;
    const auto nz = tempVec4.z / w;

    // Transform of coordinate axis.
    return Vector4F((nx + 1.0) * 0.5, (1.0 - ny) * 0.5, nz, w);
}

Point3F Camera::viewportToWorldPoint(const Vector3F &point) {
    return _innerViewportToWorldPoint(point, invViewProjMat());
}

Ray3F Camera::viewportPointToRay(const Vector2F &point) {
    Ray3F out;
    // Use the intersection of the near clipping plane as the origin point.
    Vector3F clipPoint = Vector3F(point.x, point.y, 0);
    out.origin = viewportToWorldPoint(clipPoint);
    // Use the intersection of the far clipping plane as the origin point.
    clipPoint.z = 1.0;
    Point3F farPoint = _innerViewportToWorldPoint(clipPoint, _invViewProjMat);
    out.direction = farPoint - out.origin;
    out.direction = out.direction.normalized();

    return out;
}

Vector2F Camera::screenToViewportPoint(const Vector2F &point) {
    const Vector4F viewport = this->viewport();
    return Vector2F((point.x / _width - viewport.x) / viewport.z,
                    (point.y / _height - viewport.y) / viewport.w);
}

Vector3F Camera::screenToViewportPoint(const Vector3F &point) {
    const Vector4F viewport = this->viewport();
    return Vector3F((point.x / _width - viewport.x) / viewport.z,
                    (point.y / _height - viewport.y) / viewport.w, 0);
}

Vector2F Camera::viewportToScreenPoint(const Vector2F &point) {
    const Vector4F viewport = this->viewport();
    return Vector2F((viewport.x + point.x * viewport.z) * _width,
                    (viewport.y + point.y * viewport.w) * _height);
}

Vector3F Camera::viewportToScreenPoint(const Vector3F &point) {
    const Vector4F viewport = this->viewport();
    return Vector3F((viewport.x + point.x * viewport.z) * _width,
                    (viewport.y + point.y * viewport.w) * _height, 0);
}

Vector4F Camera::viewportToScreenPoint(const Vector4F &point) {
    const Vector4F viewport = this->viewport();
    return Vector4F((viewport.x + point.x * viewport.z) * _width,
                    (viewport.y + point.y * viewport.w) * _height, 0, 0);
}

Vector4F Camera::worldToScreenPoint(const Point3F &point) {
    auto out = worldToViewportPoint(point);
    return viewportToScreenPoint(out);
}

Point3F Camera::screenToWorldPoint(const Vector3F &point) {
    auto out = screenToViewportPoint(point);
    return viewportToWorldPoint(out);
}

Ray3F Camera::screenPointToRay(const Vector2F &point) {
    Vector2F viewportPoint = screenToViewportPoint(point);
    return viewportPointToRay(viewportPoint);
}

void Camera::_onActive() {
    entity()->scene()->attachRenderCamera(this);
}

void Camera::_onInActive() {
    entity()->scene()->detachRenderCamera(this);
}

void Camera::_onDestroy() {
    _isInvViewProjDirty->destroy();
    _isViewMatrixDirty->destroy();
}

void Camera::_projMatChange() {
    _isFrustumProjectDirty = true;
    _isProjectionDirty = true;
    _isInvProjMatDirty = true;
    _isInvViewProjDirty->flag = true;
}

Point3F Camera::_innerViewportToWorldPoint(const Vector3F &point, const Matrix4x4F &invViewProjMat) {
    // Depth is a normalized value, 0 is nearPlane, 1 is farClipPlane.
    const auto depth = point.z * 2 - 1;
    // Transform to clipping space matrix
    Point3F clipPoint = Point3F(point.x * 2 - 1, 1 - point.y * 2, depth);
    clipPoint = invViewProjMat * clipPoint;
    return clipPoint;
}

void Camera::update() {
    auto vp = projectionMatrix() * viewMatrix();

    shaderData.setData(Camera::_viewMatrixProperty, viewMatrix());
    shaderData.setData(Camera::_projectionMatrixProperty, projectionMatrix());
    shaderData.setData(Camera::_vpMatrixProperty, vp);
    shaderData.setData(Camera::_inverseViewMatrixProperty, _transform->worldMatrix());
    shaderData.setData(Camera::_inverseProjectionMatrixProperty, inverseProjectionMatrix());
    shaderData.setData(Camera::_cameraPositionProperty, _transform->worldPosition());

    if (enableFrustumCulling && (_frustumViewChangeFlag->flag || _isFrustumProjectDirty)) {
        _frustum.calculateFromMatrix(vp);
        _frustumViewChangeFlag->flag = false;
        _isFrustumProjectDirty = false;
    }
}

Matrix4x4F Camera::invViewProjMat() {
    if (_isInvViewProjDirty->flag) {
        _isInvViewProjDirty->flag = false;
        _invViewProjMat = _transform->worldMatrix() * inverseProjectionMatrix();
    }
    return _invViewProjMat;
}

Matrix4x4F Camera::inverseProjectionMatrix() {
    if (_isInvProjMatDirty) {
        _isInvProjMatDirty = false;
        _inverseProjectionMatrix = projectionMatrix().inverse();
    }
    return _inverseProjectionMatrix;
}

void Camera::resize(uint32_t win_width, uint32_t win_height,
                    uint32_t fb_width, uint32_t fb_height) {
    _width = win_width;
    _height = win_height;
    _fbWidth = fb_width;
    _fbHeight = fb_height;
}

uint32_t Camera::width() const {
    return _width;
}

uint32_t Camera::height() const {
    return _height;
}

uint32_t Camera::framebufferWidth() const {
    return _fbWidth;
}

uint32_t Camera::framebufferHeight() const {
    return _fbHeight;
}

}// namespace vox
