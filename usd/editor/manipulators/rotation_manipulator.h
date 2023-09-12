//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <pxr/base/gf/matrix4d.h>
#include <pxr/base/gf/matrix4f.h>
#include <pxr/base/gf/vec3f.h>
#include <pxr/base/gf/vec2d.h>
#include <pxr/usd/usdGeom/gprim.h>
#include <pxr/usd/usdGeom/xformCommonAPI.h>
#include <imgui.h>
#include "manipulator.h"

PXR_NAMESPACE_USING_DIRECTIVE

namespace vox {
class RotationManipulator : public Manipulator {

public:
    RotationManipulator();
    ~RotationManipulator() override;

    /// From ViewportEditor
    void on_begin_edition(Viewport &) override;
    Manipulator *on_update(Viewport &) override;
    void on_end_edition(Viewport &) override;

    /// Return true if the mouse is over this manipulator in the viewport passed in argument
    bool is_mouse_over(const Viewport &) override;

    /// Draw the translate manipulator as seen in the viewport
    void on_draw_frame(const Viewport &) override;

    /// Called when the viewport changes its selection
    void on_selection_change(Viewport &) override;

private:
    UsdTimeCode get_edition_time_code(const Viewport &);
    UsdTimeCode get_viewport_time_code(const Viewport &);

    GfVec3d compute_clock_hand_vector(Viewport &viewport);

    GfMatrix4d compute_manipulator_to_world_transform(const Viewport &viewport);
    ManipulatorAxis _selectedAxis;

    UsdGeomXformCommonAPI _xformAPI;
    UsdGeomXformable _xformable;

    GfVec3d _rotateFrom{};
    GfMatrix4d _rotateMatrixOnBegin{};

    GfVec3d _planeOrigin3d{};   // Global
    GfVec3d _planeNormal3d{};   // TODO rename global
    GfVec3d _localPlaneNormal{};// Local

    std::vector<GfVec2d> _manipulatorCircles;
    std::vector<ImVec2> _manipulator2dPoints;
};

}// namespace vox