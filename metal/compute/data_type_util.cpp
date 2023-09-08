//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "data_type_util.h"

#include <cstring>
#include <type_traits>

namespace vox::compute {

template<typename To, typename From>
static To bitcast(From x) {
    static_assert(std::is_trivially_copyable_v<From>);
    static_assert(std::is_trivially_copy_constructible_v<To>);
    static_assert(sizeof(From) == sizeof(To));
    To result{};
    memcpy(&result, &x, sizeof(result));
    return result;
}

void fp16::from_float(float x) {
    auto asInt = bitcast<uint32_t>(x);
    int sign = (asInt & 0x80000000) >> 31;
    int exp = ((asInt & 0x7f800000) >> 23) - 127 + 15;
    int mantissa = (asInt & 0x7FFFFF);
    if (exp > 31) exp = 31;
    if (exp < 0) exp = 0;
    sign = sign << 15;
    exp = exp << 10;
    mantissa = mantissa >> (23 - 10);
    asInt = sign | exp | mantissa;
    value_ = static_cast<uint16_t>(asInt);
}

float fp16::to_float() const {
    auto asInt = static_cast<uint32_t>(value_);
    int sign = (asInt & 0x8000) >> 15;
    int exp = ((asInt & 0x7c00) >> 10);
    int mantissa = (asInt & 0x3FF);
    sign = sign << 31;
    if (exp > 0) {
        exp = (exp + 127 - 15) << 23;
        mantissa = mantissa << (23 - 10);
    } else {
        mantissa = 0;
    }
    asInt = sign | exp | mantissa;
    return bitcast<float>(asInt);
}

}// namespace vox::compute
