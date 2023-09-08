//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "math/vec.h"

void radix_sort_reserve(void *context, int n, void **mem_out = nullptr, size_t *size_out = nullptr);
void radix_sort_pairs_host(int *keys, int *values, int n);
void radix_sort_pairs_device(void *context, int *keys, int *values, int n);

void radix_sort_pairs_int_host(uint64_t keys, uint64_t values, int n);
void radix_sort_pairs_int_device(uint64_t keys, uint64_t values, int n);