//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "math/vec.h"

void array_sum_float_device(uint64_t a, uint64_t out, int count, int stride, int type_len);
void array_sum_float_host(uint64_t a, uint64_t out, int count, int stride, int type_len);
void array_sum_double_host(uint64_t a, uint64_t out, int count, int stride, int type_len);
void array_sum_double_device(uint64_t a, uint64_t out, int count, int stride, int type_len);

void array_inner_float_host(uint64_t a, uint64_t b, uint64_t out, int count, int stride_a, int stride_b, int type_len);
void array_inner_double_host(uint64_t a, uint64_t b, uint64_t out, int count, int stride_a, int stride_b, int type_len);
void array_inner_float_device(uint64_t a, uint64_t b, uint64_t out, int count, int stride_a, int stride_b, int type_len);
void array_inner_double_device(uint64_t a, uint64_t b, uint64_t out, int count, int stride_a, int stride_b, int type_len);