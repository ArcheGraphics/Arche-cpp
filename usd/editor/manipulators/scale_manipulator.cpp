//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "scale_manipulator.h"
#include "commands/commands.h"
#include "base/geometric_functions.h"
#include "viewport.h"
#include <pxr/base/gf/matrix4f.h>

namespace vox {
/*
 *   Same code as PositionManipulator
 *  TODO: factor code, probably under a  TRSManipulator class
 */
static constexpr GLfloat axisSize = 100.f;

ScaleManipulator::ScaleManipulator() : _selectedAxis(None) {}

ScaleManipulator::~ScaleManipulator() = default;

bool ScaleManipulator::is_mouse_over(const Viewport &viewport) {

    if (_xformable) {
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
void ScaleManipulator::on_selection_change(Viewport &viewport) {
    auto &selection = viewport.get_selection();
    auto primPath = selection.get_anchor_prim_path(viewport.get_current_stage());
    _xformAPI = UsdGeomXformCommonAPI(viewport.get_current_stage()->GetPrimAtPath(primPath));
    _xformable = UsdGeomXformable(_xformAPI.GetPrim());
}

GfMatrix4d ScaleManipulator::compute_manipulator_to_world_transform(const Viewport &viewport) {
    if (_xformable) {
        const auto currentTime = viewport.get_current_time_code();
        GfVec3d translation{};
        GfVec3f scale{}, pivot{}, rotation{};
        UsdGeomXformCommonAPI::RotationOrder rotOrder;
        _xformAPI.GetXformVectorsByAccumulation(&translation, &rotation, &scale, &pivot, &rotOrder, currentTime);
        const auto transMat = GfMatrix4d(1.0).SetTranslate(translation);
        const auto pivotMat = GfMatrix4d(1.0).SetTranslate(pivot);
        const auto rotMat = _xformAPI.GetRotationTransform(rotation, rotOrder);
        const auto parentToWorld = _xformable.ComputeParentToWorldTransform(currentTime);

        // We are just interested in the pivot position and the orientation
        const GfMatrix4d toManipulator = rotMat * pivotMat * transMat * parentToWorld;
        return toManipulator.GetOrthonormalized();
    }
    return {};
}

template<int Axis>
inline ImColor AxisColor(int selectedAxis) {
    if (selectedAxis == Axis) {
        return ImColor(ImVec4(1.0, 1.0, 0.0, 1.0));
    } else {
        return ImColor(ImVec4(Axis == Manipulator::XAxis, Axis == Manipulator::YAxis, Axis == Manipulator::ZAxis, 1.0));
    }
}

// TODO: same as rotation manipulator, share in a base class
void ScaleManipulator::on_draw_frame(const Viewport &viewport) {
    if (_xformable) {
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

        drawList->AddLine(ImVec2(originOnScreen[0], originOnScreen[1]), ImVec2(xAxisOnScreen[0], xAxisOnScreen[1]),
                          AxisColor<XAxis>(_selectedAxis), 3);
        drawList->AddLine(ImVec2(originOnScreen[0], originOnScreen[1]), ImVec2(yAxisOnScreen[0], yAxisOnScreen[1]),
                          AxisColor<YAxis>(_selectedAxis), 3);
        drawList->AddLine(ImVec2(originOnScreen[0], originOnScreen[1]), ImVec2(zAxisOnScreen[0], zAxisOnScreen[1]),
                          AxisColor<ZAxis>(_selectedAxis), 3);
        drawList->AddCircleFilled(ImVec2(xAxisOnScreen[0], xAxisOnScreen[1]), 10, ImColor(ImVec4(1.0, 0, 0, 1)), 32);
        drawList->AddCircleFilled(ImVec2(yAxisOnScreen[0], yAxisOnScreen[1]), 10, ImColor(ImVec4(0.0, 1.0, 0, 1)), 32);
        drawList->AddCircleFilled(ImVec2(zAxisOnScreen[0], zAxisOnScreen[1]), 10, ImColor(ImVec4(0.0, 0, 1.0, 1)), 32);
    }
}

void ScaleManipulator::on_begin_edition(Viewport &viewport) {
    // Save original translation values
    GfVec3d translation{};
    GfVec3f pivot{}, rotation{};
    UsdGeomXformCommonAPI::RotationOrder rotOrder;
    _xformAPI.GetXformVectorsByAccumulation(&translation, &rotation, &_scaleOnBegin, &pivot, &rotOrder,
                                            viewport.get_current_time_code());

    // Save mouse position on selected axis
    const GfMatrix4d objectTransform = compute_manipulator_to_world_transform(viewport);
    _axisLine = GfLine(objectTransform.ExtractTranslation(), objectTransform.GetRow3(_selectedAxis));
    project_mouse_on_axis(viewport, _originMouseOnAxis);

    begin_edition(viewport.get_current_stage());
}

Manipulator *ScaleManipulator::on_update(Viewport &viewport) {
    if (ImGui::IsMouseReleased(0)) {
        return viewport.get_manipulator<MouseHoverManipulator>();
    }

    if (_xformable && _selectedAxis < 3) {
        GfVec3d mouseOnAxis{};
        project_mouse_on_axis(viewport, mouseOnAxis);

        // Get the sign
        double ori;
        double cur;
        _axisLine.FindClosestPoint(_originMouseOnAxis, &ori);
        _axisLine.FindClosestPoint(mouseOnAxis, &cur);

        GfVec3f scale = _scaleOnBegin;

        // TODO division per zero check
        scale[_selectedAxis] = _scaleOnBegin[_selectedAxis] * mouseOnAxis.GetLength() / _originMouseOnAxis.GetLength();

        if (ImGui::IsKeyDown(ImGuiKey_LeftShift)) {
            scale[XAxis] = _scaleOnBegin[XAxis] * mouseOnAxis.GetLength() / _originMouseOnAxis.GetLength();
            scale[YAxis] = _scaleOnBegin[YAxis] * mouseOnAxis.GetLength() / _originMouseOnAxis.GetLength();
            scale[ZAxis] = _scaleOnBegin[XAxis] * mouseOnAxis.GetLength() / _originMouseOnAxis.GetLength();
        } else {
            scale[_selectedAxis] = _scaleOnBegin[_selectedAxis] * mouseOnAxis.GetLength() / _originMouseOnAxis.GetLength();
        }

        if (_xformAPI) {
            _xformAPI.SetScale(scale, get_edition_time_code(viewport));
        } else {
            bool reset = false;
            auto ops = _xformable.GetOrderedXformOps(&reset);
            if (ops.size() == 1 && ops[0].GetOpType() == UsdGeomXformOp::Type::TypeTransform) {
                GfVec3d translation{};
                GfVec3f scale_{}, pivot{}, rotation{};
                UsdGeomXformCommonAPI::RotationOrder rotOrder;
                _xformAPI.GetXformVectorsByAccumulation(&translation, &rotation, &scale_, &pivot, &rotOrder,
                                                        get_edition_time_code(viewport));
                const auto transMat = GfMatrix4d(1.0).SetTranslate(translation);
                const auto rotMat = _xformAPI.GetRotationTransform(rotation, rotOrder);
                GfMatrix4d current = GfMatrix4d().SetScale(scale) * rotMat * transMat;
                ops[0].Set(current, get_edition_time_code(viewport));
            }
        }
    }
    return this;
}

void ScaleManipulator::on_end_edition(Viewport &) { end_edition(); }

///
void ScaleManipulator::project_mouse_on_axis(const Viewport &viewport, GfVec3d &linePoint) {
    if (_xformable && _selectedAxis < 3) {
        GfVec3d rayPoint{};
        double a = 0;
        double b = 0;
        const auto &frustum = viewport.get_current_camera().GetFrustum();
        const auto mouseRay = frustum.ComputeRay(viewport.get_mouse_position());
        GfFindClosestPoints(mouseRay, _axisLine, &rayPoint, &linePoint, &a, &b);
    }
}

UsdTimeCode ScaleManipulator::get_edition_time_code(const Viewport &viewport) {
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