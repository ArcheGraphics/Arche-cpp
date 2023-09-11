//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <cmath>
#include <pxr/base/gf/vec2d.h>
#include <pxr/base/gf/matrix4d.h>

PXR_NAMESPACE_USING_DIRECTIVE

namespace vox {
/// Check if all components of v1 and v2 have the same signs
inline bool same_sign(const GfVec2d &v1, const GfVec2d &v2) { return v1[0] * v2[0] >= 0.0 && v1[1] * v2[1] >= 0.0; }

/// Given the segment defined by 2 points p1 and p2, check if point is within boundaries from the segment
inline bool intersects_segment(const GfVec2d &p1, const GfVec2d &p2, const GfVec2d &point, const GfVec2d &limits) {
    // Line vector
    const GfVec2d segment(p1 - p2);

    // Point clicked as a 2d point
    const auto projectionOnSegment = (point - p2).GetProjection(segment.GetNormalized());
    if (segment.GetLength()) {
        const auto ratio = projectionOnSegment.GetLength() / segment.GetLength();
        if (ratio > 0.0 && ratio <= 1.0 && same_sign(projectionOnSegment, segment)) {
            GfVec2d dist(projectionOnSegment - point + p2);
            if (fabs(dist[0]) < limits[0] && fabs(dist[1]) < limits[1]) {
                return true;
            }
        }
    }

    return false;
}

/// Given a modelview and a projection matrix, project point to the normalized screen space
inline GfVec2d project_to_normalized_screen(const GfMatrix4d &mv, const GfMatrix4d &proj, const GfVec3d &point) {
    auto projected = proj.Transform(mv.Transform(point));
    return GfVec2d(projected[0], projected[1]);
}

inline GfVec2d project_to_texture_screen_space(const GfMatrix4d &mv, const GfMatrix4d &proj, const GfVec2d &texSize, const GfVec3d &point) {
    auto projected = proj.Transform(mv.Transform(point));
    projected[0] = (projected[0] * 0.5 * texSize[0]) + texSize[0] / 2;
    projected[1] = -(projected[1] * 0.5 * texSize[1]) + texSize[1] / 2;
    return GfVec2d(projected[0], projected[1]);
}

}// namespace vox