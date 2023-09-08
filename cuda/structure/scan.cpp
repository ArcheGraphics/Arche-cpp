//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "cuda_context.h"
#include "scan.h"

#include <numeric>

template<typename T>
void scan_host(const T *values_in, T *values_out, int n, bool inclusive) {
    static void *scan_temp_memory = nullptr;
    static size_t scan_temp_max_size = 0;

    // compute temporary memory required
    if (!inclusive && n > scan_temp_max_size) {
        free_host(scan_temp_memory);
        scan_temp_memory = alloc_host(sizeof(T) * n);
        scan_temp_max_size = n;
    }

    T *result = inclusive ? values_out : static_cast<T *>(scan_temp_memory);

    // scan
    std::partial_sum(values_in, values_in + n, result);
    if (!inclusive) {
        values_out[0] = (T)0;
        memcpy_h2h(values_out + 1, result, sizeof(T) * (n - 1));
    }
}

template void scan_host(const int *, int *, int, bool);
template void scan_host(const float *, float *, int, bool);
