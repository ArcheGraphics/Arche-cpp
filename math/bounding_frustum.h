//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <cassert>
#include <cmath>
#include <optional>

#include "math/bounding_box3.h"
#include "math/bounding_plane3.h"

namespace vox {

struct BoundingFrustum {
    /** The near plane of this frustum. */
    BoundingPlane3F near;
    /** The far plane of this frustum. */
    BoundingPlane3F far;
    /** The left plane of this frustum. */
    BoundingPlane3F left;
    /** The right plane of this frustum. */
    BoundingPlane3F right;
    /** The top plane of this frustum. */
    BoundingPlane3F top;
    /** The bottom plane of this frustum. */
    BoundingPlane3F bottom;

    BoundingFrustum();

    /**
     * Constructor of BoundingFrustum.
     * @param matrix - The view-projection matrix
     */
    explicit BoundingFrustum(const Matrix4x4F &matrix);

    //! Copy constructor.
    BoundingFrustum(const BoundingFrustum &other) = default;

    /**
     * Get the plane by the given index.
     * 0: near
     * 1: far
     * 2: left
     * 3: right
     * 4: top
     * 5: bottom
     * @param index - The index
     * @returns The plane get
     */
    [[nodiscard]] BoundingPlane3F getPlane(int index) const;

    /**
     * Update all planes from the given matrix.
     * @param matrix - The given view-projection matrix
     */
    void calculateFromMatrix(const Matrix4x4F &matrix);

    /**
     * Get whether or not a specified bounding box intersects with this frustum (Contains or Intersects).
     * @param box - The box for testing
     * @returns True if bounding box intersects with this frustum, false otherwise
     */
    [[nodiscard]] bool intersectsBox(const BoundingBox3F &box) const;
};

}  // namespace vox
