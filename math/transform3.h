//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "math/bounding_box3.h"
#include "math/quaternion.h"
#include "math/ray3.h"
#include "math/vector3.h"

namespace vox {

//!
//! \brief Represents 3-D rigid body transform.
//!
template <typename T>
class Transform3 {
public:
    //! Constructs identity transform.
    Transform3();

    //! Constructs a transform with translation and orientation.
    Transform3(const Vector3<T> &translation, const Quaternion<T> &orientation);

    //! Returns the translation.
    const Vector3<T> &translation() const;

    //! Sets the translation.
    void setTranslation(const Vector3<T> &translation);

    //! Returns the orientation.
    const Quaternion<T> &orientation() const;

    //! Sets the orientation.
    void setOrientation(const Quaternion<T> &orientation);

    //! Transforms a point in world coordinate to the local frame.
    Point3<T> toLocal(const Point3<T> &point_in_world) const;

    //! Transforms a direction in world coordinate to the local frame.
    Vector3<T> toLocalDirection(const Vector3<T> &dir_in_world) const;

    //! Transforms a ray in world coordinate to the local frame.
    Ray3<T> toLocal(const Ray3<T> &ray_in_world) const;

    //! Transforms a bounding box in world coordinate to the local frame.
    BoundingBox3<T> toLocal(const BoundingBox3<T> &bbox_in_world) const;

    //! Transforms a point in local space to the world coordinate.
    Point3<T> toWorld(const Point3<T> &point_in_local) const;

    //! Transforms a direction in local space to the world coordinate.
    Vector3<T> toWorldDirection(const Vector3<T> &dir_in_local) const;

    //! Transforms a ray in local space to the world coordinate.
    Ray3<T> toWorld(const Ray3<T> &ray_in_local) const;

    //! Transforms a bounding box in local space to the world coordinate.
    BoundingBox3<T> toWorld(const BoundingBox3<T> &bbox_in_local) const;

private:
    Vector3<T> translation_;
    Quaternion<T> orientation_;
    Matrix3x3<T> orientation_mat_3_;
    Matrix3x3<T> inverse_orientation_mat_3_;
};

//! Float-type 3-D Transform.
using Transform3F = Transform3<float>;

//! Double-type 3-D Transform.
using Transform3D = Transform3<double>;

}  // namespace vox

#include "math/transform3-inl.h"
