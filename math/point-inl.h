//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <cassert>

#include "math/macros.h"

namespace vox {

template <typename T, size_t N>
Point<T, N>::Point() {
    for (auto &elem : elements_) {
        elem = static_cast<T>(0);
    }
}

template <typename T, size_t N>
template <typename... Params>
Point<T, N>::Point(Params... params) {
    static_assert(sizeof...(params) == N, "Invalid number of parameters.");

    setAt(0, params...);
}

template <typename T, size_t N>
template <typename U>
Point<T, N>::Point(const std::initializer_list<U> &lst) {
    VOX_ASSERT(lst.size() >= N);

    size_t i = 0;
    for (const auto &inputElem : lst) {
        elements_[i] = static_cast<T>(inputElem);
        ++i;
    }
}

template <typename T, size_t N>
Point<T, N>::Point(const Point &other) : elements_(other.elements_) {}

template <typename T, size_t N>
template <typename U>
void Point<T, N>::set(const std::initializer_list<U> &lst) {
    assert(lst.size() >= N);

    size_t i = 0;
    for (const auto &inputElem : lst) {
        elements_[i] = static_cast<T>(inputElem);
        ++i;
    }
}

template <typename T, size_t N>
void Point<T, N>::set(const Point &other) {
    elements_ = other.elements_;
}

template <typename T, size_t N>
template <typename U>
Point<T, N> &Point<T, N>::operator=(const std::initializer_list<U> &lst) {
    set(lst);
    return *this;
}

template <typename T, size_t N>
Point<T, N> &Point<T, N>::operator=(const Point &other) {
    set(other);
    return *this;
}

template <typename T, size_t N>
const T &Point<T, N>::operator[](size_t i) const {
    return elements_[i];
}

template <typename T, size_t N>
T &Point<T, N>::operator[](size_t i) {
    return elements_[i];
}

template <typename T, size_t N>
template <typename... Params>
void Point<T, N>::setAt(size_t i, T v, Params... params) {
    elements_[i] = v;

    setAt(i + 1, params...);
}

template <typename T, size_t N>
void Point<T, N>::setAt(size_t i, T v) {
    elements_[i] = v;
}

}  // namespace vox
