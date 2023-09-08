//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "cuda_context.h"
#include "scan.h"
#include "cuda_util.h"
#include "math/vec.h"
#include "math/array.h"
//#include <nvrtc.h>

#include <map>
#include <vector>

struct DeviceInfo {
    static constexpr int kNameLen = 128;

    CUdevice device = -1;
    int ordinal = -1;
    char name[kNameLen] = "";
    int arch = 0;
    int is_uva = 0;
};

struct ContextInfo {
    DeviceInfo *device_info = nullptr;

    CUstream stream = nullptr;// created when needed
};

// cached info for all devices, indexed by ordinal
static std::vector<DeviceInfo> g_devices;

// maps CUdevice to DeviceInfo
static std::map<CUdevice, DeviceInfo *> g_device_map;

// cached info for all known contexts
static std::map<CUcontext, ContextInfo> g_contexts;

void cuda_set_context_restore_policy(bool always_restore) {
    ContextGuard::always_restore = always_restore;
}

int cuda_get_context_restore_policy() {
    return int(ContextGuard::always_restore);
}

int cuda_init() {
    int deviceCount = 0;
    if (check_cu(cuDeviceGetCount(&deviceCount))) {
        g_devices.resize(deviceCount);

        for (int i = 0; i < deviceCount; i++) {
            CUdevice device;
            if (check_cu(cuDeviceGet(&device, i))) {
                // query device info
                g_devices[i].device = device;
                g_devices[i].ordinal = i;
                check_cu(cuDeviceGetName(g_devices[i].name, DeviceInfo::kNameLen, device));
                check_cu(cuDeviceGetAttribute(&g_devices[i].is_uva, CU_DEVICE_ATTRIBUTE_UNIFIED_ADDRESSING, device));
                int major = 0;
                int minor = 0;
                check_cu(cuDeviceGetAttribute(&major, CU_DEVICE_ATTRIBUTE_COMPUTE_CAPABILITY_MAJOR, device));
                check_cu(cuDeviceGetAttribute(&minor, CU_DEVICE_ATTRIBUTE_COMPUTE_CAPABILITY_MINOR, device));
                g_devices[i].arch = 10 * major + minor;

                g_device_map[device] = &g_devices[i];
            } else {
                return -1;
            }
        }
    } else {
        return -1;
    }

    return 0;
}

static inline CUcontext get_current_context() {
    CUcontext ctx;
    if (check_cu(cuCtxGetCurrent(&ctx)))
        return ctx;
    else
        return nullptr;
}

static inline CUstream get_current_stream() {
    return static_cast<CUstream>(cuda_context_get_stream(nullptr));
}

static ContextInfo *get_context_info(CUcontext ctx) {
    if (!ctx) {
        ctx = get_current_context();
        if (!ctx)
            return nullptr;
    }

    auto it = g_contexts.find(ctx);
    if (it != g_contexts.end()) {
        return &it->second;
    } else {
        // previously unseen context, add the info
        ContextGuard guard(ctx, true);
        ContextInfo context_info;
        CUdevice device;
        if (check_cu(cuCtxGetDevice(&device))) {
            context_info.device_info = g_device_map[device];
            auto result = g_contexts.insert(std::make_pair(ctx, context_info));
            return &result.first->second;
        }
    }

    return nullptr;
}

void *alloc_pinned(size_t s) {
    void *ptr;
    check_cuda(cudaMallocHost(&ptr, s));
    return ptr;
}

void free_pinned(void *ptr) {
    cudaFreeHost(ptr);
}

void *alloc_device(void *context, size_t s) {
    ContextGuard guard(context);

    void *ptr;
    check_cuda(cudaMalloc(&ptr, s));
    return ptr;
}

void free_device(void *context, void *ptr) {
    ContextGuard guard(context);

    check_cuda(cudaFree(ptr));
}

void memcpy_h2d(void *context, void *dest, void *src, size_t n) {
    ContextGuard guard(context);

    check_cuda(cudaMemcpyAsync(dest, src, n, cudaMemcpyHostToDevice, get_current_stream()));
}

void memcpy_d2h(void *context, void *dest, void *src, size_t n) {
    ContextGuard guard(context);

    check_cuda(cudaMemcpyAsync(dest, src, n, cudaMemcpyDeviceToHost, get_current_stream()));
}

void memcpy_d2d(void *context, void *dest, void *src, size_t n) {
    ContextGuard guard(context);

    check_cuda(cudaMemcpyAsync(dest, src, n, cudaMemcpyDeviceToDevice, get_current_stream()));
}

void memcpy_peer(void *context, void *dest, void *src, size_t n) {
    ContextGuard guard(context);

    // NB: assumes devices involved support UVA
    check_cuda(cudaMemcpyAsync(dest, src, n, cudaMemcpyDefault, get_current_stream()));
}

__global__ void memset_kernel(int *dest, int value, size_t n) {
    const size_t tid = grid_index();

    if (tid < n) {
        dest[tid] = value;
    }
}

