//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <limits>

#include "math/bounding_box.h"
#include "math/matrix4x4.h"
#include "math/point3.h"
#include "math/ray3.h"
#include "math/vector3.h"

namespace vox {

//!
//! \brief      3-D box-ray intersection result.
//!
//! \tparam     T     The value type.
//!
template <typename T>
struct BoundingBoxRayIntersection3 {
    //! True if the box and ray intersects.
    bool is_intersecting = false;

    //! Distance to the first intersection point.
    T t_near = std::numeric_limits<T>::max();

    //! Distance to the second (and the last) intersection point.
    T t_far = std::numeric_limits<T>::max();
};

//!
//! \brief 3-D axis-aligned bounding box class.
//!
//! \tparam T - Real number type.
//!
template <typename T>
struct BoundingBox<T, 3> {
public:
    //! Lower corner of the bounding box.
    Point3<T> lower_corner;

    //! Upper corner of the bounding box.
    Point3<T> upper_corner;

    //! Default constructor.
    BoundingBox();

    //! Constructs a box that tightly covers two points.
    BoundingBox(const Point3<T> &point_1, const Point3<T> &point_2);

    //! Constructs a box with other box instance.
    BoundingBox(const BoundingBox &other);

    //! Returns width of the box.
    T width() const;

    //! Returns height of the box.
    T height() const;

    //! Returns depth of the box.
    T depth() const;

    //! Returns length of the box in given axis.
    T length(size_t axis);

    //! Returns true of this box and other box overlaps.
    bool overlaps(const BoundingBox &other) const;

    //! Returns true if the input vector is inside of this box.
    bool contains(const Point3<T> &point) const;

    //! Returns true if the input ray is intersecting with this box.
    bool intersects(const Ray3<T> &ray) const;

    //! Returns intersection.isIntersecting = true if the input ray is
    //! intersecting with this box. If interesects, intersection.tNear is
    //! assigned with distant to the closest intersecting point, and
    //! intersection.tFar with furthest.
    BoundingBoxRayIntersection3<T> closestIntersection(const Ray3<T> &ray) const;

    //! Returns the mid-point of this box.
    Point3<T> midPoint() const;

    //! Returns diagonal length of this box.
    T diagonalLength() const;

    //! Returns squared diagonal length of this box.
    T diagonalLengthSquared() const;

    //! Resets this box to initial state (min=infinite, max=-infinite).
    void reset();

    //! Merges this and other point.
    void merge(const Point3<T> &point);

    //! Merges this and other box.
    void merge(const BoundingBox &other);

    //! Expands this box by given delta to all direction.
    //! If the width of the box was x, expand(y) will result a box with
    //! x+y+y width.
    void expand(T delta);

    /**
     * Transform a bounding box.
     * @param matrix - The transform to apply to the bounding box
     */
    void transform(Matrix<T, 4, 4> matrix);

    /**
     * Transform a bounding box.
     * @param matrix - The transform to apply to the bounding box
     * @returns The transformed bounding box
     */
    BoundingBox<T, 3> transform(Matrix<T, 4, 4> matrix) const;

    /**
     * Get the extent of this bounding box.
     * @returns The extent of this bounding box
     */
    Vector3<T> extent() const;

    //! Returns corner position. Index starts from x-first order.
    Point3<T> corner(size_t idx) const;

    //! Returns the clamped point.
    Point3<T> clamp(const Point3<T> &pt) const;

    //! Returns true if the box is empty.
    [[nodiscard]] bool isEmpty() const;
};

//! Type alias for 3-D BoundingBox.
template <typename T>
using BoundingBox3 = BoundingBox<T, 3>;

//! Float-type 3-D BoundingBox.
using BoundingBox3F = BoundingBox3<float>;

//! Double-type 3-D BoundingBox.
using BoundingBox3D = BoundingBox3<double>;

//! Float-type 3-D box-ray intersection result.
using BoundingBoxRayIntersection3F = BoundingBoxRayIntersection3<float>;

//! Double-type 3-D box-ray intersection result.
using BoundingBoxRayIntersection3D = BoundingBoxRayIntersection3<double>;

}  // namespace vox

#include "math/bounding_box3-inl.h"
