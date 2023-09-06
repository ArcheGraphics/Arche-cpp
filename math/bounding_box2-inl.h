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
BoundingBox<T, 2>::BoundingBox() {
    reset();
}

template <typename T>
BoundingBox<T, 2>::BoundingBox(const Point2<T> &point_1, const Point2<T> &point_2) {
    lower_corner.x = std::min(point_1.x, point_2.x);
    lower_corner.y = std::min(point_1.y, point_2.y);
    upper_corner.x = std::max(point_1.x, point_2.x);
    upper_corner.y = std::max(point_1.y, point_2.y);
}

template <typename T>
BoundingBox<T, 2>::BoundingBox(const BoundingBox &other)
    : lower_corner(other.lower_corner), upper_corner(other.upper_corner) {}

template <typename T>
T BoundingBox<T, 2>::width() const {
    return upper_corner.x - lower_corner.x;
}

template <typename T>
T BoundingBox<T, 2>::height() const {
    return upper_corner.y - lower_corner.y;
}

template <typename T>
T BoundingBox<T, 2>::length(size_t axis) {
    return upper_corner[axis] - lower_corner[axis];
}

template <typename T>
bool BoundingBox<T, 2>::overlaps(const BoundingBox &other) const {
    if (upper_corner.x < other.lower_corner.x || lower_corner.x > other.upper_corner.x) {
        return false;
    }

    if (upper_corner.y < other.lower_corner.y || lower_corner.y > other.upper_corner.y) {
        return false;
    }

    return true;
}

template <typename T>
bool BoundingBox<T, 2>::contains(const Point2<T> &point) const {
    if (upper_corner.x < point.x || lower_corner.x > point.x) {
        return false;
    }

    if (upper_corner.y < point.y || lower_corner.y > point.y) {
        return false;
    }

    return true;
}

template <typename T>
bool BoundingBox<T, 2>::intersects(const Ray2<T> &ray) const {
    T t_min = 0;
    T t_max = std::numeric_limits<T>::max();

    const Vector2<T> &ray_inv_dir = ray.direction.rdiv(1);

    for (int i = 0; i < 2; ++i) {
        T t_near = (lower_corner[i] - ray.origin[i]) * ray_inv_dir[i];
        T t_far = (upper_corner[i] - ray.origin[i]) * ray_inv_dir[i];

        if (t_near > t_far) {
            std::swap(t_near, t_far);
        }

        t_min = std::max(t_near, t_min);
        t_max = std::min(t_far, t_max);

        if (t_min > t_max) {
            return false;
        }
    }

    return true;
}

template <typename T>
BoundingBoxRayIntersection2<T> BoundingBox<T, 2>::closestIntersection(const Ray2<T> &ray) const {
    BoundingBoxRayIntersection2<T> intersection;

    T t_min = 0;
    T t_max = std::numeric_limits<T>::max();

    const Vector2<T> &ray_inv_dir = ray.direction.rdiv(1);

    for (int i = 0; i < 2; ++i) {
        T t_near = (lower_corner[i] - ray.origin[i]) * ray_inv_dir[i];
        T t_far = (upper_corner[i] - ray.origin[i]) * ray_inv_dir[i];

        if (t_near > t_far) {
            std::swap(t_near, t_far);
        }

        t_min = std::max(t_near, t_min);
        t_max = std::min(t_far, t_max);

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
Point2<T> BoundingBox<T, 2>::midPoint() const {
    Vector2<T> temp = upper_corner + lower_corner;

    return Point2<T>(temp.x / static_cast<T>(2), temp.y / static_cast<T>(2));
}

template <typename T>
T BoundingBox<T, 2>::diagonalLength() const {
    return (upper_corner - lower_corner).length();
}

template <typename T>
T BoundingBox<T, 2>::diagonalLengthSquared() const {
    return (upper_corner - lower_corner).lengthSquared();
}

template <typename T>
void BoundingBox<T, 2>::reset() {
    lower_corner.x = std::numeric_limits<T>::max();
    lower_corner.y = std::numeric_limits<T>::max();
    upper_corner.x = -std::numeric_limits<T>::max();
    upper_corner.y = -std::numeric_limits<T>::max();
}

template <typename T>
void BoundingBox<T, 2>::merge(const Point2<T> &point) {
    lower_corner.x = std::min(lower_corner.x, point.x);
    lower_corner.y = std::min(lower_corner.y, point.y);
    upper_corner.x = std::max(upper_corner.x, point.x);
    upper_corner.y = std::max(upper_corner.y, point.y);
}

template <typename T>
void BoundingBox<T, 2>::merge(const BoundingBox &other) {
    lower_corner.x = std::min(lower_corner.x, other.lower_corner.x);
    lower_corner.y = std::min(lower_corner.y, other.lower_corner.y);
    upper_corner.x = std::max(upper_corner.x, other.upper_corner.x);
    upper_corner.y = std::max(upper_corner.y, other.upper_corner.y);
}

template <typename T>
void BoundingBox<T, 2>::expand(T delta) {
    lower_corner -= delta;
    upper_corner += delta;
}

template <typename T>
Point2<T> BoundingBox<T, 2>::corner(size_t idx) const {
    static const T kH = static_cast<T>(1) / 2;
    static const Vector2<T> offset[4] = {{-kH, -kH}, {+kH, -kH}, {-kH, +kH}, {+kH, +kH}};

    return midPoint() + Vector2<T>(width(), height()) * offset[idx];
}

template <typename T>
Point2<T> BoundingBox<T, 2>::clamp(const Point2<T> &pt) const {
    return ::vox::clamp(pt, lower_corner, upper_corner);
}

template <typename T>
bool BoundingBox<T, 2>::isEmpty() const {
    return (lower_corner.x >= upper_corner.x || lower_corner.y >= upper_corner.y);
}

}  // namespace vox