void memset_device(void *context, void *dest, int value, size_t n) {
    ContextGuard guard(context);

    if ((n % 4) > 0) {
        // for unaligned lengths fallback to CUDA memset
        check_cuda(cudaMemsetAsync(dest, value, n, get_current_stream()));
    } else {
        // custom kernel to support 4-byte values (and slightly lower host overhead)
        const size_t num_words = n / 4;
        wp_launch_device(WP_CURRENT_CONTEXT, memset_kernel, num_words, ((int *)dest, value, num_words))
    }
}

// fill memory buffer with a value: generic memtile kernel using memcpy for each element
__global__ void memtile_kernel(void *dst, const void *src, size_t srcsize, size_t n) {
    size_t tid = grid_index();
    if (tid < n) {
        memcpy((int8_t *)dst + srcsize * tid, src, srcsize);
    }
}

// this should be faster than memtile_kernel, but requires proper alignment of dst
template<typename T>
__global__ void memtile_value_kernel(T *dst, T value, size_t n) {
    size_t tid = grid_index();
    if (tid < n) {
        dst[tid] = value;
    }
}

void memtile_device(void *context, void *dst, const void *src, size_t srcsize, size_t n) {
    ContextGuard guard(context);

    auto dst_addr = reinterpret_cast<size_t>(dst);
    auto src_addr = reinterpret_cast<size_t>(src);

    // try memtile_value first because it should be faster, but we need to ensure proper alignment
    if (srcsize == 8 && (dst_addr & 7) == 0 && (src_addr & 7) == 0) {
        auto *p = reinterpret_cast<int64_t *>(dst);
        int64_t value = *reinterpret_cast<const int64_t *>(src);
        wp_launch_device(WP_CURRENT_CONTEXT, memtile_value_kernel, n, (p, value, n))
    } else if (srcsize == 4 && (dst_addr & 3) == 0 && (src_addr & 3) == 0) {
        auto *p = reinterpret_cast<int32_t *>(dst);
        int32_t value = *reinterpret_cast<const int32_t *>(src);
        wp_launch_device(WP_CURRENT_CONTEXT, memtile_value_kernel, n, (p, value, n))
    } else if (srcsize == 2 && (dst_addr & 1) == 0 && (src_addr & 1) == 0) {
        auto *p = reinterpret_cast<int16_t *>(dst);
        int16_t value = *reinterpret_cast<const int16_t *>(src);
        wp_launch_device(WP_CURRENT_CONTEXT, memtile_value_kernel, n, (p, value, n))
    } else if (srcsize == 1) {
        check_cuda(cudaMemset(dst, *reinterpret_cast<const int8_t *>(src), n));
    } else {
        // generic version

        // TODO: use a persistent stream-local staging buffer to avoid allocs?
        void *src_device;
        check_cuda(cudaMalloc(&src_device, srcsize));
        check_cuda(cudaMemcpyAsync(src_device, src, srcsize, cudaMemcpyHostToDevice, get_current_stream()));

        wp_launch_device(WP_CURRENT_CONTEXT, memtile_kernel, n, (dst, src_device, srcsize, n))

            check_cuda(cudaFree(src_device));
    }
}

static __global__ void array_copy_1d_kernel(void *dst, const void *src,
                                            int dst_stride, int src_stride,
                                            const int *dst_indices, const int *src_indices,
                                            int n, int elem_size) {
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    if (i < n) {
        int src_idx = src_indices ? src_indices[i] : i;
        int dst_idx = dst_indices ? dst_indices[i] : i;
        const char *p = (const char *)src + src_idx * src_stride;
        char *q = (char *)dst + dst_idx * dst_stride;
        memcpy(q, p, elem_size);
    }
}

static __global__ void array_copy_2d_kernel(void *dst, const void *src,
                                            wp::vec_t<2, int> dst_strides, wp::vec_t<2, int> src_strides,
                                            wp::vec_t<2, const int *> dst_indices, wp::vec_t<2, const int *> src_indices,
                                            wp::vec_t<2, int> shape, int elem_size) {
    int tid = blockIdx.x * blockDim.x + threadIdx.x;
    int n = shape[1];
    int i = tid / n;
    int j = tid % n;
    if (i < shape[0] /*&& j < shape[1]*/) {
        int src_idx0 = src_indices[0] ? src_indices[0][i] : i;
        int dst_idx0 = dst_indices[0] ? dst_indices[0][i] : i;
        int src_idx1 = src_indices[1] ? src_indices[1][j] : j;
        int dst_idx1 = dst_indices[1] ? dst_indices[1][j] : j;
        const char *p = (const char *)src + src_idx0 * src_strides[0] + src_idx1 * src_strides[1];
        char *q = (char *)dst + dst_idx0 * dst_strides[0] + dst_idx1 * dst_strides[1];
        memcpy(q, p, elem_size);
    }
}

