//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

namespace wp {

// wp::initializer_array<> is a simple substitute for std::initializer_list<>
// which doesn't depend on compiler implementation-specific support. It copies
// elements by value and only supports array-style indexing.
template<unsigned Length, typename Type>
struct initializer_array {
    const Type storage[Length];

    __device__ Type operator[](unsigned i) {
        return storage[i];
    }

    __device__ Type operator[](unsigned i) const {
        return storage[i];
    }
};

}// namespace wp
