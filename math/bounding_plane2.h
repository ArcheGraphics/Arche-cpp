//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "math/bounding_plane.h"
#include "math/vector2.h"

namespace vox {

//!
//! \brief      Class for 2-D plane.
//!
//! \tparam     T     The value type.
//!
template <typename T>
struct BoundingPlane<T, 2> final {
public:
    static_assert(std::is_floating_point<T>::value, "Plane only can be instantiated with floating point types");

    //! The normal of the plane.
    Vector2<T> normal;

    //! The distance of the plane along its normal to the origin.
    T distance;

    //! Constructs an empty plane that points (1, 0) from (0, 0).
    BoundingPlane();

    //! Constructs a plane with given origin and direction.
    BoundingPlane(const Vector2<T> &newNormal, const T &newDistance);

    /**
     * Calculate the plane that contains the three specified points.
     * @param point0 - The first point
     * @param point1 - The second point
     * @param point2 - The third point
     */
    BoundingPlane(const Vector2<T> &point0, const Vector2<T> &point1);

    //! Copy constructor.
    BoundingPlane(const BoundingPlane &other);

    /**
     * Normalize the normal vector of this plane.
     * @returns The plane after normalize
     */
    BoundingPlane<T, 2> normalized() const;

    /**
     * Normalize the normal vector of this plane.
     */
    void normalize();
};

//! Type alias for 2-D plane.
template <typename T>
using BoundingPlane2 = BoundingPlane<T, 2>;

//! Float-type 2-D plane.
using BoundingPlane2F = BoundingPlane2<float>;

//! Double-type 2-D plane.
using BoundingPlane2D = BoundingPlane2<double>;

}  // namespace vox

#include "math/bounding_plane2-inl.h"