static __global__ void array_copy_3d_kernel(void *dst, const void *src,
                                            wp::vec_t<3, int> dst_strides, wp::vec_t<3, int> src_strides,
                                            wp::vec_t<3, const int *> dst_indices, wp::vec_t<3, const int *> src_indices,
                                            wp::vec_t<3, int> shape, int elem_size) {
    int tid = blockIdx.x * blockDim.x + threadIdx.x;
    int n = shape[1];
    int o = shape[2];
    int i = tid / (n * o);
    int j = tid % (n * o) / o;
    int k = tid % o;
    if (i < shape[0] && j < shape[1] /*&& k < shape[2]*/) {
        int src_idx0 = src_indices[0] ? src_indices[0][i] : i;
        int dst_idx0 = dst_indices[0] ? dst_indices[0][i] : i;
        int src_idx1 = src_indices[1] ? src_indices[1][j] : j;
        int dst_idx1 = dst_indices[1] ? dst_indices[1][j] : j;
        int src_idx2 = src_indices[2] ? src_indices[2][k] : k;
        int dst_idx2 = dst_indices[2] ? dst_indices[2][k] : k;
        const char *p = (const char *)src + src_idx0 * src_strides[0] + src_idx1 * src_strides[1] + src_idx2 * src_strides[2];
        char *q = (char *)dst + dst_idx0 * dst_strides[0] + dst_idx1 * dst_strides[1] + dst_idx2 * dst_strides[2];
        memcpy(q, p, elem_size);
    }
}

static __global__ void array_copy_4d_kernel(void *dst, const void *src,
                                            wp::vec_t<4, int> dst_strides, wp::vec_t<4, int> src_strides,
                                            wp::vec_t<4, const int *> dst_indices, wp::vec_t<4, const int *> src_indices,
                                            wp::vec_t<4, int> shape, int elem_size) {
    int tid = blockIdx.x * blockDim.x + threadIdx.x;
    int n = shape[1];
    int o = shape[2];
    int p = shape[3];
    int i = tid / (n * o * p);
    int j = tid % (n * o * p) / (o * p);
    int k = tid % (o * p) / p;
    int l = tid % p;
    if (i < shape[0] && j < shape[1] && k < shape[2] /*&& l < shape[3]*/) {
        int src_idx0 = src_indices[0] ? src_indices[0][i] : i;
        int dst_idx0 = dst_indices[0] ? dst_indices[0][i] : i;
        int src_idx1 = src_indices[1] ? src_indices[1][j] : j;
        int dst_idx1 = dst_indices[1] ? dst_indices[1][j] : j;
        int src_idx2 = src_indices[2] ? src_indices[2][k] : k;
        int dst_idx2 = dst_indices[2] ? dst_indices[2][k] : k;
        int src_idx3 = src_indices[3] ? src_indices[3][l] : l;
        int dst_idx3 = dst_indices[3] ? dst_indices[3][l] : l;
        const char *p = (const char *)src + src_idx0 * src_strides[0] + src_idx1 * src_strides[1] + src_idx2 * src_strides[2] + src_idx3 * src_strides[3];
        char *q = (char *)dst + dst_idx0 * dst_strides[0] + dst_idx1 * dst_strides[1] + dst_idx2 * dst_strides[2] + dst_idx3 * dst_strides[3];
        memcpy(q, p, elem_size);
    }
}

