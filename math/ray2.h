//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "math/point2.h"
#include "math/ray.h"
#include "math/vector2.h"

namespace vox {

//!
//! \brief      Class for 2-D ray.
//!
//! \tparam     T     The value type.
//!
template <typename T>
struct Ray<T, 2> final {
public:
    static_assert(std::is_floating_point<T>::value, "Ray only can be instantiated with floating point types");

    //! The origin of the ray.
    Point2<T> origin;

    //! The direction of the ray.
    Vector2<T> direction;

    //! Constructs an empty ray that points (1, 0) from (0, 0).
    Ray();

    //! Constructs a ray with given origin and direction.
    Ray(const Point2<T> &new_origin, const Vector2<T> &new_direction);

    //! Copy constructor.
    Ray(const Ray &other);

    //! Returns a point on the ray at distance \p t.
    Point2<T> pointAt(T t) const;
};

//! Type alias for 2-D ray.
template <typename T>
using Ray2 = Ray<T, 2>;

//! Float-type 2-D ray.
using Ray2F = Ray2<float>;

//! Double-type 2-D ray.
using Ray2D = Ray2<double>;

}  // namespace vox

#include "math/ray2-inl.h"
