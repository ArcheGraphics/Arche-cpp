//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vec.h"

namespace wp {

// All iterable types should implement 3 methods:
//
// T iter_next(iter)       - returns the current value and moves iterator to next state
// int iter_cmp(iter)      - returns 0 if finished
// iter iter_reverse(iter) - return an iterator of the same type representing the reverse order
//
// iter_next() should also be registered as a built-in hidden function so that code-gen
// can call it and generate the appropriate variable storage

// represents a built-in Python range() loop
struct range_t {
    __device__ range_t() = default;
    __device__ explicit range_t(int) {}// for backward pass

    int start{};
    int end{};
    int step{};

    int i{};
};

__device__ inline range_t range(int end) {
    range_t r{};
    r.start = 0;
    r.end = end;
    r.step = 1;

    r.i = r.start;

    return r;
}

__device__ inline range_t range(int start, int end) {
    range_t r{};
    r.start = start;
    r.end = end;
    r.step = 1;

    r.i = r.start;

    return r;
}

__device__ inline range_t range(int start, int end, int step) {
    range_t r{};
    r.start = start;
    r.end = end;
    r.step = step;

    r.i = r.start;

    return r;
}

__device__ inline int iter_next(range_t &r) {
    int iter = r.i;

    r.i += r.step;
    return iter;
}

__device__ inline bool iter_cmp(const range_t &r) {
    // implements for-loop comparison to emulate Python range() loops with negative arguments
    if (r.step == 0)
        // degenerate case where step == 0
        return false;
    if (r.step > 0)
        // normal case where step > 0
        return r.i < r.end;
    else
        // reverse case where step < 0
        return r.i > r.end;
}

__device__ inline range_t iter_reverse(const range_t &r) {
    // generates a reverse range, equivalent to reversed(range())
    range_t rev{};
    rev.start = r.end - 1;
    rev.end = r.start - 1;
    rev.step = -r.step;

    rev.i = rev.start;

    return rev;
}

}// namespace wp