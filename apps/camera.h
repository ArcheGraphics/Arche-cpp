//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <pxr/base/gf/vec2d.h>
#include <pxr/base/gf/vec3d.h>
#include <pxr/base/gf/camera.h>
#include <pxr/base/gf/matrix4d.h>
#include <pxr/base/gf/rotation.h>

namespace vox {
class Viewport;

enum class Projection {
    Perspective = 0,
    Orthographic
};

struct CameraParams {
    pxr::GfVec3d rotation;
    pxr::GfVec3d focus;
    double distance;
    double focalLength;
    Projection projection;
    pxr::GfVec3d leftBottomNear;
    pxr::GfVec3d rightTopFar;
    double scaleViewport;
};

class Camera {
public:
    /// Initializes the class and sets the current renderer.
    explicit Camera(Viewport *viewport);

    /// Initializes a camera instance and sets the camera configuration and the current renderer.
    Camera(const pxr::GfCamera &sceneCamera, Viewport *viewport);

    /// Sets the camera position based on the current focus.
    void setPositionFromFocus();

    /// Moves the camera by the specified delta and requests a new frame to render.
    void panByDelta(pxr::GfVec2d delta);

    /// Adjusts the x- and y-rotations and requests a new frame to render.
    void rotateByDelta(pxr::GfVec2d delta);
    /// Adjusts the current zoom and requests a new frame to render.
    void zoomByDelta(double delta);

    /// Sets the new zoom and requests a new frame to render.
    void setZoomFactor(double zoomFactor);
    /// Gets the zoom factor based on the focal length.
    [[nodiscard]] double getZoomFactor() const;

    /// Compose a final rotation matrix and adjusts if the scene Z axis is up.
    pxr::GfRotation getRotation();
    /// Composes the final matrix for the camera.
    pxr::GfMatrix4d getTransform();

    /// Builds the data structure for the camera shader parameters.
    CameraParams getShaderParams();

public:
    inline pxr::GfVec3d position() { return _position; }
    inline pxr::GfVec3d rotation() { return _rotation; }
    inline pxr::GfVec3d focus() { return _focus; }
    [[nodiscard]] inline double distance() const { return _distance; }
    [[nodiscard]] inline double focalLength() const { return _focalLength; }
    [[nodiscard]] inline double standardFocalLength() const { return _standardFocalLength; }
    [[nodiscard]] inline double scaleBias() const { return _scaleBias; }
    inline Projection projection() { return _projection; }
    inline pxr::GfVec3d leftBottomNear() { return _leftBottomNear; }
    inline pxr::GfVec3d rightTopFar() { return _rightTopFar; }
    [[nodiscard]] inline double scaleViewport() const { return _scaleViewport; }

    inline void setPosition(const pxr::GfVec3d &value) { _position = value; }
    inline void setRotation(const pxr::GfVec3d &value) { _rotation = value; }
    inline void setFocus(const pxr::GfVec3d &value) { _focus = value; }
    inline void setDistance(double value) { _distance = value; }
    inline void setFocalLength(double value) { _focalLength = value; }
    inline void setStandardFocalLength(double value) { _standardFocalLength = value; }
    inline void setScaleBias(double value) { _scaleBias = value; }
    inline void setProjection(Projection value) { _projection = value; }
    inline void setLeftBottomNear(const pxr::GfVec3d &value) { _leftBottomNear = value; }
    inline void setRightTopFar(const pxr::GfVec3d &value) { _rightTopFar = value; }
    inline void setScaleViewport(double value) { _scaleViewport = value; }

private:
    Viewport *_viewport;

    pxr::GfVec3d _position{};
    pxr::GfVec3d _rotation{};
    pxr::GfVec3d _focus{};
    double _distance;
    double _focalLength{};
    double _standardFocalLength{};
    double _scaleBias{};
    Projection _projection;
    pxr::GfVec3d _leftBottomNear{};
    pxr::GfVec3d _rightTopFar{};
    double _scaleViewport{};
};

}// namespace vox