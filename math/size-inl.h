//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <cassert>

#include "math/size.h"

namespace vox {

template <size_t N>
Size<N>::Size() {
    for (auto &elem : elements) {
        elem = 0;
    }
}

template <size_t N>
template <typename... Params>
Size<N>::Size(Params... params) {
    static_assert(sizeof...(params) == N, "Invalid number of parameters.");

    setAt(0, params...);
}

template <size_t N>
Size<N>::Size(const std::initializer_list<size_t> &lst) {
    assert(lst.size() >= N);

    size_t i = 0;
    for (const auto &inputElem : lst) {
        elements[i] = inputElem;
        ++i;
    }
}

template <size_t N>
Size<N>::Size(const Size &other) : elements(other.elements) {}

template <size_t N>
const size_t &Size<N>::operator[](size_t i) const {
    return elements[i];
}

template <size_t N>
size_t &Size<N>::operator[](size_t i) {
    return elements[i];
}

template <size_t N>
template <typename... Params>
void Size<N>::setAt(size_t i, size_t v, Params... params) {
    elements[i] = v;

    setAt(i + 1, params...);
}

template <size_t N>
void Size<N>::setAt(size_t i, size_t v) {
    elements[i] = v;
}

}  // namespace vox
