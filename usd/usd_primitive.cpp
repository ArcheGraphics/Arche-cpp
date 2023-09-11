//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "usd_primitive.h"

#include <pxr/base/gf/rotation.h>
#include <pxr/usd/usdGeom/metrics.h>
#include <pxr/usd/usdGeom/mesh.h>
#include <pxr/usd/usdGeom/scope.h>
#include <pxr/usd/usdGeom/sphere.h>
#include <pxr/usd/usdGeom/capsule.h>
#include <pxr/usd/usdGeom/cylinder.h>
#include <pxr/usd/usdGeom/cone.h>
#include <pxr/usd/usdGeom/cube.h>
#include <pxr/usd/usdGeom/pointInstancer.h>
#include <pxr/usd/usdGeom/points.h>
#include <pxr/usd/usdLux/distantLight.h>

namespace vox {
namespace {
void _usd_add_xform(const pxr::UsdSchemaBase &schemaBase) {
    auto prim = pxr::UsdGeomXform(schemaBase);
    prim.ClearXformOpOrder();
    auto t = prim.AddTransformOp();
    auto r = prim.AddOrientOp();
    auto s = prim.AddScaleOp();
}

void _usd_set_xform(const pxr::UsdSchemaBase &schemaBase, pxr::GfVec3f pos, pxr::GfQuatf rot, pxr::GfVec3f scale, float time) {
    auto xform = pxr::UsdGeomXform(schemaBase);

    bool resetsXformStack;
    std::vector<pxr::UsdGeomXformOp> xform_ops = xform.GetOrderedXformOps(&resetsXformStack);
    xform_ops[0].Set(pos, time);
    xform_ops[1].Set(rot, time);
    xform_ops[2].Set(scale, time);
    xform.SetXformOpOrder(xform_ops);
}

// transforms a cylinder such that it connects the two points pos0, pos1
std::tuple<pxr::GfVec3f, pxr::GfQuath, pxr::GfVec3f> _compute_segment_xform(pxr::GfVec3f pos0, pxr::GfVec3f pos1) {
    auto mid = (pos0 + pos1) * 0.5;
    auto height = (pos1 - pos0).GetLength();
    auto dir = (pos1 - pos0) / height;

    pxr::GfRotation rot{pxr::GfVec3d(0.0, 0.0, 1.0), pxr::GfVec3d(dir)};
    auto scale = pxr::GfVec3f(1.0, 1.0, height);
    return {mid, pxr::GfQuath(rot.GetQuat()), scale};
}
}// namespace

void UsdRenderer::initialize(float scaling, UpAxis up_axis) {
    _root = pxr::UsdGeomXform::Define(_stage, pxr::SdfPath("/root"));

    // apply scaling
    _root.ClearXformOpOrder();
    auto s = _root.AddScaleOp();
    s.Set(pxr::GfVec3f(scaling, scaling, scaling), 0);

    _stage->SetDefaultPrim(_root.GetPrim());
    _stage->SetStartTimeCode(0);
    _stage->SetEndTimeCode(0);
    _stage->SetTimeCodesPerSecond(_fps);
    switch (up_axis) {
        case UpAxis::X:
            pxr::UsdGeomSetStageUpAxis(_stage, pxr::UsdGeomTokens->x);
            break;
        case UpAxis::Y:
            pxr::UsdGeomSetStageUpAxis(_stage, pxr::UsdGeomTokens->y);
            break;
        case UpAxis::Z:
            pxr::UsdGeomSetStageUpAxis(_stage, pxr::UsdGeomTokens->z);
            break;
    }

    // add default lights
    auto light_0 = pxr::UsdLuxDistantLight::Define(_stage, pxr::SdfPath{"/Light_0"});
    light_0.GetPrim().CreateAttribute({"intensity"}, pxr::SdfValueTypeNames->Float,
                                      false)
        .Set(2500.0);
    light_0.GetPrim().CreateAttribute({"color"}, pxr::SdfValueTypeNames->Color3f,
                                      false)
        .Set(pxr::GfVec3f(0.98, 0.85, 0.7));
    pxr::UsdGeomXform(light_0.GetPrim()).AddRotateXOp().Set(70.0);
    pxr::UsdGeomXform(light_0.GetPrim()).AddRotateXOp().Set(-45.0);

    auto light_1 = pxr::UsdLuxDistantLight::Define(_stage, pxr::SdfPath{"/Light_1"});
    light_1.GetPrim().CreateAttribute({"intensity"}, pxr::SdfValueTypeNames->Float, false).Set(2500.0);
    light_1.GetPrim().CreateAttribute({"color"}, pxr::SdfValueTypeNames->Color3f, false).Set(pxr::GfVec3f(0.62, 0.82, 0.98));

    pxr::UsdGeomXform(light_1.GetPrim()).AddRotateYOp().Set(-70.0);
    pxr::UsdGeomXform(light_1.GetPrim()).AddRotateXOp().Set(-45.0);
}

void UsdRenderer::begin_frame(float time) {
    _stage->SetEndTimeCode(time * _fps);
    _time = time * _fps;
}

void UsdRenderer::end_frame() {}

void UsdRenderer::register_body(const pxr::TfToken &body_name) {
    auto xform = pxr::UsdGeomXform::Define(_stage, _root.GetPath().AppendChild(body_name));
    _usd_add_xform(xform);
}

pxr::SdfPath UsdRenderer::_resolve_path(const pxr::TfToken &name,
                                        std::optional<pxr::TfToken> parent_body,
                                        bool is_template) {
    if (is_template) {
        return _root.GetPath().AppendChild(pxr::TfToken{"_template_shapes"}).AppendChild(name);
    }
    if (!parent_body.has_value()) {
        return _root.GetPath().AppendChild(name);
    } else {
        return _root.GetPath().AppendChild(parent_body.value()).AppendChild(name);
    }
}

// Render a plane with the given dimensions.
pxr::SdfPath UsdRenderer::render_plane(const pxr::TfToken &name, pxr::GfVec3f pos, pxr::GfQuatf rot, float width, float length,
                                       const std::optional<pxr::TfToken> &parent_body, bool is_template) {
    pxr::SdfPath prim_path;
    pxr::SdfPath plane_path;
    if (is_template) {
        prim_path = _resolve_path(name, parent_body, is_template);
        auto blueprint = pxr::UsdGeomScope::Define(_stage, prim_path);
        auto blueprint_prim = blueprint.GetPrim();
        blueprint_prim.SetInstanceable(true);
        blueprint_prim.SetSpecifier(pxr::SdfSpecifierClass);
        plane_path = prim_path.AppendChild(pxr::TfToken{"plane"});
    } else {
        plane_path = _resolve_path(name, parent_body);
        prim_path = plane_path;
    }

    auto plane = pxr::UsdGeomMesh::Get(_stage, plane_path);
    // todo
    {
        plane = pxr::UsdGeomMesh::Define(_stage, plane_path);
        plane.CreateDoubleSidedAttr().Set(true);
        width = width > 0 ? width : 100.f;
        length = length > 0 ? length : 100.f;
        pxr::VtArray points = {pxr::GfVec3f(-width, 0.0, -length), pxr::GfVec3f(width, 0.0, -length),
                               pxr::GfVec3f(width, 0.0, length), pxr::GfVec3f(-width, 0.0, length)};
        pxr::VtArray normals = {pxr::GfVec3f(0.0, 1.0, 0.0), pxr::GfVec3f(0.0, 1.0, 0.0),
                                pxr::GfVec3f(0.0, 1.0, 0.0), pxr::GfVec3f(0.0, 1.0, 0.0)};
        pxr::VtArray counts = {4};
        pxr::VtArray indices = {0, 1, 2, 3};
        plane.GetPointsAttr().Set(points);
        plane.GetNormalsAttr().Set(normals);
        plane.GetFaceVertexCountsAttr().Set(counts);
        plane.GetFaceVertexIndicesAttr().Set(indices);
        _usd_add_xform(plane);
    }

    if (!is_template) {
        _usd_set_xform(plane, pos, rot, pxr::GfVec3f{1, 1, 1}, 0);
    }

    return prim_path;
}

void UsdRenderer::render_ground(float size) {
    auto mesh = pxr::UsdGeomMesh::Define(_stage, _root.GetPath().AppendChild(pxr::TfToken{"ground"}));
    mesh.CreateDoubleSidedAttr().Set(true);

    pxr::VtVec3fArray points;
    pxr::VtVec3fArray normals;
    if (_up_axis == UpAxis::X) {
        points = {pxr::GfVec3f(0.0, -size, -size), pxr::GfVec3f(0.0, size, -size),
                  pxr::GfVec3f(0.0, size, size), pxr::GfVec3f(0.0, -size, size)};
        normals = {pxr::GfVec3f(1.0, 0.0, 0.0), pxr::GfVec3f(1.0, 0.0, 0.0),
                   pxr::GfVec3f(1.0, 0.0, 0.0), pxr::GfVec3f(1.0, 0.0, 0.0)};
    } else if (_up_axis == UpAxis::Y) {
        points = {pxr::GfVec3f(-size, 0.0, -size), pxr::GfVec3f(size, 0.0, -size),
                  pxr::GfVec3f(size, 0.0, size), pxr::GfVec3f(-size, 0.0, size)};
        normals = {pxr::GfVec3f(0.0, 1.0, 0.0), pxr::GfVec3f(0.0, 1.0, 0.0),
                   pxr::GfVec3f(0.0, 1.0, 0.0), pxr::GfVec3f(0.0, 1.0, 0.0)};
    } else {
        points = {pxr::GfVec3f(-size, -size, 0.0), pxr::GfVec3f(size, -size, 0.0),
                  pxr::GfVec3f(size, size, 0.0), pxr::GfVec3f(-size, size, 0.0)};
        normals = {pxr::GfVec3f(0.0, 0.0, 1.0), pxr::GfVec3f(0.0, 0.0, 1.0),
                   pxr::GfVec3f(0.0, 0.0, 1.0), pxr::GfVec3f(0.0, 0.0, 1.0)};
    }

    pxr::VtIntArray counts = {4};
    pxr::VtIntArray indices = {0, 1, 2, 3};

    mesh.GetPointsAttr().Set(points);
    mesh.GetNormalsAttr().Set(normals);
    mesh.GetFaceVertexCountsAttr().Set(counts);
    mesh.GetFaceVertexIndicesAttr().Set(indices);
}

/// Debug helper to add a sphere for visualization
pxr::SdfPath UsdRenderer::render_sphere(const pxr::TfToken &name, pxr::GfVec3f pos, pxr::GfQuatf rot, float radius,
                                        const std::optional<pxr::TfToken> &parent_body, bool is_template) {
    pxr::SdfPath prim_path;
    pxr::SdfPath sphere_path;
    if (is_template) {
        prim_path = _resolve_path(name, parent_body, is_template);
        auto blueprint = pxr::UsdGeomScope::Define(_stage, prim_path);
        auto blueprint_prim = blueprint.GetPrim();
        blueprint_prim.SetInstanceable(true);
        blueprint_prim.SetSpecifier(pxr::SdfSpecifierClass);
        sphere_path = prim_path.AppendChild(pxr::TfToken{"sphere"});
    } else {
        sphere_path = _resolve_path(name, parent_body);
        prim_path = sphere_path;
    }

    auto sphere = pxr::UsdGeomSphere::Get(_stage, sphere_path);
    // todo
    {
        sphere = pxr::UsdGeomSphere::Define(_stage, sphere_path);
        _usd_add_xform(sphere);
    }

    sphere.GetRadiusAttr().Set(radius, _time);

    if (!is_template) {
        _usd_set_xform(sphere, pos, rot, pxr::GfVec3f{1.f, 1.f, 1.f}, 0);
    }
    return prim_path;
}

/// Debug helper to add a capsule for visualization
pxr::SdfPath UsdRenderer::render_capsule(const pxr::TfToken &name, pxr::GfVec3f pos, pxr::GfQuatf rot, float radius, float half_height,
                                         const std::optional<pxr::TfToken> &parent_body, bool is_template) {
    pxr::SdfPath prim_path;
    pxr::SdfPath capsule_path;
    if (is_template) {
        prim_path = _resolve_path(name, parent_body, is_template);
        auto blueprint = pxr::UsdGeomScope::Define(_stage, prim_path);
        auto blueprint_prim = blueprint.GetPrim();
        blueprint_prim.SetInstanceable(true);
        blueprint_prim.SetSpecifier(pxr::SdfSpecifierClass);
        capsule_path = prim_path.AppendChild(pxr::TfToken{"capsule"});
    } else {
        capsule_path = _resolve_path(name, parent_body);
        prim_path = capsule_path;
    }

    auto capsule = pxr::UsdGeomCapsule::Get(_stage, capsule_path);
    // todo
    {
        capsule = pxr::UsdGeomCapsule::Define(_stage, capsule_path);
        _usd_add_xform(capsule);
    }

    capsule.GetRadiusAttr().Set(radius);
    capsule.GetHeightAttr().Set(half_height * 2.f);
    capsule.GetAxisAttr().Set("Y");

    if (!is_template) {
        _usd_set_xform(capsule, pos, rot, pxr::GfVec3f{1.f, 1.f, 1.f}, 0.f);
    }
    return prim_path;
}

// Debug helper to add a cylinder for visualization
pxr::SdfPath UsdRenderer::render_cylinder(const pxr::TfToken &name, pxr::GfVec3f pos, pxr::GfQuatf rot, float radius, float half_height,
                                          const std::optional<pxr::TfToken> &parent_body, bool is_template) {
    pxr::SdfPath prim_path;
    pxr::SdfPath cylinder_path;
    if (is_template) {
        prim_path = _resolve_path(name, parent_body, is_template);
        auto blueprint = pxr::UsdGeomScope::Define(_stage, prim_path);
        auto blueprint_prim = blueprint.GetPrim();
        blueprint_prim.SetInstanceable(true);
        blueprint_prim.SetSpecifier(pxr::SdfSpecifierClass);
        cylinder_path = prim_path.AppendChild(pxr::TfToken{"cylinder"});
    } else {
        cylinder_path = _resolve_path(name, parent_body);
        prim_path = cylinder_path;
    }

    auto cylinder = pxr::UsdGeomCylinder::Get(_stage, cylinder_path);
    // todo
    {
        cylinder = pxr::UsdGeomCylinder::Define(_stage, cylinder_path);
        _usd_add_xform(cylinder);
    }

    cylinder.GetRadiusAttr().Set(radius);
    cylinder.GetHeightAttr().Set(half_height * 2.f);
    cylinder.GetAxisAttr().Set("Y");

    if (!is_template) {
        _usd_set_xform(cylinder, pos, rot, pxr::GfVec3f{1.f, 1.f, 1.f}, 0.f);
    }
    return prim_path;
}

// Debug helper to add a cone for visualization
pxr::SdfPath UsdRenderer::render_cone(const pxr::TfToken &name, pxr::GfVec3f pos, pxr::GfQuatf rot, float radius, float half_height,
                                      const std::optional<pxr::TfToken> &parent_body, bool is_template) {
    pxr::SdfPath prim_path;
    pxr::SdfPath cone_path;
    if (is_template) {
        prim_path = _resolve_path(name, parent_body, is_template);
        auto blueprint = pxr::UsdGeomScope::Define(_stage, prim_path);
        auto blueprint_prim = blueprint.GetPrim();
        blueprint_prim.SetInstanceable(true);
        blueprint_prim.SetSpecifier(pxr::SdfSpecifierClass);
        cone_path = prim_path.AppendChild(pxr::TfToken{"cone"});
    } else {
        cone_path = _resolve_path(name, parent_body);
        prim_path = cone_path;
    }

    auto cone = pxr::UsdGeomCone::Get(_stage, cone_path);
    // todo
    {
        cone = pxr::UsdGeomCone::Define(_stage, cone_path);
        _usd_add_xform(cone);
    }

    cone.GetRadiusAttr().Set(radius);
    cone.GetHeightAttr().Set(half_height * 2.f);
    cone.GetAxisAttr().Set("Y");

    if (!is_template) {
        _usd_set_xform(cone, pos, rot, pxr::GfVec3f{1.f, 1.f, 1.f}, 0.f);
    }
    return prim_path;
}

// Debug helper to add a box for visualization
pxr::SdfPath UsdRenderer::render_box(const pxr::TfToken &name, pxr::GfVec3f pos, pxr::GfQuatf rot, pxr::GfVec3f extents,
                                     const std::optional<pxr::TfToken> &parent_body, bool is_template) {
    pxr::SdfPath prim_path;
    pxr::SdfPath cube_path;
    if (is_template) {
        prim_path = _resolve_path(name, parent_body, is_template);
        auto blueprint = pxr::UsdGeomScope::Define(_stage, prim_path);
        auto blueprint_prim = blueprint.GetPrim();
        blueprint_prim.SetInstanceable(true);
        blueprint_prim.SetSpecifier(pxr::SdfSpecifierClass);
        cube_path = prim_path.AppendChild(pxr::TfToken{"cube"});
    } else {
        cube_path = _resolve_path(name, parent_body);
        prim_path = cube_path;
    }

    auto cube = pxr::UsdGeomCube::Get(_stage, cube_path);
    // todo
    {
        cube = pxr::UsdGeomCube::Define(_stage, cube_path);
        _usd_add_xform(cube);
    }

    if (!is_template) {
        _usd_set_xform(cube, pos, rot, extents, 0.f);
    }
    return prim_path;
}

void UsdRenderer::render_ref(const std::string &name, const pxr::TfToken &path,
                             pxr::GfVec3f pos, pxr::GfQuatf rot, pxr::GfVec3f scale) {
    auto ref_path = pxr::SdfPath("/root/" + name);
    auto ref = pxr::UsdGeomXform::Get(_stage, ref_path);
    // todo
    {
        ref = pxr::UsdGeomXform::Define(_stage, ref_path);
        ref.GetPrim().GetReferences().AddReference(path);
        _usd_add_xform(ref);
    }

    // update transform
    _usd_set_xform(ref, pos, rot, scale, _time);
}

pxr::SdfPath UsdRenderer::render_mesh(const pxr::TfToken &name, const pxr::VtVec3fArray &points, const pxr::VtIntArray &indices,
                                      const std::optional<pxr::VtVec3fArray> &color,
                                      pxr::GfVec3f pos, pxr::GfQuatf rot,
                                      pxr::GfVec3f scale, bool update_topology,
                                      const std::optional<pxr::TfToken> &parent_body, bool is_template) {
    pxr::SdfPath prim_path;
    pxr::SdfPath mesh_path;
    if (is_template) {
        prim_path = _resolve_path(name, parent_body, is_template);
        auto blueprint = pxr::UsdGeomScope::Define(_stage, prim_path);
        auto blueprint_prim = blueprint.GetPrim();
        blueprint_prim.SetInstanceable(true);
        blueprint_prim.SetSpecifier(pxr::SdfSpecifierClass);
        mesh_path = prim_path.AppendChild(pxr::TfToken{"mesh"});
    } else {
        mesh_path = _resolve_path(name, parent_body);
        prim_path = mesh_path;
    }

    auto mesh = pxr::UsdGeomMesh::Get(_stage, mesh_path);
    // todo
    {
        mesh = pxr::UsdGeomMesh::Define(_stage, mesh_path);
        pxr::UsdGeomPrimvar(mesh.GetDisplayColorAttr()).SetInterpolation(pxr::TfToken("vertex"));
        _usd_add_xform(mesh);

        // force topology update on first time
        update_topology = true;
    }

    mesh.GetPointsAttr().Set(points, _time);

    if (update_topology) {
        // todo
        //            idxs = np.array(indices).reshape(-1, 3)
        //            mesh.GetFaceVertexIndicesAttr().Set(idxs, self.time)
        //            mesh.GetFaceVertexCountsAttr().Set([3] * len(idxs), self.time)
    }

    if (color.has_value()) {
        mesh.GetDisplayColorAttr().Set(color.value(), _time);
    }

    if (!is_template) {
        _usd_set_xform(mesh, pos, rot, scale, _time);
    }
    return prim_path;
}

// Debug helper to add a line list as a set of capsules
void UsdRenderer::render_line_list(const pxr::TfToken &name, const pxr::VtVec3fArray &vertices,
                                   const pxr::VtIntArray &indices, pxr::GfVec3f color, float radius) {
    auto num_lines = int(indices.size() / 2);
    if (num_lines < 1) {
        return;
    }

    // look up rope point instancer
    auto instancer_path = _root.GetPath().AppendChild(name);
    auto instancer = pxr::UsdGeomPointInstancer::Get(_stage, instancer_path);
    // todo
    {
        instancer = pxr::UsdGeomPointInstancer::Define(_stage, instancer_path);
        auto instancer_capsule = pxr::UsdGeomCapsule::Define(_stage,
                                                             instancer.GetPath().AppendChild(pxr::TfToken("capsule")));
        instancer_capsule.GetRadiusAttr().Set(radius);
        instancer.CreatePrototypesRel().SetTargets({instancer_capsule.GetPath()});
    }

    pxr::VtVec3fArray line_positions;
    pxr::VtQuatfArray line_rotations;
    pxr::VtVec3fArray line_scales;
    for (int i = 0; i < num_lines; ++i) {
        auto pos0 = vertices[indices[i * 2 + 0]];
        auto pos1 = vertices[indices[i * 2 + 1]];
        auto [pos, rot, scale] = _compute_segment_xform(
            pxr::GfVec3f(float(pos0[0]), float(pos0[1]), float(pos0[2])),
            pxr::GfVec3f(float(pos1[0]), float(pos1[1]), float(pos1[2])));

        line_positions.push_back(pos);
        line_rotations.push_back(rot);
        line_scales.push_back(scale);
    }

    instancer.GetPositionsAttr().Set(line_positions, _time);
    instancer.GetOrientationsAttr().Set(line_rotations, _time);
    instancer.GetScalesAttr().Set(line_scales, _time);
    // todo
    //                instancer.GetProtoIndicesAttr().Set([0] * num_lines, _time);
}

void UsdRenderer::render_line_strip(const pxr::TfToken &name, const pxr::VtVec3fArray &vertices,
                                    pxr::GfVec3f color, float radius) {
    auto num_lines = int(vertices.size() - 1);
    if (num_lines < 1) {
        return;
    }

    auto instancer_path = _root.GetPath().AppendChild(name);
    auto instancer = pxr::UsdGeomPointInstancer::Get(_stage, instancer_path);
    // todo
    {
        instancer = pxr::UsdGeomPointInstancer::Define(_stage, instancer_path);
        auto instancer_capsule = pxr::UsdGeomCapsule::Define(_stage, instancer.GetPath().AppendChild(pxr::TfToken("capsule")));
        instancer_capsule.GetRadiusAttr().Set(radius);
        instancer.CreatePrototypesRel().SetTargets({instancer_capsule.GetPath()});
    }

    pxr::VtVec3fArray line_positions;
    pxr::VtQuatfArray line_rotations;
    pxr::VtVec3fArray line_scales;
    for (int i = 0; i < num_lines; ++i) {
        auto pos0 = vertices[i];
        auto pos1 = vertices[i + 1];
        auto [pos, rot, scale] = _compute_segment_xform(
            pxr::GfVec3f(float(pos0[0]), float(pos0[1]), float(pos0[2])),
            pxr::GfVec3f(float(pos1[0]), float(pos1[1]), float(pos1[2])));

        line_positions.push_back(pos);
        line_rotations.push_back(rot);
        line_scales.push_back(scale);
    }

    instancer.GetPositionsAttr().Set(line_positions, _time);
    instancer.GetOrientationsAttr().Set(line_rotations, _time);
    instancer.GetScalesAttr().Set(line_scales, _time);
    // todo
    //        instancer.GetProtoIndicesAttr().Set([0] * num_lines, _time);

    auto instancer_capsule = pxr::UsdGeomCapsule::Get(_stage, instancer.GetPath().AppendChild(pxr::TfToken{"capsule"}));
    instancer_capsule.GetDisplayColorAttr().Set(pxr::VtVec3fArray{pxr::GfVec3f(color)}, _time);
}

void UsdRenderer::render_point(const pxr::TfToken &name, const pxr::VtVec3fArray &points,
                               float radius, std::optional<pxr::GfVec3f> color) {
    auto instancer_path = _root.GetPath().AppendChild(name);
    auto instancer = pxr::UsdGeomPointInstancer::Get(_stage, instancer_path);
    // todo
    {
        if (!color.has_value()) {
            instancer = pxr::UsdGeomPointInstancer::Define(_stage, instancer_path);
            auto instancer_sphere = pxr::UsdGeomSphere::Define(_stage, instancer.GetPath().AppendChild(pxr::TfToken{"sphere"}));

            instancer_sphere.GetRadiusAttr().Set(radius);

            instancer.CreatePrototypesRel().SetTargets({instancer_sphere.GetPath()});
            // todo
            //                instancer.CreateProtoIndicesAttr().Set();

            //                auto quat
            //                    instancer.GetOrientationsAttr().Set(quats, _time);
        } else {
            auto instancer = pxr::UsdGeomPoints::Define(_stage, instancer_path);
            instancer.GetWidthsAttr().Set(pxr::VtFloatArray(points.size(), radius));
        }
    }

    //        if colors is None:
    //            instancer.GetPositionsAttr().Set(points, self.time)
    //        else:
    //            instancer.GetPointsAttr().Set(points, self.time)
    //            instancer.GetDisplayColorAttr().Set(colors, self.time)
}

}// namespace vox