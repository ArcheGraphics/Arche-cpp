//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "math/vector.h"

namespace vox {

//!
//! \brief      Class for ray.
//!
//! \tparam     T     The value type.
//! \tparam     N     The dimension.
//!
template <typename T, size_t N>
struct BoundingPlane {
    static_assert(N != 2 && N != 3, "Not implemented.");
    static_assert(std::is_floating_point<T>::value, "Plane only can be instantiated with floating point types");
};

}  // namespace vox
