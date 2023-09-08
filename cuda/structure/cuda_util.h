//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <cudaTypedefs.h>
#include <cuda_runtime_api.h>

#include <cstdio>

#define check_cuda(code) (check_cuda_result(code, __FILE__, __LINE__))
#define check_cu(code) (check_cu_result(code, __FILE__, __LINE__))

#if defined(__CUDACC__)
#if _DEBUG
// helper for launching kernels (synchronize + error checking after each kernel)
#define wp_launch_device(context, kernel, dim, args)                                      \
    {                                                                                     \
        if (dim) {                                                                        \
            ContextGuard guard(context);                                                  \
            const int num_threads = 256;                                                  \
            const int num_blocks = (dim + num_threads - 1) / num_threads;                 \
            kernel<<<num_blocks, 256, 0, (cudaStream_t)cuda_stream_get_current()>>> args; \
            check_cuda(cuda_context_check(WP_CURRENT_CONTEXT));                           \
        }                                                                                 \
    }
#else
// helper for launching kernels (no error checking)
#define wp_launch_device(context, kernel, dim, args)                                      \
    {                                                                                     \
        if (dim) {                                                                        \
            ContextGuard guard(context);                                                  \
            const int num_threads = 256;                                                  \
            const int num_blocks = (dim + num_threads - 1) / num_threads;                 \
            kernel<<<num_blocks, 256, 0, (cudaStream_t)cuda_stream_get_current()>>> args; \
        }                                                                                 \
    }
#endif// _DEBUG
#endif// defined(__CUDACC__)

bool check_cuda_result(cudaError_t code, const char *file, int line);

inline bool check_cuda_result(uint64_t code, const char *file, int line) {
    return check_cuda_result(static_cast<cudaError_t>(code), file, line);
}

bool check_cu_result(CUresult result, const char *file, int line);

#if defined(__CUDACC__)
const int LAUNCH_MAX_DIMS = 4;// should match types.py

struct launch_bounds_t {
    int shape[LAUNCH_MAX_DIMS];// size of each dimension
    int ndim;                  // number of valid dimension
    size_t size;               // total number of threads
};

// store launch bounds in shared memory so
// we can access them from any user func
// this is to avoid having to explicitly
// set another piece of __constant__ memory
// from the host
__shared__ launch_bounds_t s_launchBounds;

inline __device__ size_t grid_index() {
    // Need to cast at least one of the variables being multiplied so that type promotion happens before the multiplication
    size_t grid_index = static_cast<size_t>(blockDim.x) * static_cast<size_t>(blockIdx.x) + static_cast<size_t>(threadIdx.x);
    return grid_index;
}

inline __device__ int tid() {
    const size_t index = grid_index();

    // For the 1-D tid() we need to warn the user if we're about to provide a truncated index
    // Only do this in _DEBUG when called from device to avoid excessive register allocation
#if defined(_DEBUG) || !defined(__CUDA_ARCH__)
    if (index > 2147483647) {
        printf("Warp warning: tid() is returning an overflowed int\n");
    }
#endif
    return static_cast<int>(index);
}

inline __device__ void tid(int &i, int &j) {
    const size_t index = grid_index();

    const size_t n = s_launchBounds.shape[1];

    // convert to work item
    i = index / n;
    j = index % n;
}

inline __device__ void tid(int &i, int &j, int &k) {
    const size_t index = grid_index();

    const size_t n = s_launchBounds.shape[1];
    const size_t o = s_launchBounds.shape[2];

    // convert to work item
    i = index / (n * o);
    j = index % (n * o) / o;
    k = index % o;
}

inline __device__ void tid(int &i, int &j, int &k, int &l) {
    const size_t index = grid_index();

    const size_t n = s_launchBounds.shape[1];
    const size_t o = s_launchBounds.shape[2];
    const size_t p = s_launchBounds.shape[3];

    // convert to work item
    i = index / (n * o * p);
    j = index % (n * o * p) / (o * p);
    k = index % (o * p) / p;
    l = index % p;
}
#endif// defined(__CUDACC__)

//
// Scoped CUDA context guard
//
// Behaviour on entry
// - If the given `context` is NULL, do nothing.
// - If the given `context` is the same as the current context, do nothing.
// - If the given `context` is different from the current context, make the given context current.
//
// Behaviour on exit
// - If the current context did not change on entry, do nothing.
// - If the `restore` flag was true on entry, make the previous context current.
//
// Default exit behaviour policy
// - If the `restore` flag is omitted on entry, fall back on the global `always_restore` flag.
// - This allows us to easily change the default behaviour of the guards.
//
class ContextGuard {
public:
    // default policy for restoring contexts
    static bool always_restore;

    explicit ContextGuard(CUcontext context, bool restore = always_restore)
        : need_restore(false) {
        if (context) {
            if (check_cu(cuCtxGetCurrent(&prev_context)) && context != prev_context)
                need_restore = check_cu(cuCtxSetCurrent(context)) && restore;
        }
    }

    explicit ContextGuard(void *context, bool restore = always_restore)
        : ContextGuard(static_cast<CUcontext>(context), restore) {
    }

    ~ContextGuard() {
        if (need_restore)
            check_cu(cuCtxSetCurrent(prev_context));
    }

private:
    CUcontext prev_context{};
    bool need_restore;
};

// Pass this value to device functions as the `context` parameter to bypass unnecessary context management.
// This works in conjuntion with ContextGuards, which do nothing if the given context is NULL.
// Using this variable instead of passing NULL directly aids readability and makes the intent clear.
constexpr void *WP_CURRENT_CONTEXT = nullptr;