size_t array_copy_device(void *context, void *dst, void *src, int dst_type, int src_type, int elem_size) {
    if (!src || !dst)
        return 0;

    const void *src_data = nullptr;
    const void *src_grad = nullptr;
    void *dst_data = nullptr;
    void *dst_grad = nullptr;
    int src_ndim = 0;
    int dst_ndim = 0;
    const int *src_shape = nullptr;
    const int *dst_shape = nullptr;
    const int *src_strides = nullptr;
    const int *dst_strides = nullptr;
    const int *const *src_indices = nullptr;
    const int *const *dst_indices = nullptr;

    const int *null_indices[wp::ARRAY_MAX_DIMS] = {nullptr};

    if (src_type == wp::ARRAY_TYPE_REGULAR) {
        const wp::array_t<void> &src_arr = *static_cast<const wp::array_t<void> *>(src);
        src_data = src_arr.data;
        src_grad = src_arr.grad;
        src_ndim = src_arr.ndim;
        src_shape = src_arr.shape.dims;
        src_strides = src_arr.strides;
        src_indices = null_indices;
    } else if (src_type == wp::ARRAY_TYPE_INDEXED) {
        const wp::indexedarray_t<void> &src_arr = *static_cast<const wp::indexedarray_t<void> *>(src);
        src_data = src_arr.arr.data;
        src_ndim = src_arr.arr.ndim;
        src_shape = src_arr.shape.dims;
        src_strides = src_arr.arr.strides;
        src_indices = src_arr.indices;
    } else {
        fprintf(stderr, "Warp error: Invalid array type (%d)\n", src_type);
        return 0;
    }

    if (dst_type == wp::ARRAY_TYPE_REGULAR) {
        const wp::array_t<void> &dst_arr = *static_cast<const wp::array_t<void> *>(dst);
        dst_data = dst_arr.data;
        dst_grad = dst_arr.grad;
        dst_ndim = dst_arr.ndim;
        dst_shape = dst_arr.shape.dims;
        dst_strides = dst_arr.strides;
        dst_indices = null_indices;
    } else if (dst_type == wp::ARRAY_TYPE_INDEXED) {
        const wp::indexedarray_t<void> &dst_arr = *static_cast<const wp::indexedarray_t<void> *>(dst);
        dst_data = dst_arr.arr.data;
        dst_ndim = dst_arr.arr.ndim;
        dst_shape = dst_arr.shape.dims;
        dst_strides = dst_arr.arr.strides;
        dst_indices = dst_arr.indices;
    } else {
        fprintf(stderr, "Warp error: Invalid array type (%d)\n", dst_type);
        return 0;
    }

    if (src_ndim != dst_ndim) {
        fprintf(stderr, "Warp error: Incompatible array dimensionalities (%d and %d)\n", src_ndim, dst_ndim);
        return 0;
    }

    bool has_grad = (src_grad && dst_grad);
    size_t n = 1;

    for (int i = 0; i < src_ndim; i++) {
        if (src_shape[i] != dst_shape[i]) {
            fprintf(stderr, "Warp error: Incompatible array shapes\n");
            return 0;
        }
        n *= src_shape[i];
    }

    ContextGuard guard(context);

    switch (src_ndim) {
        case 1: {
            wp_launch_device(WP_CURRENT_CONTEXT, array_copy_1d_kernel, n, (dst_data, src_data, dst_strides[0], src_strides[0], dst_indices[0], src_indices[0], src_shape[0], elem_size)) if (has_grad) {
                wp_launch_device(WP_CURRENT_CONTEXT, array_copy_1d_kernel, n, (dst_grad, src_grad, dst_strides[0], src_strides[0], dst_indices[0], src_indices[0], src_shape[0], elem_size))
            }
            break;
        }
        case 2: {
            wp::vec_t<2, int> shape_v(src_shape[0], src_shape[1]);
            wp::vec_t<2, int> src_strides_v(src_strides[0], src_strides[1]);
            wp::vec_t<2, int> dst_strides_v(dst_strides[0], dst_strides[1]);
            wp::vec_t<2, const int *> src_indices_v(src_indices[0], src_indices[1]);
            wp::vec_t<2, const int *> dst_indices_v(dst_indices[0], dst_indices[1]);

            wp_launch_device(WP_CURRENT_CONTEXT, array_copy_2d_kernel, n, (dst_data, src_data, dst_strides_v, src_strides_v, dst_indices_v, src_indices_v, shape_v, elem_size)) if (has_grad) {
                wp_launch_device(WP_CURRENT_CONTEXT, array_copy_2d_kernel, n, (dst_grad, src_grad, dst_strides_v, src_strides_v, dst_indices_v, src_indices_v, shape_v, elem_size))
            }
            break;
        }
        case 3: {
            wp::vec_t<3, int> shape_v(src_shape[0], src_shape[1], src_shape[2]);
            wp::vec_t<3, int> src_strides_v(src_strides[0], src_strides[1], src_strides[2]);
            wp::vec_t<3, int> dst_strides_v(dst_strides[0], dst_strides[1], dst_strides[2]);
            wp::vec_t<3, const int *> src_indices_v(src_indices[0], src_indices[1], src_indices[2]);
            wp::vec_t<3, const int *> dst_indices_v(dst_indices[0], dst_indices[1], dst_indices[2]);

            wp_launch_device(WP_CURRENT_CONTEXT, array_copy_3d_kernel, n, (dst_data, src_data, dst_strides_v, src_strides_v, dst_indices_v, src_indices_v, shape_v, elem_size)) if (has_grad) {
                wp_launch_device(WP_CURRENT_CONTEXT, array_copy_3d_kernel, n, (dst_grad, src_grad, dst_strides_v, src_strides_v, dst_indices_v, src_indices_v, shape_v, elem_size))
            }
            break;
        }
        case 4: {
            wp::vec_t<4, int> shape_v(src_shape[0], src_shape[1], src_shape[2], src_shape[3]);
            wp::vec_t<4, int> src_strides_v(src_strides[0], src_strides[1], src_strides[2], src_strides[3]);
            wp::vec_t<4, int> dst_strides_v(dst_strides[0], dst_strides[1], dst_strides[2], dst_strides[3]);
            wp::vec_t<4, const int *> src_indices_v(src_indices[0], src_indices[1], src_indices[2], src_indices[3]);
            wp::vec_t<4, const int *> dst_indices_v(dst_indices[0], dst_indices[1], dst_indices[2], dst_indices[3]);

            wp_launch_device(WP_CURRENT_CONTEXT, array_copy_4d_kernel, n, (dst_data, src_data, dst_strides_v, src_strides_v, dst_indices_v, src_indices_v, shape_v, elem_size)) if (has_grad) {
                wp_launch_device(WP_CURRENT_CONTEXT, array_copy_4d_kernel, n, (dst_grad, src_grad, dst_strides_v, src_strides_v, dst_indices_v, src_indices_v, shape_v, elem_size))
            }
            break;
        }
        default:
            fprintf(stderr, "Warp error: invalid array dimensionality (%d)\n", src_ndim);
            return 0;
    }

    if (check_cuda(cudaGetLastError()))
        return n;
    else
        return 0;
}

