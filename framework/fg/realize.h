//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <memory>
#include <type_traits>

namespace vox::fg {
template<typename description_type, typename actual_type>
struct missing_realize_implementation : std::false_type {};

template<typename description_type, typename actual_type>
std::unique_ptr<actual_type> realize(const description_type &description) {
    static_assert(missing_realize_implementation<description_type, actual_type>::value, "Missing realize implementation for description - type pair.");
    return nullptr;
}
}// namespace vox::fg