//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <pxr/base/gf/camera.h>
#include <pxr/base/gf/vec2i.h>

namespace vox {
/// Type of camera movement
/// TODO add arcball and turntable options
enum struct MovementType { None,
                           Orbit,
                           Truck,
                           Dolly };

PXR_NAMESPACE_USING_DIRECTIVE

/// Camera manipulator handling many types of camera movement
class CameraRig {
public:
    explicit CameraRig(const GfVec2i &viewportSize, bool isZUp = false);
    ~CameraRig() = default;

    // No copy allowed
    CameraRig(const CameraRig &) = delete;
    CameraRig &operator=(const CameraRig &) = delete;

    /// Reset the camera position to the original position
    void reset_position(GfCamera &);

    /// Set the type of movement
    void set_movement_type(MovementType mode) { _movementType = mode; }

    /// Update the camera position depending on the Movement type
    bool move(GfCamera &, double deltaX, double deltaY);

    /// Frame the camera so that the bounding box is visible.
    void frame_bounding_box(GfCamera &, const GfBBox3d &);

    /// Set if the up vector is Z
    void set_z_is_up(bool);

    ///
    void set_viewport_size(const GfVec2i &viewportSize) { _viewportSize = viewportSize; }

private:
    MovementType _movementType;
    GfMatrix4d _zUpMatrix{};
    double _selectionSize;/// Last "FrameBoundingBox" selection size
    GfVec2i _viewportSize;
    float _dist = 100;
};
}// namespace vox