static __global__ void array_fill_1d_kernel(void *data,
                                            int n,
                                            int stride,
                                            const int *indices,
                                            const void *value,
                                            int value_size) {
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    if (i < n) {
        int idx = indices ? indices[i] : i;
        char *p = (char *)data + idx * stride;
        memcpy(p, value, value_size);
    }
}

static __global__ void array_fill_2d_kernel(void *data,
                                            wp::vec_t<2, int> shape,
                                            wp::vec_t<2, int> strides,
                                            wp::vec_t<2, const int *> indices,
                                            const void *value,
                                            int value_size) {
    int tid = blockIdx.x * blockDim.x + threadIdx.x;
    int n = shape[1];
    int i = tid / n;
    int j = tid % n;
    if (i < shape[0] /*&& j < shape[1]*/) {
        int idx0 = indices[0] ? indices[0][i] : i;
        int idx1 = indices[1] ? indices[1][j] : j;
        char *p = (char *)data + idx0 * strides[0] + idx1 * strides[1];
        memcpy(p, value, value_size);
    }
}

static __global__ void array_fill_3d_kernel(void *data,
                                            wp::vec_t<3, int> shape,
                                            wp::vec_t<3, int> strides,
                                            wp::vec_t<3, const int *> indices,
                                            const void *value,
                                            int value_size) {
    int tid = blockIdx.x * blockDim.x + threadIdx.x;
    int n = shape[1];
    int o = shape[2];
    int i = tid / (n * o);
    int j = tid % (n * o) / o;
    int k = tid % o;
    if (i < shape[0] && j < shape[1] /*&& k < shape[2]*/) {
        int idx0 = indices[0] ? indices[0][i] : i;
        int idx1 = indices[1] ? indices[1][j] : j;
        int idx2 = indices[2] ? indices[2][k] : k;
        char *p = (char *)data + idx0 * strides[0] + idx1 * strides[1] + idx2 * strides[2];
        memcpy(p, value, value_size);
    }
}

static __global__ void array_fill_4d_kernel(void *data,
                                            wp::vec_t<4, int> shape,
                                            wp::vec_t<4, int> strides,
                                            wp::vec_t<4, const int *> indices,
                                            const void *value,
                                            int value_size) {
    int tid = blockIdx.x * blockDim.x + threadIdx.x;
    int n = shape[1];
    int o = shape[2];
    int p = shape[3];
    int i = tid / (n * o * p);
    int j = tid % (n * o * p) / (o * p);
    int k = tid % (o * p) / p;
    int l = tid % p;
    if (i < shape[0] && j < shape[1] && k < shape[2] /*&& l < shape[3]*/) {
        int idx0 = indices[0] ? indices[0][i] : i;
        int idx1 = indices[1] ? indices[1][j] : j;
        int idx2 = indices[2] ? indices[2][k] : k;
        int idx3 = indices[3] ? indices[3][l] : l;
        char *p = (char *)data + idx0 * strides[0] + idx1 * strides[1] + idx2 * strides[2] + idx3 * strides[3];
        memcpy(p, value, value_size);
    }
}

