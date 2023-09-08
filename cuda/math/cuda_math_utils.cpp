//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "math/cuda_math_utils.h"

#if !defined(__CUDACC__)

// Helper for implementing isfinite()
bool isfinite(double x) {
    return std::isfinite(x);
}

#endif// !__CUDA_ARCH__