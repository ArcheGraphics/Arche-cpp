//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "math/bounding_box2.h"
#include "math/ray2.h"
#include "math/vector2.h"

namespace vox {

//!
//! \brief Represents 2-D rigid body transform.
//!
template <typename T>
class Transform2 {
public:
    //! Constructs identity transform.
    Transform2();

    //! Constructs a transform with translation and orientation.
    Transform2(const Vector2<T> &translation, double orientation);

    //! Returns the translation.
    const Vector2<T> &translation() const;

    //! Sets the translation.
    void setTranslation(const Vector2<T> &translation);

    //! Returns the orientation in radians.
    [[nodiscard]] double orientation() const;

    //! Sets the orientation in radians.
    void setOrientation(double orientation);

    //! Transforms a point in world coordinate to the local frame.
    Point2<T> toLocal(const Point2<T> &point_in_world) const;

    //! Transforms a direction in world coordinate to the local frame.
    Vector2<T> toLocalDirection(const Vector2<T> &dir_in_world) const;

    //! Transforms a ray in world coordinate to the local frame.
    Ray2<T> toLocal(const Ray2<T> &ray_in_world) const;

    //! Transforms a bounding box in world coordinate to the local frame.
    BoundingBox2<T> toLocal(const BoundingBox2<T> &bbox_in_world) const;

    //! Transforms a point in local space to the world coordinate.
    Point2<T> toWorld(const Point2<T> &point_in_local) const;

    //! Transforms a direction in local space to the world coordinate.
    Vector2<T> toWorldDirection(const Vector2<T> &dir_in_local) const;

    //! Transforms a ray in local space to the world coordinate.
    Ray2<T> toWorld(const Ray2<T> &ray_in_local) const;

    //! Transforms a bounding box in local space to the world coordinate.
    BoundingBox2<T> toWorld(const BoundingBox2<T> &bbox_in_local) const;

private:
    Vector2<T> translation_;
    double orientation_ = 0.0;
    double cos_angle_ = 1.0;
    double sin_angle_ = 0.0;
};

//! Float-type 2-D Transform.
using Transform2F = Transform2<float>;

//! Double-type 2-D Transform.
using Transform2D = Transform2<double>;

}  // namespace vox

#include "math/transform2-inl.h"