void array_fill_device(void *context, void *arr_ptr, int arr_type, const void *value_ptr, int value_size) {
    if (!arr_ptr || !value_ptr)
        return;

    void *data = nullptr;
    int ndim = 0;
    const int *shape = nullptr;
    const int *strides = nullptr;
    const int *const *indices = nullptr;

    const int *null_indices[wp::ARRAY_MAX_DIMS] = {nullptr};

    if (arr_type == wp::ARRAY_TYPE_REGULAR) {
        wp::array_t<void> &arr = *static_cast<wp::array_t<void> *>(arr_ptr);
        data = arr.data;
        ndim = arr.ndim;
        shape = arr.shape.dims;
        strides = arr.strides;
        indices = null_indices;
    } else if (arr_type == wp::ARRAY_TYPE_INDEXED) {
        wp::indexedarray_t<void> &ia = *static_cast<wp::indexedarray_t<void> *>(arr_ptr);
        data = ia.arr.data;
        ndim = ia.arr.ndim;
        shape = ia.shape.dims;
        strides = ia.arr.strides;
        indices = ia.indices;
    } else {
        fprintf(stderr, "Warp error: Invalid array type id %d\n", arr_type);
        return;
    }

    size_t n = 1;
    for (int i = 0; i < ndim; i++)
        n *= shape[i];

    ContextGuard guard(context);

    // copy value to device memory
    void *value_devptr;
    check_cuda(cudaMalloc(&value_devptr, value_size));
    check_cuda(cudaMemcpyAsync(value_devptr, value_ptr, value_size, cudaMemcpyHostToDevice, get_current_stream()));

    switch (ndim) {
        case 1: {
            wp_launch_device(WP_CURRENT_CONTEXT, array_fill_1d_kernel, n,
                             (data, shape[0], strides[0], indices[0], value_devptr, value_size)) break;
        }
        case 2: {
            wp::vec_t<2, int> shape_v(shape[0], shape[1]);
            wp::vec_t<2, int> strides_v(strides[0], strides[1]);
            wp::vec_t<2, const int *> indices_v(indices[0], indices[1]);
            wp_launch_device(WP_CURRENT_CONTEXT, array_fill_2d_kernel, n,
                             (data, shape_v, strides_v, indices_v, value_devptr, value_size)) break;
        }
        case 3: {
            wp::vec_t<3, int> shape_v(shape[0], shape[1], shape[2]);
            wp::vec_t<3, int> strides_v(strides[0], strides[1], strides[2]);
            wp::vec_t<3, const int *> indices_v(indices[0], indices[1], indices[2]);
            wp_launch_device(WP_CURRENT_CONTEXT, array_fill_3d_kernel, n,
                             (data, shape_v, strides_v, indices_v, value_devptr, value_size)) break;
        }
        case 4: {
            wp::vec_t<4, int> shape_v(shape[0], shape[1], shape[2], shape[3]);
            wp::vec_t<4, int> strides_v(strides[0], strides[1], strides[2], strides[3]);
            wp::vec_t<4, const int *> indices_v(indices[0], indices[1], indices[2], indices[3]);
            wp_launch_device(WP_CURRENT_CONTEXT, array_fill_4d_kernel, n,
                             (data, shape_v, strides_v, indices_v, value_devptr, value_size)) break;
        }
        default:
            fprintf(stderr, "Warp error: invalid array dimensionality (%d)\n", ndim);
            return;
    }
}

void array_scan_int_device(uint64_t in, uint64_t out, int len, bool inclusive) {
    scan_device((const int *)in, (int *)out, len, inclusive);
}

void array_scan_float_device(uint64_t in, uint64_t out, int len, bool inclusive) {
    scan_device((const float *)in, (float *)out, len, inclusive);
}

int cuda_driver_version() {
    int version;
    if (check_cu(cuDriverGetVersion(&version)))
        return version;
    else
        return 0;
}

int cuda_toolkit_version() {
    return CUDA_VERSION;
}

int cuda_device_get_count() {
    int count = 0;
    check_cu(cuDeviceGetCount(&count));
    return count;
}

void *cuda_device_primary_context_retain(int ordinal) {
    CUcontext context = nullptr;
    CUdevice device;
    if (check_cu(cuDeviceGet(&device, ordinal)))
        check_cu(cuDevicePrimaryCtxRetain(&context, device));
    return context;
}

void cuda_device_primary_context_release(int ordinal) {
    CUdevice device;
    if (check_cu(cuDeviceGet(&device, ordinal)))
        check_cu(cuDevicePrimaryCtxRelease(device));
}

const char *cuda_device_get_name(int ordinal) {
    if (ordinal >= 0 && ordinal < int(g_devices.size()))
        return g_devices[ordinal].name;
    return nullptr;
}

int cuda_device_get_arch(int ordinal) {
    if (ordinal >= 0 && ordinal < int(g_devices.size()))
        return g_devices[ordinal].arch;
    return 0;
}

int cuda_device_is_uva(int ordinal) {
    if (ordinal >= 0 && ordinal < int(g_devices.size()))
        return g_devices[ordinal].is_uva;
    return 0;
}

void *cuda_context_get_current() {
    return get_current_context();
}

void cuda_context_set_current(void *context) {
    auto ctx = static_cast<CUcontext>(context);
    CUcontext prev_ctx = nullptr;
    check_cu(cuCtxGetCurrent(&prev_ctx));
    if (ctx != prev_ctx) {
        check_cu(cuCtxSetCurrent(ctx));
    }
}

void cuda_context_push_current(void *context) {
    check_cu(cuCtxPushCurrent(static_cast<CUcontext>(context)));
}

void cuda_context_pop_current() {
    CUcontext context;
    check_cu(cuCtxPopCurrent(&context));
}

void *cuda_context_create(int device_ordinal) {
    CUcontext ctx = nullptr;
    CUdevice device;
    if (check_cu(cuDeviceGet(&device, device_ordinal)))
        check_cu(cuCtxCreate(&ctx, 0, device));
    return ctx;
}

void cuda_context_destroy(void *context) {
    if (context) {
        auto ctx = static_cast<CUcontext>(context);

        // ensure this is not the current context
        if (ctx == cuda_context_get_current())
            cuda_context_set_current(nullptr);

        // release the cached info about this context
        ContextInfo *info = get_context_info(ctx);
        if (info) {
            if (info->stream)
                check_cu(cuStreamDestroy(info->stream));

            g_contexts.erase(ctx);
        }

        check_cu(cuCtxDestroy(ctx));
    }
}

