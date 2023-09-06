//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <algorithm>
#include <cmath>

#include "math/transform2.h"

namespace vox {
template <typename T>
inline Transform2<T>::Transform2() = default;

template <typename T>
inline Transform2<T>::Transform2(const Vector2<T> &translation, double orientation)
    : translation_(translation), orientation_(orientation) {
    cos_angle_ = std::cos(orientation);
    sin_angle_ = std::sin(orientation);
}

template <typename T>
inline const Vector2<T> &Transform2<T>::translation() const {
    return translation_;
}

template <typename T>
inline void Transform2<T>::setTranslation(const Vector2<T> &translation) {
    translation_ = translation;
}

template <typename T>
inline double Transform2<T>::orientation() const {
    return orientation_;
}

template <typename T>
inline void Transform2<T>::setOrientation(double orientation) {
    orientation_ = orientation;
    cos_angle_ = std::cos(orientation);
    sin_angle_ = std::sin(orientation);
}

template <typename T>
inline Point2<T> Transform2<T>::toLocal(const Point2<T> &point_in_world) const {
    // Convert to the local frame
    Point2<T> xmt = point_in_world - translation_;
    return Point2<T>(cos_angle_ * xmt.x + sin_angle_ * xmt.y, -sin_angle_ * xmt.x + cos_angle_ * xmt.y);
}

template <typename T>
inline Vector2<T> Transform2<T>::toLocalDirection(const Vector2<T> &dir_in_world) const {
    // Convert to the local frame
    return Vector2<T>(cos_angle_ * dir_in_world.x + sin_angle_ * dir_in_world.y,
                      -sin_angle_ * dir_in_world.x + cos_angle_ * dir_in_world.y);
}

template <typename T>
inline Ray2<T> Transform2<T>::toLocal(const Ray2<T> &ray_in_world) const {
    return Ray2<T>(toLocal(ray_in_world.origin), toLocalDirection(ray_in_world.direction));
}

template <typename T>
inline BoundingBox2<T> Transform2<T>::toLocal(const BoundingBox2<T> &bbox_in_world) const {
    BoundingBox2<T> bbox_in_local;
    for (int i = 0; i < 4; ++i) {
        auto corner_in_local = toLocal(bbox_in_world.corner(i));
        bbox_in_local.lower_corner = min(bbox_in_local.lower_corner, corner_in_local);
        bbox_in_local.upper_corner = max(bbox_in_local.upper_corner, corner_in_local);
    }
    return bbox_in_local;
}

template <typename T>
inline Point2<T> Transform2<T>::toWorld(const Point2<T> &point_in_local) const {
    // Convert to the world frame
    return Point2<T>(cos_angle_ * point_in_local.x - sin_angle_ * point_in_local.y + translation_.x,
                     sin_angle_ * point_in_local.x + cos_angle_ * point_in_local.y + translation_.y);
}

template <typename T>
inline Vector2<T> Transform2<T>::toWorldDirection(const Vector2<T> &dir_in_local) const {
    // Convert to the world frame
    return Vector2<T>(cos_angle_ * dir_in_local.x - sin_angle_ * dir_in_local.y,
                      sin_angle_ * dir_in_local.x + cos_angle_ * dir_in_local.y);
}

template <typename T>
inline Ray2<T> Transform2<T>::toWorld(const Ray2<T> &ray_in_local) const {
    return Ray2<T>(toWorld(ray_in_local.origin), toWorldDirection(ray_in_local.direction));
}

template <typename T>
inline BoundingBox2<T> Transform2<T>::toWorld(const BoundingBox2<T> &bbox_in_local) const {
    BoundingBox2<T> bbox_in_world;
    for (int i = 0; i < 4; ++i) {
        auto corner_in_world = toWorld(bbox_in_local.corner(i));
        bbox_in_world.lower_corner = min(bbox_in_world.lower_corner, corner_in_world);
        bbox_in_world.upper_corner = max(bbox_in_world.upper_corner, corner_in_world);
    }
    return bbox_in_world;
}

}  // namespace vox
