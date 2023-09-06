//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "math/vector.h"

namespace vox {

//!
//! \brief Generic N-D axis-aligned bounding box class.
//!
//! \tparam T - Real number type.
//! \tparam N - Dimension.
//!
template <typename T, size_t N>
struct BoundingBox {
public:
    static_assert(N > 0, "Size of static-sized box should be greater than zero.");

    using VectorType = Vector<T, N>;

    //! Lower corner of the bounding box.
    VectorType lower_corner;

    //! Upper corner of the bounding box.
    VectorType upper_corner;

    //! Default constructor.
    BoundingBox();

    //! Constructs a box that tightly covers two points.
    BoundingBox(const VectorType &point_1, const VectorType &point_2);

    //! Constructs a box with other box instance.
    BoundingBox(const BoundingBox &other);

    //! Returns true of this box and other box overlaps.
    bool overlaps(const BoundingBox &other) const;

    //! Returns true if the input point is inside of this box.
    bool contains(const VectorType &point) const;

    //! Returns the mid-point of this box.
    VectorType midPoint() const;

    //! Returns diagonal length of this box.
    T diagonalLength() const;

    //! Returns squared diagonal length of this box.
    T diagonalLengthSquared() const;

    //! Resets this box to initial state (min=infinite, max=-infinite).
    void reset();

    //! Merges this and other point.
    void merge(const VectorType &point);

    //! Merges this and other boxes.
    void merge(const BoundingBox &other);

    //! Expands this box by given delta to all direction.
    //! If the width of the box was x, expand(y) will result a box with
    //! x+y+y width.
    void expand(T delta);
};

}  // namespace vox

#include "math/bounding_box-inl.h"
