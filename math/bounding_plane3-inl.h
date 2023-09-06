//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "math/bounding_plane3.h"

namespace vox {
template <typename T>
BoundingPlane<T, 3>::BoundingPlane() : BoundingPlane(Vector3<T>(), 0) {}

template <typename T>
BoundingPlane<T, 3>::BoundingPlane(const Vector3<T> &new_normal, const T &new_distance)
    : normal(new_normal), distance(new_distance) {}

template <typename T>
BoundingPlane<T, 3>::BoundingPlane(const Point3<T> &point_0, const Point3<T> &point_1, const Point3<T> &point_2) {
    T x_0 = point_0.x;
    T y_0 = point_0.y;
    T z_0 = point_0.z;
    T x_1 = point_1.x - x_0;
    T y_1 = point_1.y - y_0;
    T z_1 = point_1.z - z_0;
    T x_2 = point_2.x - x_0;
    T y_2 = point_2.y - y_0;
    T z_2 = point_2.z - z_0;
    T yz = y_1 * z_2 - z_1 * y_2;
    T xz = z_1 * x_2 - x_1 * z_2;
    T xy = x_1 * y_2 - y_1 * x_2;
    T inv_pyth = 1.0 / std::sqrt(yz * yz + xz * xz + xy * xy);

    T x = yz * inv_pyth;
    T y = xz * inv_pyth;
    T z = xy * inv_pyth;

    normal.x = x;
    normal.y = y;
    normal.z = z;
    distance = -(x * x_0 + y * y_0 + z * z_0);
}

template <typename T>
BoundingPlane<T, 3>::BoundingPlane(const BoundingPlane &other) : normal(other.normal), distance(other.distance) {}

template <typename T>
BoundingPlane<T, 3> BoundingPlane<T, 3>::normalized() const {
    T factor = 1.0 / normal.length();

    Vector3<T> out_normal(normal.x * factor, normal.y * factor, normal.z * factor);
    return BoundingPlane<T, 3>(out_normal, distance * factor);
}

template <typename T>
void BoundingPlane<T, 3>::normalize() {
    T factor = 1.0 / normal.length();

    normal.x *= factor;
    normal.y *= factor;
    normal.z *= factor;
    distance *= factor;
}

}  // namespace vox
