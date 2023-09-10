//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <pxr/pxr.h>
#include <pxr/usd/usd/prim.h>
#include <pxr/usd/usdGeom/xform.h>

#include <utility>

namespace vox {
enum class UpAxis {
    X,
    Y,
    Z,
};

class UsdRenderer {
public:
    explicit UsdRenderer(pxr::UsdStageRefPtr stage, UpAxis up_axis = UpAxis::Y, float fps = 60, float scaling = 1.0)
        : _stage(std::move(stage)),
          _fps(fps) {
        initialize(scaling, up_axis);
    }

    explicit UsdRenderer(const std::string &name, UpAxis up_axis = UpAxis::Y, float fps = 60, float scaling = 1.0)
        : _fps(fps) {
        _stage = pxr::UsdStage::CreateNew(name);
        initialize(scaling, up_axis);
    }

    void initialize(float scaling, UpAxis up_axis);

    void begin_frame(float time);

    void end_frame();

    void register_body(const pxr::TfToken &body_name);

    pxr::SdfPath _resolve_path(const pxr::TfToken &name,
                               std::optional<pxr::TfToken> parent_body = std::nullopt,
                               bool is_template = false);

    // Render a plane with the given dimensions.
    pxr::SdfPath render_plane(const pxr::TfToken &name, pxr::GfVec3f pos, pxr::GfQuatf rot, float width, float length,
                              const std::optional<pxr::TfToken> &parent_body = std::nullopt, bool is_template = false);

    void render_ground(float size = 100.0);

    /// Debug helper to add a sphere for visualization
    pxr::SdfPath render_sphere(const pxr::TfToken &name, pxr::GfVec3f pos, pxr::GfQuatf rot, float radius,
                               const std::optional<pxr::TfToken> &parent_body = std::nullopt, bool is_template = false);

    /// Debug helper to add a capsule for visualization
    pxr::SdfPath render_capsule(const pxr::TfToken &name, pxr::GfVec3f pos, pxr::GfQuatf rot, float radius, float half_height,
                                const std::optional<pxr::TfToken> &parent_body = std::nullopt, bool is_template = false);

    // Debug helper to add a cylinder for visualization
    pxr::SdfPath render_cylinder(const pxr::TfToken &name, pxr::GfVec3f pos, pxr::GfQuatf rot, float radius, float half_height,
                                 const std::optional<pxr::TfToken> &parent_body = std::nullopt, bool is_template = false);

    // Debug helper to add a cone for visualization
    pxr::SdfPath render_cone(const pxr::TfToken &name, pxr::GfVec3f pos, pxr::GfQuatf rot, float radius, float half_height,
                             const std::optional<pxr::TfToken> &parent_body = std::nullopt, bool is_template = false);

    // Debug helper to add a box for visualization
    pxr::SdfPath render_box(const pxr::TfToken &name, pxr::GfVec3f pos, pxr::GfQuatf rot, pxr::GfVec3f extents,
                            const std::optional<pxr::TfToken> &parent_body = std::nullopt, bool is_template = false);

    void render_ref(const std::string &name, const pxr::TfToken &path,
                    pxr::GfVec3f pos, pxr::GfQuatf rot, pxr::GfVec3f scale);

    pxr::SdfPath render_mesh(const pxr::TfToken &name, const pxr::VtVec3fArray &points, const pxr::VtIntArray &indices,
                             const std::optional<pxr::VtVec3fArray> &color = std::nullopt,
                             pxr::GfVec3f pos = {0, 0, 0}, pxr::GfQuatf rot = {0, 0, 0, 1},
                             pxr::GfVec3f scale = {1, 1, 1}, bool update_topology = false,
                             const std::optional<pxr::TfToken> &parent_body = std::nullopt, bool is_template = false);

    // Debug helper to add a line list as a set of capsules
    void render_line_list(const pxr::TfToken &name, const pxr::VtVec3fArray &vertices,
                          const pxr::VtIntArray &indices, pxr::GfVec3f color, float radius);

    void render_line_strip(const pxr::TfToken &name, const pxr::VtVec3fArray &vertices,
                           pxr::GfVec3f color, float radius);

    void render_point(const pxr::TfToken &name, const pxr::VtVec3fArray &points,
                      float radius, std::optional<pxr::GfVec3f> color = std::nullopt);

private:
    pxr::UsdStageRefPtr _stage;
    pxr::UsdGeomXform _root;

    float _fps;
    UpAxis _up_axis;
    float _time{0};
};

}// namespace vox