void cuda_context_synchronize(void *context) {
    ContextGuard guard(context);

    check_cu(cuCtxSynchronize());
}

uint64_t cuda_context_check(void *context) {
    ContextGuard guard(context);

    cudaStreamCaptureStatus status;
    cudaStreamIsCapturing(get_current_stream(), &status);

    // do not check during cuda stream capture
    // since we cannot synchronize the device
    if (status == cudaStreamCaptureStatusNone) {
        cudaDeviceSynchronize();
        return cudaPeekAtLastError();
    } else {
        return 0;
    }
}

int cuda_context_get_device_ordinal(void *context) {
    ContextInfo *info = get_context_info(static_cast<CUcontext>(context));
    return info && info->device_info ? info->device_info->ordinal : -1;
}

int cuda_context_is_primary(void *context) {
    int ordinal = cuda_context_get_device_ordinal(context);
    if (ordinal != -1) {
        // there is no CUDA API to check if a context is primary, but we can temporarily
        // acquire the device's primary context to check the pointer
        void *device_primary_context = cuda_device_primary_context_retain(ordinal);
        cuda_device_primary_context_release(ordinal);
        return int(context == device_primary_context);
    }
    return 0;
}

void *cuda_context_get_stream(void *context) {
    ContextInfo *info = get_context_info(static_cast<CUcontext>(context));
    if (info) {
        return info->stream;
    }
    return nullptr;
}

void cuda_context_set_stream(void *context, void *stream) {
    ContextInfo *info = get_context_info(static_cast<CUcontext>(context));
    if (info) {
        info->stream = static_cast<CUstream>(stream);
    }
}

int cuda_context_enable_peer_access(void *context, void *peer_context) {
    if (!context || !peer_context) {
        fprintf(stderr, "Warp error: Failed to enable peer access: invalid argument\n");
        return 0;
    }

    if (context == peer_context)
        return 1;// ok

    auto ctx = static_cast<CUcontext>(context);
    auto peer_ctx = static_cast<CUcontext>(peer_context);

    ContextInfo *info = get_context_info(ctx);
    ContextInfo *peer_info = get_context_info(peer_ctx);
    if (!info || !peer_info) {
        fprintf(stderr, "Warp error: Failed to enable peer access: failed to get context info\n");
        return 0;
    }

    // check if same device
    if (info->device_info == peer_info->device_info) {
        if (info->device_info->is_uva) {
            return 1;// ok
        } else {
            fprintf(stderr, "Warp error: Failed to enable peer access: device doesn't support UVA\n");
            return 0;
        }
    } else {
        // different devices, try to enable
        ContextGuard guard(ctx, true);
        CUresult result = cuCtxEnablePeerAccess(peer_ctx, 0);
        if (result == CUDA_SUCCESS || result == CUDA_ERROR_PEER_ACCESS_ALREADY_ENABLED) {
            return 1;// ok
        } else {
            check_cu(result);
            return 0;
        }
    }
}

int cuda_context_can_access_peer(void *context, void *peer_context) {
    if (!context || !peer_context)
        return 0;

    if (context == peer_context)
        return 1;

    auto ctx = static_cast<CUcontext>(context);
    auto peer_ctx = static_cast<CUcontext>(peer_context);

    ContextInfo *info = get_context_info(ctx);
    ContextInfo *peer_info = get_context_info(peer_ctx);
    if (!info || !peer_info)
        return 0;

    // check if same device
    if (info->device_info == peer_info->device_info) {
        if (info->device_info->is_uva)
            return 1;
        else
            return 0;
    } else {
        // different devices, try to enable
        // TODO: is there a better way to check?
        ContextGuard guard(ctx, true);
        CUresult result = cuCtxEnablePeerAccess(peer_ctx, 0);
        if (result == CUDA_SUCCESS || result == CUDA_ERROR_PEER_ACCESS_ALREADY_ENABLED)
            return 1;
        else
            return 0;
    }
}

void *cuda_stream_create(void *context) {
    CUcontext ctx = context ? static_cast<CUcontext>(context) : get_current_context();
    if (!ctx)
        return nullptr;

    ContextGuard guard(context, true);

    CUstream stream;
    if (check_cu(cuStreamCreate(&stream, CU_STREAM_DEFAULT)))
        return stream;
    else
        return nullptr;
}

void cuda_stream_destroy(void *context, void *stream) {
    if (!stream)
        return;

    CUcontext ctx = context ? static_cast<CUcontext>(context) : get_current_context();
    if (!ctx)
        return;

    ContextGuard guard(context, true);

    check_cu(cuStreamDestroy(static_cast<CUstream>(stream)));
}

