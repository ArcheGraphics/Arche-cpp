//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <algorithm>
#include <cmath>

#include "math/transform3.h"

namespace vox {
template <typename T>
inline Transform3<T>::Transform3() = default;

template <typename T>
inline Transform3<T>::Transform3(const Vector3<T> &translation, const Quaternion<T> &orientation) {
    setTranslation(translation);
    setOrientation(orientation);
}

template <typename T>
inline const Vector3<T> &Transform3<T>::translation() const {
    return translation_;
}

template <typename T>
inline void Transform3<T>::setTranslation(const Vector3<T> &translation) {
    translation_ = translation;
}

template <typename T>
inline const Quaternion<T> &Transform3<T>::orientation() const {
    return orientation_;
}

template <typename T>
inline void Transform3<T>::setOrientation(const Quaternion<T> &orientation) {
    orientation_ = orientation;
    orientation_mat_3_ = orientation.matrix3();
    inverse_orientation_mat_3_ = orientation.inverse().matrix3();
}

template <typename T>
inline Point3<T> Transform3<T>::toLocal(const Point3<T> &point_in_world) const {
    return inverse_orientation_mat_3_ * (point_in_world - translation_);
}

template <typename T>
inline Vector3<T> Transform3<T>::toLocalDirection(const Vector3<T> &dir_in_world) const {
    return inverse_orientation_mat_3_ * dir_in_world;
}

template <typename T>
inline Ray3<T> Transform3<T>::toLocal(const Ray3<T> &ray_in_world) const {
    return Ray3<T>(toLocal(ray_in_world.origin), toLocalDirection(ray_in_world.direction));
}

template <typename T>
inline BoundingBox3<T> Transform3<T>::toLocal(const BoundingBox3<T> &bbox_in_world) const {
    BoundingBox3<T> bbox_in_local;
    for (int i = 0; i < 8; ++i) {
        auto corner_in_local = toLocal(bbox_in_world.corner(i));
        bbox_in_local.lower_corner = min(bbox_in_local.lower_corner, corner_in_local);
        bbox_in_local.upper_corner = max(bbox_in_local.upper_corner, corner_in_local);
    }
    return bbox_in_local;
}

template <typename T>
inline Point3<T> Transform3<T>::toWorld(const Point3<T> &point_in_local) const {
    return (orientation_mat_3_ * point_in_local) + translation_;
}

template <typename T>
inline Vector3<T> Transform3<T>::toWorldDirection(const Vector3<T> &dir_in_local) const {
    return orientation_mat_3_ * dir_in_local;
}

template <typename T>
inline Ray3<T> Transform3<T>::toWorld(const Ray3<T> &ray_in_local) const {
    return Ray3<T>(toWorld(ray_in_local.origin), toWorldDirection(ray_in_local.direction));
}

template <typename T>
inline BoundingBox3<T> Transform3<T>::toWorld(const BoundingBox3<T> &bbox_in_local) const {
    BoundingBox3<T> bbox_in_world;
    for (int i = 0; i < 8; ++i) {
        auto corner_in_world = toWorld(bbox_in_local.corner(i));
        bbox_in_world.lower_corner = min(bbox_in_world.lower_corner, corner_in_world);
        bbox_in_world.upper_corner = max(bbox_in_world.upper_corner, corner_in_world);
    }
    return bbox_in_world;
}

}  // namespace vox
