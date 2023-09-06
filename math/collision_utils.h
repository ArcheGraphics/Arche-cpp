//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <array>
#include <cassert>
#include <cmath>

#include "math/bounding_box3.h"
#include "math/bounding_frustum.h"
#include "math/bounding_plane3.h"
#include "math/ray3.h"

namespace vox {
/**
 * Defines the intersection between a plane and a bounding volume.
 */
enum class PlaneIntersectionType {
    /** There is no intersection, the bounding volume is in the back of the plane. */
    Back,
    /** There is no intersection, the bounding volume is in the front of the plane. */
    Front,
    /** The plane is intersected. */
    Intersecting
};

/**
 * Defines how the bounding volumes intersects or contain one another.
 */
enum class ContainmentType {
    /** Indicates that there is no overlap between two bounding volumes. */
    Disjoint,
    /** Indicates that one bounding volume completely contains another volume. */
    Contains,
    /** Indicates that bounding volumes partially overlap one another. */
    Intersects
};

/**
 * Calculate the distance from a point to a plane.
 * @param plane - The plane
 * @param point - The point
 * @returns The distance from a point to a plane
 */
float distancePlaneAndPoint(const BoundingPlane3F &plane, const Point3F &point);

/**
 * Get the intersection type between a plane and a point.
 * @param plane - The plane
 * @param point - The point
 * @returns The intersection type
 */
PlaneIntersectionType intersectsPlaneAndPoint(const BoundingPlane3F &plane, const Point3F &point);

/**
 * Get the intersection type between a plane and a box (AABB).
 * @param plane - The plane
 * @param box - The box
 * @returns The intersection type
 */
PlaneIntersectionType intersectsPlaneAndBox(const BoundingPlane3F &plane, const BoundingBox3F &box);

/**
 * Get the intersection type between a ray and a plane.
 * @param ray - The ray
 * @param plane - The plane
 * @returns The distance from ray to plane if intersecting, -1 otherwise
 */
float intersectsRayAndPlane(const Ray3F &ray, const BoundingPlane3F &plane);

/**
 * Get the intersection type between a ray and a box (AABB).
 * @param ray - The ray
 * @param box - The box
 * @returns The distance from ray to box if intersecting, -1 otherwise
 */
float intersectsRayAndBox(const Ray3F &ray, const BoundingBox3F &box);

/**
 * Check whether the boxes intersect.
 * @param box_a - The first box to check
 * @param box_b - The second box to check
 * @returns True if the boxes intersect, false otherwise
 */
bool intersectsBoxAndBox(const BoundingBox3F &box_a, const BoundingBox3F &box_b);

/**
 * Get whether or not a specified bounding box intersects with this frustum (Contains or Intersects).
 * @param frustum - The frustum
 * @param box - The box
 * @returns True if bounding box intersects with this frustum, false otherwise
 */
bool intersectsFrustumAndBox(const BoundingFrustum &frustum, const BoundingBox3F &box);

/**
 * Get the containment type between a frustum and a box (AABB).
 * @param frustum - The frustum
 * @param box - The box
 * @returns The containment type
 */
ContainmentType frustumContainsBox(const BoundingFrustum &frustum, const BoundingBox3F &box);

}  // namespace vox
