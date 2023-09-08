//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "cuda_util.h"

bool ContextGuard::always_restore = false;

bool check_cuda_result(cudaError_t code, const char *file, int line) {
    if (code == cudaSuccess)
        return true;

    fprintf(stderr, "Warp CUDA error %u: %s (%s:%d)\n", unsigned(code), cudaGetErrorString(code), file, line);
    return false;
}

bool check_cu_result(CUresult result, const char *file, int line) {
    if (result == CUDA_SUCCESS)
        return true;

    const char *errString = nullptr;
    cuGetErrorString(result, &errString);

    if (errString)
        fprintf(stderr, "Warp CUDA error %u: %s (%s:%d)\n", unsigned(result), errString, file, line);
    else
        fprintf(stderr, "Warp CUDA error %u (%s:%d)\n", unsigned(result), file, line);

    return false;
}
