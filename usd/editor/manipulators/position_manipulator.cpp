//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "position_manipulator.h"
#include "commands/commands.h"
#include "base/constants.h"
#include "base/geometric_functions.h"
#include "viewport.h"
#include <imgui.h>
#include <iostream>
#include <pxr/base/gf/matrix4f.h>

namespace vox {
/*
    TODO:  we ultimately want to be compatible with Vulkan / Metal, the following opengl/glsl code should really be using the
   ImGui API instead of native OpenGL code. Have a look before writing too much OpenGL code Also, it might be better to avoid
   developing a "parallel to imgui" event handling system because the gizmos are not implemented inside USD (may be they could ?
   couldn't they ?) or implemented in ImGui

    // NOTES from the USD doc:
    // If you need to compute the transform for multiple prims on a stage,
    // it will be much, much more efficient to instantiate a UsdGeomXformCache and query it directly; doing so will reuse
    // sub-computations shared by the prims.
    //
    // https://graphics.pixar.com/usd/docs/api/usd_geom_page_front.html
    // Matrices are laid out and indexed in row-major order, such that, given a GfMatrix4d datum mat, mat[3][1] denotes the second
    // column of the fourth row.

    // Reference on manipulators:
    // http://ed.ilogues.com/2018/06/27/translate-rotate-and-scale-manipulators-in-3d-modelling-programs
*/

static constexpr float axisSize = 100.f;

PositionManipulator::PositionManipulator() : _selectedAxis(None) {}

PositionManipulator::~PositionManipulator() {}

bool PositionManipulator::is_mouse_over(const Viewport &viewport) {
    if (_xformAPI || _xformable) {
        const auto &frustum = viewport.get_current_camera().GetFrustum();
        const auto mv = frustum.ComputeViewMatrix();
        const auto proj = frustum.ComputeProjectionMatrix();

        const auto toWorld = compute_manipulator_to_world_transform(viewport);

        // World position for origin is the pivot
        const auto pivot = toWorld.ExtractTranslation();

        // Axis are scaled to keep the same screen size
        const double axisLength = axisSize * viewport.compute_scale_factor(pivot, axisSize);

        // Local axis as draw in opengl
        const GfVec4d xAxis3d = GfVec4d(axisLength, 0.0, 0.0, 1.0) * toWorld;
        const GfVec4d yAxis3d = GfVec4d(0.0, axisLength, 0.0, 1.0) * toWorld;
        const GfVec4d zAxis3d = GfVec4d(0.0, 0.0, axisLength, 1.0) * toWorld;

        const auto originOnScreen = project_to_normalized_screen(mv, proj, pivot);
        const auto xAxisOnScreen = project_to_normalized_screen(mv, proj, GfVec3d(xAxis3d.data()));
        const auto yAxisOnScreen = project_to_normalized_screen(mv, proj, GfVec3d(yAxis3d.data()));
        const auto zAxisOnScreen = project_to_normalized_screen(mv, proj, GfVec3d(zAxis3d.data()));

        const auto pickBounds = viewport.get_picking_boundary_size();
        const auto mousePosition = viewport.get_mouse_position();

        // TODO: it looks like USD has function to compute segment, have a look !
        if (intersects_segment(xAxisOnScreen, originOnScreen, mousePosition, pickBounds)) {
            _selectedAxis = XAxis;
            return true;
        } else if (intersects_segment(yAxisOnScreen, originOnScreen, mousePosition, pickBounds)) {
            _selectedAxis = YAxis;
            return true;
        } else if (intersects_segment(zAxisOnScreen, originOnScreen, mousePosition, pickBounds)) {
            _selectedAxis = ZAxis;
            return true;
        }
    }
    _selectedAxis = None;
    return false;
}

// Same as rotation manipulator now -- TODO : share in a common class
void PositionManipulator::on_selection_change(Viewport &viewport) {
    auto &selection = viewport.get_selection();
    auto primPath = selection.get_anchor_prim_path(viewport.get_current_stage());
    _xformAPI = UsdGeomXformCommonAPI(viewport.get_current_stage()->GetPrimAtPath(primPath));
    _xformable = UsdGeomXformable(viewport.get_current_stage()->GetPrimAtPath(primPath));
}

GfMatrix4d PositionManipulator::compute_manipulator_to_world_transform(const Viewport &viewport) {
    if (_xformable) {
        const auto currentTime = viewport.get_current_time_code();
        GfMatrix4d localTransform;
        bool resetsXformStack = false;
        _xformable.GetLocalTransformation(&localTransform, &resetsXformStack, currentTime);
        const GfVec3d translation = localTransform.ExtractTranslation();
        const auto transMat = GfMatrix4d(1.0).SetTranslate(translation);
        // const auto pivotMat = GfMatrix4d(1.0).SetTranslate(pivot); // Do we need to get the pivot ?
        const auto parentToWorld = _xformable.ComputeParentToWorldTransform(currentTime);

        // We are just interested in the pivot position and the orientation
        const GfMatrix4d toManipulator = /* pivotMat * */ transMat * parentToWorld;// TODO pivot ?? or not pivot ???
        return toManipulator.GetOrthonormalized();
    }
    return GfMatrix4d();
}

inline void DrawArrow(ImDrawList *drawList, ImVec2 ori, ImVec2 tip, const ImVec4 &color, float thickness) {
    constexpr float arrowThickness = 20.f;
    drawList->AddLine(ori, tip, ImColor(color), thickness);
    const float len = sqrt((tip[0] - ori[0]) * (tip[0] - ori[0]) + (tip[1] - ori[1]) * (tip[1] - ori[1]));
    if (len <= arrowThickness)
        return;
    const ImVec2 vec(arrowThickness * (tip[0] - ori[0]) / len, arrowThickness * (tip[1] - ori[1]) / len);
    const ImVec2 pt1(tip[0] - vec[0] - 0.5 * vec[1], tip[1] - vec[1] + 0.5 * vec[0]);
    const ImVec2 pt2(tip[0] - vec[0] + 0.5 * vec[1], tip[1] - vec[1] - 0.5 * vec[0]);
    drawList->AddTriangleFilled(pt1, pt2, tip, ImColor(color));
}

template<int Axis>
inline ImColor AxisColor(int selectedAxis) {
    if (selectedAxis == Axis) {
        return ImColor(ImVec4(1.0, 1.0, 0.0, 1.0));
    } else {
        return ImColor(ImVec4(Axis == Manipulator::XAxis, Axis == Manipulator::YAxis, Axis == Manipulator::ZAxis, 1.0));
    }
}

void PositionManipulator::on_draw_frame(const Viewport &viewport) {
    if (_xformAPI || _xformable) {
        const auto &frustum = viewport.get_current_camera().GetFrustum();
        const auto mv = frustum.ComputeViewMatrix();
        const auto proj = frustum.ComputeProjectionMatrix();

        const auto toWorld = compute_manipulator_to_world_transform(viewport);

        // World position for origin is the pivot
        const auto pivot = toWorld.ExtractTranslation();

        // Axis are scaled to keep the same screen size
        const double axisLength = axisSize * viewport.compute_scale_factor(pivot, axisSize);

        // Local axis as draw in opengl
        const GfVec4d xAxis3d = GfVec4d(axisLength, 0.0, 0.0, 1.0) * toWorld;
        const GfVec4d yAxis3d = GfVec4d(0.0, axisLength, 0.0, 1.0) * toWorld;
        const GfVec4d zAxis3d = GfVec4d(0.0, 0.0, axisLength, 1.0) * toWorld;

        ImDrawList *drawList = ImGui::GetWindowDrawList();
        ImGuiViewport *viewport = ImGui::GetMainViewport();
        auto textureSize = GfVec2d(viewport->WorkSize[0], viewport->WorkSize[1]);

        const auto originOnScreen = project_to_texture_screen_space(mv, proj, textureSize, pivot);
        const auto xAxisOnScreen = project_to_texture_screen_space(mv, proj, textureSize, GfVec3d(xAxis3d.data()));
        const auto yAxisOnScreen = project_to_texture_screen_space(mv, proj, textureSize, GfVec3d(yAxis3d.data()));
        const auto zAxisOnScreen = project_to_texture_screen_space(mv, proj, textureSize, GfVec3d(zAxis3d.data()));

        // TODO : Draw grey if not editable

        DrawArrow(drawList, ImVec2(originOnScreen[0], originOnScreen[1]), ImVec2(xAxisOnScreen[0], xAxisOnScreen[1]),
                  AxisColor<XAxis>(_selectedAxis), 3);
        DrawArrow(drawList, ImVec2(originOnScreen[0], originOnScreen[1]), ImVec2(yAxisOnScreen[0], yAxisOnScreen[1]),
                  AxisColor<YAxis>(_selectedAxis), 3);
        DrawArrow(drawList, ImVec2(originOnScreen[0], originOnScreen[1]), ImVec2(zAxisOnScreen[0], zAxisOnScreen[1]),
                  AxisColor<ZAxis>(_selectedAxis), 3);
    }
}

void PositionManipulator::on_begin_edition(Viewport &viewport) {
    // Save original translation values
    GfVec3f scale, pivot, rotation;
    GfMatrix4d localTransform;
    bool resetsXformStack = false;
    _xformable.GetLocalTransformation(&localTransform, &resetsXformStack, viewport.get_current_time_code());
    _translationOnBegin = localTransform.ExtractTranslation();

    // Save mouse position on selected axis
    const GfMatrix4d objectTransform = compute_manipulator_to_world_transform(viewport);
    _axisLine = GfLine(objectTransform.ExtractTranslation(), objectTransform.GetRow3(_selectedAxis));
    project_mouse_on_axis(viewport, _originMouseOnAxis);

    begin_edition(viewport.get_current_stage());
}

Manipulator *PositionManipulator::on_update(Viewport &viewport) {
    if (ImGui::IsMouseReleased(0)) {
        return viewport.get_manipulator<MouseHoverManipulator>();
    }

    if (_xformable && _selectedAxis < 3) {
        GfVec3d mouseOnAxis;
        project_mouse_on_axis(viewport, mouseOnAxis);

        // Get the sign
        double ori = 0.0;// = _axisLine.GetDirection()*_originMouseOnAxis;
        double cur = 0.0;// = _axisLine.GetDirection()*mouseOnAxis;
        _axisLine.FindClosestPoint(_originMouseOnAxis, &ori);
        _axisLine.FindClosestPoint(mouseOnAxis, &cur);
        double sign = cur > ori ? 1.0 : -1.0;

        GfVec3d translation = _translationOnBegin;
        translation[_selectedAxis] += sign * (_originMouseOnAxis - mouseOnAxis).GetLength();
        if (_xformAPI) {
            _xformAPI.SetTranslate(translation, get_edition_time_code(viewport));
        } else {
            bool reset = false;
            auto ops = _xformable.GetOrderedXformOps(&reset);
            if (ops.size() == 1 && ops[0].GetOpType() == UsdGeomXformOp::Type::TypeTransform) {
                GfMatrix4d current = ops[0].GetOpTransform(get_edition_time_code(viewport));
                current.SetTranslateOnly(translation);// TODO: what happens if there is a pivot ???
                ops[0].Set(current, get_edition_time_code(viewport));
            }
        }
    }
    return this;
};

void PositionManipulator::on_end_edition(Viewport &) { end_edition(); };

///
void PositionManipulator::project_mouse_on_axis(const Viewport &viewport, GfVec3d &linePoint) {
    if ((_xformAPI || _xformable) && _selectedAxis < 3) {
        GfVec3d rayPoint;
        double a = 0;
        double b = 0;
        const auto &frustum = viewport.get_current_camera().GetFrustum();
        const auto mouseRay = frustum.ComputeRay(viewport.get_mouse_position());
        GfFindClosestPoints(mouseRay, _axisLine, &rayPoint, &linePoint, &a, &b);
    }
}

UsdTimeCode PositionManipulator::get_edition_time_code(const Viewport &viewport) {
    std::vector<double> timeSamples;// TODO: is there a faster way to know it the xformable has timesamples ?
    const auto xformable = UsdGeomXformable(_xformAPI.GetPrim());
    xformable.GetTimeSamples(&timeSamples);
    if (timeSamples.empty()) {
        return UsdTimeCode::Default();
    } else {
        return viewport.get_current_time_code();
    }
}

}// namespace vox
