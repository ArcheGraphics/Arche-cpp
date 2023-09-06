//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <algorithm>
#include <limits>
#include <utility>

namespace vox {

template <typename T>
BoundingBox<T, 3>::BoundingBox() {
    reset();
}

template <typename T>
BoundingBox<T, 3>::BoundingBox(const Point3<T> &point_1, const Point3<T> &point_2) {
    lower_corner.x = std::min(point_1.x, point_2.x);
    lower_corner.y = std::min(point_1.y, point_2.y);
    lower_corner.z = std::min(point_1.z, point_2.z);
    upper_corner.x = std::max(point_1.x, point_2.x);
    upper_corner.y = std::max(point_1.y, point_2.y);
    upper_corner.z = std::max(point_1.z, point_2.z);
}

template <typename T>
BoundingBox<T, 3>::BoundingBox(const BoundingBox &other)
    : lower_corner(other.lower_corner), upper_corner(other.upper_corner) {}

template <typename T>
T BoundingBox<T, 3>::width() const {
    return upper_corner.x - lower_corner.x;
}

template <typename T>
T BoundingBox<T, 3>::height() const {
    return upper_corner.y - lower_corner.y;
}

template <typename T>
T BoundingBox<T, 3>::depth() const {
    return upper_corner.z - lower_corner.z;
}

template <typename T>
T BoundingBox<T, 3>::length(size_t axis) {
    return upper_corner[axis] - lower_corner[axis];
}

template <typename T>
bool BoundingBox<T, 3>::overlaps(const BoundingBox &other) const {
    if (upper_corner.x < other.lower_corner.x || lower_corner.x > other.upper_corner.x) {
        return false;
    }

    if (upper_corner.y < other.lower_corner.y || lower_corner.y > other.upper_corner.y) {
        return false;
    }

    if (upper_corner.z < other.lower_corner.z || lower_corner.z > other.upper_corner.z) {
        return false;
    }

    return true;
}

template <typename T>
bool BoundingBox<T, 3>::contains(const Point3<T> &point) const {
    if (upper_corner.x < point.x || lower_corner.x > point.x) {
        return false;
    }

    if (upper_corner.y < point.y || lower_corner.y > point.y) {
        return false;
    }

    if (upper_corner.z < point.z || lower_corner.z > point.z) {
        return false;
    }

    return true;
}

template <typename T>
bool BoundingBox<T, 3>::intersects(const Ray3<T> &ray) const {
    T t_min = 0;
    T t_max = std::numeric_limits<T>::max();
    const Vector3<T> &ray_inv_dir = ray.direction.rdiv(1);

    for (int i = 0; i < 3; ++i) {
        T t_near = (lower_corner[i] - ray.origin[i]) * ray_inv_dir[i];
        T t_far = (upper_corner[i] - ray.origin[i]) * ray_inv_dir[i];

        if (t_near > t_far) std::swap(t_near, t_far);
        t_min = t_near > t_min ? t_near : t_min;
        t_max = t_far < t_max ? t_far : t_max;

        if (t_min > t_max) return false;
    }

    return true;
}

template <typename T>
BoundingBoxRayIntersection3<T> BoundingBox<T, 3>::closestIntersection(const Ray3<T> &ray) const {
    BoundingBoxRayIntersection3<T> intersection;

    T t_min = 0;
    T t_max = std::numeric_limits<T>::max();
    const Vector3<T> &ray_inv_dir = ray.direction.rdiv(1);

    for (int i = 0; i < 3; ++i) {
        T t_near = (lower_corner[i] - ray.origin[i]) * ray_inv_dir[i];
        T t_far = (upper_corner[i] - ray.origin[i]) * ray_inv_dir[i];

        if (t_near > t_far) std::swap(t_near, t_far);
        t_min = t_near > t_min ? t_near : t_min;
        t_max = t_far < t_max ? t_far : t_max;

        if (t_min > t_max) {
            intersection.is_intersecting = false;
            return intersection;
        }
    }

    intersection.is_intersecting = true;

    if (contains(ray.origin)) {
        intersection.t_near = t_max;
        intersection.t_far = std::numeric_limits<T>::max();
    } else {
        intersection.t_near = t_min;
        intersection.t_far = t_max;
    }

    return intersection;
}

template <typename T>
Point3<T> BoundingBox<T, 3>::midPoint() const {
    Vector3<T> temp = upper_corner + lower_corner;

    return Point3<T>(temp.x / static_cast<T>(2), temp.y / static_cast<T>(2), temp.z / static_cast<T>(2));
}

template <typename T>
T BoundingBox<T, 3>::diagonalLength() const {
    return (upper_corner - lower_corner).length();
}

template <typename T>
T BoundingBox<T, 3>::diagonalLengthSquared() const {
    return (upper_corner - lower_corner).lengthSquared();
}

template <typename T>
void BoundingBox<T, 3>::reset() {
    lower_corner.x = std::numeric_limits<T>::max();
    lower_corner.y = std::numeric_limits<T>::max();
    lower_corner.z = std::numeric_limits<T>::max();
    upper_corner.x = -std::numeric_limits<T>::max();
    upper_corner.y = -std::numeric_limits<T>::max();
    upper_corner.z = -std::numeric_limits<T>::max();
}

template <typename T>
void BoundingBox<T, 3>::merge(const Point3<T> &point) {
    lower_corner.x = std::min(lower_corner.x, point.x);
    lower_corner.y = std::min(lower_corner.y, point.y);
    lower_corner.z = std::min(lower_corner.z, point.z);
    upper_corner.x = std::max(upper_corner.x, point.x);
    upper_corner.y = std::max(upper_corner.y, point.y);
    upper_corner.z = std::max(upper_corner.z, point.z);
}

template <typename T>
void BoundingBox<T, 3>::merge(const BoundingBox &other) {
    lower_corner.x = std::min(lower_corner.x, other.lower_corner.x);
    lower_corner.y = std::min(lower_corner.y, other.lower_corner.y);
    lower_corner.z = std::min(lower_corner.z, other.lower_corner.z);
    upper_corner.x = std::max(upper_corner.x, other.upper_corner.x);
    upper_corner.y = std::max(upper_corner.y, other.upper_corner.y);
    upper_corner.z = std::max(upper_corner.z, other.upper_corner.z);
}

template <typename T>
void BoundingBox<T, 3>::expand(T delta) {
    lower_corner -= delta;
    upper_corner += delta;
}

template <typename T>
void BoundingBox<T, 3>::transform(Matrix<T, 4, 4> matrix) {
    Point3<T> center = midPoint();
    Vector3<T> extent = this->extent();
    center = matrix * center;

    extent.x = std::abs(extent.x * matrix[0]) + std::abs(extent.y * matrix[4]) + std::abs(extent.z * matrix[8]);
    extent.y = std::abs(extent.x * matrix[1]) + std::abs(extent.y * matrix[5]) + std::abs(extent.z * matrix[9]);
    extent.z = std::abs(extent.x * matrix[2]) + std::abs(extent.y * matrix[6]) + std::abs(extent.z * matrix[10]);

    // set min、max
    lower_corner = center - extent;
    upper_corner = center + extent;
}

template <typename T>
BoundingBox<T, 3> BoundingBox<T, 3>::transform(Matrix<T, 4, 4> matrix) const {
    Point3<T> center = midPoint();
    Vector3<T> extent = this->extent();
    center = matrix * center;

    extent.x = std::abs(extent.x * matrix[0]) + std::abs(extent.y * matrix[4]) + std::abs(extent.z * matrix[8]);
    extent.y = std::abs(extent.x * matrix[1]) + std::abs(extent.y * matrix[5]) + std::abs(extent.z * matrix[9]);
    extent.z = std::abs(extent.x * matrix[2]) + std::abs(extent.y * matrix[6]) + std::abs(extent.z * matrix[10]);

    // set min、max
    return BoundingBox<T, 3>(center - extent, center + extent);
}

template <typename T>
Vector3<T> BoundingBox<T, 3>::extent() const {
    return (upper_corner - lower_corner) * (T)0.5;
}

template <typename T>
Point3<T> BoundingBox<T, 3>::corner(size_t idx) const {
    static const T kH = static_cast<T>(1) / 2;
    static const Vector3<T> offset[8] = {{-kH, -kH, -kH}, {+kH, -kH, -kH}, {-kH, +kH, -kH}, {+kH, +kH, -kH},
                                         {-kH, -kH, +kH}, {+kH, -kH, +kH}, {-kH, +kH, +kH}, {+kH, +kH, +kH}};

    return midPoint() + Vector3<T>(width(), height(), depth()) * offset[idx];
}

template <typename T>
Point3<T> BoundingBox<T, 3>::clamp(const Point3<T> &pt) const {
    return ::vox::clamp(pt, lower_corner, upper_corner);
}

template <typename T>
bool BoundingBox<T, 3>::isEmpty() const {
    return (lower_corner.x >= upper_corner.x || lower_corner.y >= upper_corner.y || lower_corner.z >= upper_corner.z);
}

}  // namespace vox
