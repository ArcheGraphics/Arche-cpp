//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <pxr/base/gf/matrix4d.h>
#include <pxr/base/gf/vec3f.h>
#include <pxr/base/gf/vec2d.h>
#include <pxr/base/gf/line.h>
#include <pxr/usd/usdGeom/gprim.h>
#include <pxr/usd/usdGeom/xformCommonAPI.h>

#include "manipulator.h"

PXR_NAMESPACE_USING_DIRECTIVE

namespace vox {
// Scale manipulator
// This is a almost perfect copy of the position manipulator, this needs to be factored in
class ScaleManipulator : public Manipulator {

public:
    ScaleManipulator();
    ~ScaleManipulator();

    void on_begin_edition(Viewport &) override;
    Manipulator *on_update(Viewport &) override;
    void on_end_edition(Viewport &) override;

    /// Return true if the mouse is over this manipulator for the viewport passed in argument
    bool is_mouse_over(const Viewport &) override;

    /// Draw the translate manipulator as seen in the viewport
    void on_draw_frame(const Viewport &) override;

    /// Called when the viewport changes its selection
    void on_selection_change(Viewport &) override;

private:
    void project_mouse_on_axis(const Viewport &viewport, GfVec3d &closestPoint);
    GfMatrix4d compute_manipulator_to_world_transform(const Viewport &viewport);

    UsdTimeCode get_edition_time_code(const Viewport &viewport);

    ManipulatorAxis _selectedAxis;

    GfVec3d _originMouseOnAxis;
    GfVec3f _scaleOnBegin;
    GfLine _axisLine;

    UsdGeomXformCommonAPI _xformAPI;
    UsdGeomXformable _xformable;
};

}// namespace vox