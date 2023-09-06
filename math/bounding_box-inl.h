//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <algorithm>
#include <limits>

#include "math/math_utils.h"

namespace vox {

template <typename T, size_t N>
BoundingBox<T, N>::BoundingBox() {
    reset();
}

template <typename T, size_t N>
BoundingBox<T, N>::BoundingBox(const VectorType &point_1, const VectorType &point_2) {
    for (size_t i = 0; i < N; ++i) {
        lower_corner[i] = std::min(point_1[i], point_2[i]);
        upper_corner[i] = std::max(point_1[i], point_2[i]);
    }
}

template <typename T, size_t N>
BoundingBox<T, N>::BoundingBox(const BoundingBox &other)
    : lower_corner(other.lower_corner), upper_corner(other.upper_corner) {}

template <typename T, size_t N>
bool BoundingBox<T, N>::overlaps(const BoundingBox &other) const {
    for (size_t i = 0; i < N; ++i) {
        if (upper_corner[i] < other.lower_corner[i] || lower_corner[i] > other.upper_corner[i]) {
            return false;
        }
    }

    return true;
}

template <typename T, size_t N>
bool BoundingBox<T, N>::contains(const VectorType &point) const {
    for (size_t i = 0; i < N; ++i) {
        if (upper_corner[i] < point[i] || lower_corner[i] > point[i]) {
            return false;
        }
    }

    return true;
}

template <typename T, size_t N>
Vector<T, N> BoundingBox<T, N>::midPoint() const {
    Vector<T, N> result;
    for (size_t i = 0; i < N; ++i) {
        result[i] = (upper_corner[i] + lower_corner[i]) / 2;
    }
    return result;
}

template <typename T, size_t N>
T BoundingBox<T, N>::diagonalLength() const {
    T result = 0;
    for (size_t i = 0; i < N; ++i) {
        result += square(upper_corner[i] - lower_corner[i]);
    }
    return std::sqrt(result);
}

template <typename T, size_t N>
T BoundingBox<T, N>::diagonalLengthSquared() const {
    T result = 0;
    for (size_t i = 0; i < N; ++i) {
        result += square(upper_corner[i] - lower_corner[i]);
    }
    return result;
}

template <typename T, size_t N>
void BoundingBox<T, N>::reset() {
    for (size_t i = 0; i < N; ++i) {
        lower_corner[i] = std::numeric_limits<T>::max();
        upper_corner[i] = -std::numeric_limits<T>::max();
    }
}

template <typename T, size_t N>
void BoundingBox<T, N>::merge(const VectorType &point) {
    for (size_t i = 0; i < N; ++i) {
        lower_corner[i] = std::min(lower_corner[i], point[i]);
        upper_corner[i] = std::max(upper_corner[i], point[i]);
    }
}

template <typename T, size_t N>
void BoundingBox<T, N>::merge(const BoundingBox &other) {
    for (size_t i = 0; i < N; ++i) {
        lower_corner[i] = std::min(lower_corner[i], other.lower_corner[i]);
        upper_corner[i] = std::max(upper_corner[i], other.upper_corner[i]);
    }
}

template <typename T, size_t N>
void BoundingBox<T, N>::expand(T delta) {
    for (size_t i = 0; i < N; ++i) {
        lower_corner[i] -= delta;
        upper_corner[i] += delta;
    }
}

}  // namespace vox
