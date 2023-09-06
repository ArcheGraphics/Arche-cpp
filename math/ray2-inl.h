//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

namespace vox {

template <typename T>
Ray<T, 2>::Ray() : Ray(Point2<T>(), Vector2<T>(1, 0)) {}

template <typename T>
Ray<T, 2>::Ray(const Point2<T> &new_origin, const Vector2<T> &new_direction)
    : origin(new_origin), direction(new_direction.normalized()) {}

template <typename T>
Ray<T, 2>::Ray(const Ray &other) : origin(other.origin), direction(other.direction) {}

template <typename T>
Point2<T> Ray<T, 2>::pointAt(T t) const {
    return origin + t * direction;
}

}  // namespace vox
