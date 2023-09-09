//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "camera.h"
//#include "apps/renderer.h"
#include <pxr/base/gf/frustum.h>

using namespace pxr;

namespace vox {
namespace {
double constexpr _minFocalLength = 10.0;
double constexpr _maxFocalLength = 500.0;
}// namespace

Camera::Camera(Renderer *renderer) {
    _renderer = renderer;
    _rotation = pxr::GfVec3d(0.0);
    _focus = pxr::GfVec3d(0.0);
    _distance = 50.0;
    _scaleViewport = 1.0;
}

/// Initializes a camera instance and sets the camera configuration and the current renderer.
Camera::Camera(const pxr::GfCamera &sceneCamera, Renderer *renderer) {
    GfMatrix4d cameraTransform(1.0);
    cameraTransform = sceneCamera.GetTransform();

//    if (renderer->isZUp()) {
        cameraTransform = cameraTransform * GfMatrix4d().SetRotate(
                                                GfRotation(GfVec3d::XAxis(), -90.0));
//    }

    GfVec3d rotation = cameraTransform.DecomposeRotation(
        GfVec3d::YAxis(),
        GfVec3d::XAxis(),
        GfVec3d::ZAxis());

    _rotation = {rotation[1], rotation[0], rotation[2]};

    const GfFrustum frustum = sceneCamera.GetFrustum();
    const GfVec3d position = frustum.GetPosition();
    const GfVec3d viewDir = frustum.ComputeViewDirection();

    _distance = sceneCamera.GetFocusDistance();
    _focus = position + _distance * viewDir;
    _focalLength = sceneCamera.GetFocalLength();

    _renderer = renderer;
}

/// Sets the camera position based on the current focus.
void Camera::setPositionFromFocus() {
    GfRotation gfRotation = getRotation();
    GfVec3d viewDir = gfRotation.TransformDir(-GfVec3d::ZAxis());
    _position = _focus - _distance * viewDir;
}

/// Moves the camera by the specified delta and requests a new frame to render.
void Camera::panByDelta(pxr::GfVec2d delta) {
    GfRotation gfRotation = getRotation();
    GfMatrix4d cameraTransform = GfMatrix4d().SetRotate(gfRotation.GetInverse());

    GfVec4d xColumn = cameraTransform.GetColumn(0);
    GfVec4d yColumn = cameraTransform.GetColumn(1);

    GfVec3d xAxis(xColumn[0], xColumn[1], xColumn[2]);
    GfVec3d yAxis(yColumn[0], yColumn[1], yColumn[2]);
    double scale = _scaleBias * std::abs(_distance / 256.0);

    _focus += scale * (delta[0] * xAxis + delta[1] * yAxis);

//    _renderer->requestFrame();
}

/// Adjusts the x- and y-rotations and requests a new frame to render.
void Camera::rotateByDelta(pxr::GfVec2d delta) {
    _rotation += {delta[1], delta[0], 0.0f};
//    _renderer->requestFrame();
}
/// Adjusts the current zoom and requests a new frame to render.
void Camera::zoomByDelta(double delta) {
    if (_projection == Projection::Orthographic) {
        _scaleViewport += 0.1 * ((delta > 0) - (delta < 0));
        _scaleViewport = std::max(0.1, _scaleViewport);
    } else {
        _distance += delta * _scaleBias;
    }

//    _renderer->requestFrame();
}

/// Sets the new zoom and requests a new frame to render.
void Camera::setZoomFactor(double zoomFactor) {
    _focalLength = _standardFocalLength * zoomFactor;
//    _renderer->requestFrame();
}
/// Gets the zoom factor based on the focal length.
double Camera::getZoomFactor() const {
    return _focalLength / _standardFocalLength;
}

/// Compose a final rotation matrix and adjusts if the scene Z axis is up.
pxr::GfRotation Camera::getRotation() {
    GfRotation gfRotation = GfRotation(GfVec3d::ZAxis(), _rotation[2]) *
                            GfRotation(GfVec3d::XAxis(), _rotation[0]) *
                            GfRotation(GfVec3d::YAxis(), _rotation[1]);

//    if (_renderer->isZUp()) {
        gfRotation = gfRotation * GfRotation(GfVec3d::XAxis(), 90.0);
//    }

    return gfRotation;
}

/// Composes the final matrix for the camera.
pxr::GfMatrix4d Camera::getTransform() {
    GfRotation gfRotation = getRotation();
    GfMatrix4d cameraTransform(1.0);

    cameraTransform =
        GfMatrix4d().SetTranslate(GfVec3d(0.0, 0.0, _distance)) *
        GfMatrix4d().SetRotate(gfRotation) *
        GfMatrix4d().SetTranslate(_focus);

    return cameraTransform;
}

/// Builds the data structure for the camera shader parameters.
CameraParams Camera::getShaderParams() {
    CameraParams shaderParams{_rotation,
                              _focus,
                              _distance,
                              _focalLength,
                              _projection,
                              _leftBottomNear,
                              _rightTopFar,
                              _scaleViewport};

    return shaderParams;
}
}// namespace vox