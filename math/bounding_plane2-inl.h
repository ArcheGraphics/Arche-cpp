//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "math/bounding_plane2.h"

namespace vox {
template <typename T>
BoundingPlane<T, 2>::BoundingPlane() : BoundingPlane(Vector2<T>(), 0) {}

template <typename T>
BoundingPlane<T, 2>::BoundingPlane(const Vector2<T> &newNormal, const T &newDistance)
    : normal(newNormal), distance(newDistance) {}

template <typename T>
BoundingPlane<T, 2>::BoundingPlane(const Vector2<T> &point0, const Vector2<T> &point1) {
    // TODO
}

template <typename T>
BoundingPlane<T, 2>::BoundingPlane(const BoundingPlane &other) : normal(other.normal), distance(other.distance) {}

template <typename T>
BoundingPlane<T, 2> BoundingPlane<T, 2>::normalized() const {
    T factor = 1.0 / normal.length();

    Vector2<T> outNormal(normal.x * factor, normal.y * factor);
    return Plane<T, 2>(outNormal, distance * factor);
}

template <typename T>
void BoundingPlane<T, 2>::normalize() {
    T factor = 1.0 / normal.length();

    normal.x *= factor;
    normal.y *= factor;
    distance *= factor;
}

}  // namespace vox