void cuda_stream_synchronize(void *context, void *stream) {
    ContextGuard guard(context);

    check_cu(cuStreamSynchronize(static_cast<CUstream>(stream)));
}

void *cuda_stream_get_current() {
    return get_current_stream();
}

void cuda_stream_wait_event(void *context, void *stream, void *event) {
    ContextGuard guard(context);

    check_cu(cuStreamWaitEvent(static_cast<CUstream>(stream), static_cast<CUevent>(event), 0));
}

void cuda_stream_wait_stream(void *context, void *stream, void *other_stream, void *event) {
    ContextGuard guard(context);

    check_cu(cuEventRecord(static_cast<CUevent>(event), static_cast<CUstream>(other_stream)));
    check_cu(cuStreamWaitEvent(static_cast<CUstream>(stream), static_cast<CUevent>(event), 0));
}

void *cuda_event_create(void *context, unsigned flags) {
    ContextGuard guard(context);

    CUevent event;
    if (check_cu(cuEventCreate(&event, flags)))
        return event;
    else
        return nullptr;
}

void cuda_event_destroy(void *context, void *event) {
    ContextGuard guard(context, true);

    check_cu(cuEventDestroy(static_cast<CUevent>(event)));
}

void cuda_event_record(void *context, void *event, void *stream) {
    ContextGuard guard(context);

    check_cu(cuEventRecord(static_cast<CUevent>(event), static_cast<CUstream>(stream)));
}

void cuda_graph_begin_capture(void *context) {
    ContextGuard guard(context);

    check_cuda(cudaStreamBeginCapture(get_current_stream(), cudaStreamCaptureModeGlobal));
}

void *cuda_graph_end_capture(void *context) {
    ContextGuard guard(context);

    cudaGraph_t graph = nullptr;
    check_cuda(cudaStreamEndCapture(get_current_stream(), &graph));

    if (graph) {
        // enable to create debug GraphVis visualization of graph
        //cudaGraphDebugDotPrint(graph, "graph.dot", cudaGraphDebugDotFlagsVerbose);

        cudaGraphExec_t graph_exec = nullptr;
        check_cuda(cudaGraphInstantiate(&graph_exec, graph, nullptr, nullptr, 0));

        // can use after CUDA 11.4 to permit graphs to capture cudaMallocAsync() operations
        //check_cuda(cudaGraphInstantiateWithFlags(&graph_exec, graph, cudaGraphInstantiateFlagAutoFreeOnLaunch));

        // free source graph
        check_cuda(cudaGraphDestroy(graph));

        return graph_exec;
    } else {
        return nullptr;
    }
}

void cuda_graph_launch(void *context, void *graph_exec) {
    ContextGuard guard(context);

    check_cuda(cudaGraphLaunch((cudaGraphExec_t)graph_exec, get_current_stream()));
}

void cuda_graph_destroy(void *context, void *graph_exec) {
    ContextGuard guard(context);

    check_cuda(cudaGraphExecDestroy((cudaGraphExec_t)graph_exec));
}

void *cuda_get_kernel(void *context, void *module, const char *name) {
    ContextGuard guard(context);

    CUfunction kernel = nullptr;
    if (!check_cu(cuModuleGetFunction(&kernel, (CUmodule)module, name)))
        fprintf(stderr, "Warp CUDA error: Failed to lookup kernel function %s in module\n", name);

    return kernel;
}

size_t cuda_launch_kernel(void *context, void *kernel, size_t dim, void **args) {
    ContextGuard guard(context);

    const int block_dim = 256;
    // CUDA specs up to compute capability 9.0 says the max x-dim grid is 2**31-1, so
    // grid_dim is fine as an int for the near future
    const int grid_dim = (dim + block_dim - 1) / block_dim;

    CUresult res = cuLaunchKernel(
        (CUfunction)kernel,
        grid_dim, 1, 1,
        block_dim, 1, 1,
        0, get_current_stream(),
        args,
        nullptr);

    check_cu(res);

    return res;
}

void cuda_graphics_map(void *context, void *resource) {
    ContextGuard guard(context);

    check_cu(cuGraphicsMapResources(1, (CUgraphicsResource *)resource, get_current_stream()));
}

void cuda_graphics_unmap(void *context, void *resource) {
    ContextGuard guard(context);

    check_cu(cuGraphicsUnmapResources(1, (CUgraphicsResource *)resource, get_current_stream()));
}

void cuda_graphics_device_ptr_and_size(void *context, void *resource, uint64_t *ptr, size_t *size) {
    ContextGuard guard(context);

    CUdeviceptr device_ptr;
    size_t bytes;
    check_cu(cuGraphicsResourceGetMappedPointer(&device_ptr, &bytes, *(CUgraphicsResource *)resource));

    *ptr = device_ptr;
    *size = bytes;
}

void cuda_graphics_unregister_resource(void *context, void *resource) {
    ContextGuard guard(context);

    auto *res = (CUgraphicsResource *)resource;
    check_cu(cuGraphicsUnregisterResource(*res));
    delete res;
}