//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <iostream>
#include <pxr/base/gf/line.h>
#include <pxr/base/gf/math.h>
#include <vector>

#include "commands/commands.h"
#include "base/constants.h"
#include "base/geometric_functions.h"
#include "rotation_manipulator.h"
#include "viewport.h"

namespace vox {
static constexpr float axisSize = 1.2f;
static constexpr int nbSegments = 1024;// nb segments per circle

static void create_circle(std::vector<GfVec2d> &points) {
    points.clear();
    points.reserve(3 * nbSegments);
    for (int i = 0; i < nbSegments; ++i) {
        points.emplace_back(cos(2.f * std::numbers::pi_v<float> * i / nbSegments), sin(2.f * std::numbers::pi_v<float> * i / nbSegments));
    }
}

RotationManipulator::RotationManipulator() : _selectedAxis(None) {
    create_circle(_manipulatorCircles);
    _manipulator2dPoints.reserve(3 * nbSegments);
}

RotationManipulator::~RotationManipulator() {}

static bool intersects_unit_circle(const GfVec3d &planeNormal3d, const GfVec3d &planeOrigin3d, const GfRay &ray, float scale) {
    if (scale == 0)
        return false;

    GfPlane plane(planeNormal3d, planeOrigin3d);
    double distance = 0.0;
    constexpr float limit = 0.1;// TODO: this should be computed relative to the apparent width of the circle lines
    if (ray.Intersect(plane, &distance) && distance > 0.0) {
        const auto intersection = ray.GetPoint(distance);
        const auto segment = planeOrigin3d - intersection;

        if (fabs(1.f - segment.GetLength() / scale) < limit) {
            return true;
        }
    }
    return false;
}

bool RotationManipulator::is_mouse_over(const Viewport &viewport) {
    if (_xformable) {
        const GfVec2d mousePosition = viewport.get_mouse_position();
        const auto &frustum = viewport.get_current_camera().GetFrustum();
        const GfRay ray = frustum.ComputeRay(mousePosition);
        const auto manipulatorCoordinates = compute_manipulator_to_world_transform(viewport);
        const GfVec3d xAxis = manipulatorCoordinates.GetRow3(0);
        const GfVec3d yAxis = manipulatorCoordinates.GetRow3(1);
        const GfVec3d zAxis = manipulatorCoordinates.GetRow3(2);
        const GfVec3d origin = manipulatorCoordinates.GetRow3(3);

        // Circles are scaled to keep the same screen size
        double scale = viewport.compute_scale_factor(origin, axisSize);

        if (intersects_unit_circle(xAxis, origin, ray, scale)) {
            _selectedAxis = XAxis;
            return true;
        } else if (intersects_unit_circle(yAxis, origin, ray, scale)) {
            _selectedAxis = YAxis;
            return true;
        } else if (intersects_unit_circle(zAxis, origin, ray, scale)) {
            _selectedAxis = ZAxis;
            return true;
        }
    }
    _selectedAxis = None;
    return false;
}

void RotationManipulator::on_selection_change(Viewport &viewport) {
    // TODO: we should set here if the new selection will be editable or not
    auto &selection = viewport.get_selection();
    auto primPath = selection.get_anchor_prim_path(viewport.get_current_stage());
    _xformAPI = UsdGeomXformCommonAPI(viewport.get_current_stage()->GetPrimAtPath(primPath));
    _xformable = UsdGeomXformable(_xformAPI.GetPrim());
}

GfMatrix4d RotationManipulator::compute_manipulator_to_world_transform(const Viewport &viewport) {
    if (_xformable) {
        const auto currentTime = get_viewport_time_code(viewport);
        GfVec3d translation;
        GfVec3f rotation, scale, pivot;

        UsdGeomXformCommonAPI::RotationOrder rotOrder;
        _xformAPI.GetXformVectorsByAccumulation(&translation, &rotation, &scale, &pivot, &rotOrder, currentTime);
        GfMatrix4d rotMat =
            UsdGeomXformOp::GetOpTransform(UsdGeomXformCommonAPI::ConvertRotationOrderToOpType(rotOrder), VtValue(rotation));

        const auto transMat = GfMatrix4d(1.0).SetTranslate(translation);
        const auto pivotMat = GfMatrix4d(1.0).SetTranslate(pivot);
        // const auto xformable = UsdGeomXformable(_xformAPI.GetPrim());
        const auto parentToWorldMat = _xformable.ComputeParentToWorldTransform(currentTime);

        // We are just interested in the pivot position and the orientation
        const GfMatrix4d toManipulator = rotMat * pivotMat * transMat * parentToWorldMat;

        return toManipulator.GetOrthonormalized();
    }
    return GfMatrix4d(1.0);
}

// Probably not the most efficient implementation, but it avoids sin/cos/arctan and works only with vectors
// It could be fun to spend some time looking for alternatives and benchmark them, even if it's really not
// critical at this point. Use a binary search algorithm on rotated array to find the begining of the visible part,
// then iterate but with less resolution (1 point every 3 samples for example)
template<int axis>
inline static size_t project_half_circle(const std::vector<GfVec2d> &_manipulatorCircles, double scale,
                                         const GfVec3d &cameraPosition, const GfVec4d &manipulatorOrigin, const GfMatrix4d &mv,
                                         const GfMatrix4d &proj, const GfVec2d &textureSize, const GfMatrix4d &toWorld,
                                         std::vector<ImVec2> &_manipulator2dPoints) {
    int rotateIndex = -1;
    size_t circleBegin = _manipulator2dPoints.size();
    for (int i = 0; i < _manipulatorCircles.size(); ++i) {
        const GfVec4d pointInLocalSpace =
            axis == 0 ? GfVec4d(0.0, _manipulatorCircles[i][0], _manipulatorCircles[i][1], 1.0) : (axis == 1 ? GfVec4d(_manipulatorCircles[i][0], 0.0, _manipulatorCircles[i][1], 1.0) : GfVec4d(_manipulatorCircles[i][0], _manipulatorCircles[i][1], 0.0, 1.0));
        const GfVec4d pointInWorldSpace = GfCompMult(GfVec4d(scale, scale, scale, 1.0), pointInLocalSpace) * toWorld;
        const double dot = GfDot((GfVec4d(cameraPosition[0], cameraPosition[1], cameraPosition[2], 1.0) - manipulatorOrigin),
                                 (pointInWorldSpace - manipulatorOrigin));
        if (dot >= 0.0) {
            const GfVec2d pointInPixelSpace =
                project_to_texture_screen_space(mv, proj, textureSize, GfVec3d(pointInWorldSpace.data()));
            _manipulator2dPoints.emplace_back(pointInPixelSpace[0], pointInPixelSpace[1]);
        } else {
            if (rotateIndex == -1 && _manipulator2dPoints.size() > circleBegin) {
                rotateIndex = _manipulator2dPoints.size();
            }
        }
    };

    // Reorder the points so there is no visible straight line. This could also be costly
    if (rotateIndex != -1) {
        std::rotate(_manipulator2dPoints.begin() + circleBegin,
                    _manipulator2dPoints.begin() + circleBegin + rotateIndex - circleBegin, _manipulator2dPoints.end());
        rotateIndex = -1;
    }
    return _manipulator2dPoints.size();
};

template<int Axis>
inline ImColor AxisColor(int selectedAxis) {
    if (selectedAxis == Axis) {
        return ImColor(ImVec4(1.0, 1.0, 0.0, 1.0));
    } else {
        return ImColor(ImVec4(Axis == Manipulator::XAxis, Axis == Manipulator::YAxis, Axis == Manipulator::ZAxis, 1.0));
    }
}

void RotationManipulator::on_draw_frame(const Viewport &viewport) {
    if (_xformable) {
        const auto &camera = viewport.get_current_camera();
        auto mv = camera.GetFrustum().ComputeViewMatrix();
        auto proj = camera.GetFrustum().ComputeProjectionMatrix();
        auto manipulatorCoordinates = compute_manipulator_to_world_transform(viewport);
        auto origin = manipulatorCoordinates.ExtractTranslation();
        auto cameraPosition = mv.GetInverse().ExtractTranslation();

        // Circles must be scaled to keep the same screen size
        double scale = viewport.compute_scale_factor(origin, axisSize);

        const auto toWorld = compute_manipulator_to_world_transform(viewport);

        ImDrawList *draw_list = ImGui::GetWindowDrawList();
        ImGuiViewport *viewport = ImGui::GetMainViewport();
        auto textureSize = GfVec2d(viewport->WorkSize[0], viewport->WorkSize[1]);

        const GfVec4d manipulatorOrigin = GfVec4d(origin[0], origin[1], origin[2], 1.0);
        // Fill _manipulator2dPoints
        _manipulator2dPoints.clear();
        const size_t xEnd = project_half_circle<0>(_manipulatorCircles, scale, cameraPosition, manipulatorOrigin, mv, proj,
                                                   textureSize, toWorld, _manipulator2dPoints);
        const size_t yEnd = project_half_circle<1>(_manipulatorCircles, scale, cameraPosition, manipulatorOrigin, mv, proj,
                                                   textureSize, toWorld, _manipulator2dPoints);
        const size_t zEnd = project_half_circle<2>(_manipulatorCircles, scale, cameraPosition, manipulatorOrigin, mv, proj,
                                                   textureSize, toWorld, _manipulator2dPoints);

        draw_list->AddPolyline(_manipulator2dPoints.data(), xEnd, AxisColor<XAxis>(_selectedAxis), ImDrawFlags_None, 3);
        draw_list->AddPolyline(_manipulator2dPoints.data() + xEnd, yEnd - xEnd, AxisColor<YAxis>(_selectedAxis), ImDrawFlags_None, 3);
        draw_list->AddPolyline(_manipulator2dPoints.data() + yEnd, zEnd - yEnd, AxisColor<ZAxis>(_selectedAxis), ImDrawFlags_None, 3);
    }
}

// TODO: find a more meaningful function name
GfVec3d RotationManipulator::compute_clock_hand_vector(Viewport &viewport) {
    const GfPlane plane(_planeNormal3d, _planeOrigin3d);
    double distance = 0.0;

    const GfVec2d mousePosition = viewport.get_mouse_position();
    const GfFrustum frustum = viewport.get_current_camera().GetFrustum();
    const GfRay ray = frustum.ComputeRay(mousePosition);
    if (ray.Intersect(plane, &distance)) {
        const auto intersection = ray.GetPoint(distance);
        return _planeOrigin3d - intersection;
    }

    return GfVec3d();
}

void RotationManipulator::on_begin_edition(Viewport &viewport) {
    if (_xformable) {
        const auto manipulatorCoordinates = compute_manipulator_to_world_transform(viewport);
        _planeOrigin3d = manipulatorCoordinates.ExtractTranslation();
        _planeNormal3d = GfVec3d();// default init
        if (_selectedAxis == XAxis) {
            _planeNormal3d = manipulatorCoordinates.GetRow3(0);
        } else if (_selectedAxis == YAxis) {
            _planeNormal3d = manipulatorCoordinates.GetRow3(1);
        } else if (_selectedAxis == ZAxis) {
            _planeNormal3d = manipulatorCoordinates.GetRow3(2);
        }

        // Compute rotation starting point
        _rotateFrom = compute_clock_hand_vector(viewport);

        // Save the rotation values
        GfVec3d translation;
        GfVec3f rotation, scale, pivot;
        UsdGeomXformCommonAPI::RotationOrder rotOrder;
        _xformAPI.GetXformVectorsByAccumulation(&translation, &rotation, &scale, &pivot, &rotOrder,
                                                get_viewport_time_code(viewport));

        _rotateMatrixOnBegin =
            UsdGeomXformOp::GetOpTransform(UsdGeomXformCommonAPI::ConvertRotationOrderToOpType(rotOrder), VtValue(rotation));
    }
    begin_edition(viewport.get_current_stage());
}

Manipulator *RotationManipulator::on_update(Viewport &viewport) {
    if (ImGui::IsMouseReleased(0)) {
        return viewport.get_manipulator<MouseHoverManipulator>();
    }
    if (_xformable && _selectedAxis != None) {

        // Compute rotation angle in world coordinates
        const GfVec3d rotateTo = compute_clock_hand_vector(viewport);
        const GfRotation worldRotation(_rotateFrom, rotateTo);
        const auto axisSign = _planeNormal3d * worldRotation.GetAxis() > 0 ? 1.0 : -1.0;

        // Compute rotation axis in local coordinates
        // We use the plane normal as the rotation between _rotateFrom and rotateTo might not land exactly on the rotation axis
        const GfVec3d xAxis = _rotateMatrixOnBegin.GetRow3(0);
        const GfVec3d yAxis = _rotateMatrixOnBegin.GetRow3(1);
        const GfVec3d zAxis = _rotateMatrixOnBegin.GetRow3(2);

        GfVec3d localPlaneNormal = xAxis;// default init
        if (_selectedAxis == XAxis) {
            localPlaneNormal = xAxis;
        } else if (_selectedAxis == YAxis) {
            localPlaneNormal = yAxis;
        } else if (_selectedAxis == ZAxis) {
            localPlaneNormal = zAxis;
        }

        const GfRotation deltaRotation(localPlaneNormal * axisSign, worldRotation.GetAngle());
        // NOTE: should that be _rotateMatrixOnBegin * deltaRotation instead ? the formula for opTrans use this order
        const GfMatrix4d resultingRotation = GfMatrix4d(1.0).SetRotate(deltaRotation) * _rotateMatrixOnBegin;

        // Get latest rotation values to give a hint to the decompose function
        GfVec3d translation;
        GfVec3f rotation, scale, pivot;
        UsdGeomXformCommonAPI::RotationOrder rotOrder;
        _xformAPI.GetXformVectorsByAccumulation(&translation, &rotation, &scale, &pivot, &rotOrder,
                                                get_viewport_time_code(viewport));
        double thetaTw = GfDegreesToRadians(rotation[0]);
        double thetaFB = GfDegreesToRadians(rotation[1]);
        double thetaLR = GfDegreesToRadians(rotation[2]);
        double thetaSw = 0.0;
        // Decompose the matrix in angle values
        GfRotation::DecomposeRotation(resultingRotation, xAxis, yAxis, zAxis, 1.0, &thetaTw, &thetaFB, &thetaLR, &thetaSw, true);
        const GfVec3f newRotationValues =
            GfVec3f(GfRadiansToDegrees(thetaTw), GfRadiansToDegrees(thetaFB), GfRadiansToDegrees(thetaLR));
        if (_xformAPI) {
            _xformAPI.SetRotate(newRotationValues, rotOrder, get_edition_time_code(viewport));
        } else {// Modify only if we have a single matrix
            bool reset = false;
            auto ops = _xformable.GetOrderedXformOps(&reset);
            if (ops.size() == 1 && ops[0].GetOpType() == UsdGeomXformOp::Type::TypeTransform) {
                // [ "xformOp:translate", "xformOp:translate:pivot", "xformOp:rotateXYZ",
                // "xformOp:scale", "!invert!xformOp:translate:pivot" ] - No pivot here
                GfMatrix4d current = GfMatrix4d().SetScale(scale) * _rotateMatrixOnBegin *
                                     GfMatrix4d(1.0).SetRotate(deltaRotation) * GfMatrix4d().SetTranslate(translation);
                ops[0].Set(current, get_edition_time_code(viewport));
            }
        }
    }

    return this;
};

void RotationManipulator::on_end_edition(Viewport &) { end_edition(); }

// TODO code should be shared with position manipulator
UsdTimeCode RotationManipulator::get_edition_time_code(const Viewport &viewport) {
    std::vector<double> timeSamples;// TODO: is there a faster way to know it the xformable has timesamples ?
    const auto xformable = UsdGeomXformable(_xformAPI.GetPrim());
    xformable.GetTimeSamples(&timeSamples);
    if (timeSamples.empty()) {
        return UsdTimeCode::Default();
    } else {
        return get_viewport_time_code(viewport);
    }
}

UsdTimeCode RotationManipulator::get_viewport_time_code(const Viewport &viewport) { return viewport.get_current_time_code(); }

}// namespace vox