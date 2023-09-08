//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "cuda_context.h"
#include "scan.h"
#include "math/array.h"

#include <cstdlib>
#include <cstring>

void *alloc_host(size_t s) {
    return malloc(s);
}

void free_host(void *ptr) {
    free(ptr);
}

void memcpy_h2h(void *dest, void *src, size_t n) {
    memcpy(dest, src, n);
}

void memset_host(void *dest, int value, size_t n) {
    if ((n % 4) > 0) {
        memset(dest, value, n);
    } else {
        const size_t num_words = n / 4;
        for (size_t i = 0; i < num_words; ++i)
            ((int *)dest)[i] = value;
    }
}

// fill memory buffer with a value: this is a faster memtile variant
// for types bigger than one byte, but requires proper alignment of dst
template<typename T>
void memtile_value_host(T *dst, T value, size_t n) {
    while (n--)
        *dst++ = value;
}

void memtile_host(void *dst, const void *src, size_t srcsize, size_t n) {
    auto dst_addr = reinterpret_cast<size_t>(dst);
    auto src_addr = reinterpret_cast<size_t>(src);

    // try memtile_value first because it should be faster, but we need to ensure proper alignment
    if (srcsize == 8 && (dst_addr & 7) == 0 && (src_addr & 7) == 0)
        memtile_value_host(reinterpret_cast<int64_t *>(dst), *reinterpret_cast<const int64_t *>(src), n);
    else if (srcsize == 4 && (dst_addr & 3) == 0 && (src_addr & 3) == 0)
        memtile_value_host(reinterpret_cast<int32_t *>(dst), *reinterpret_cast<const int32_t *>(src), n);
    else if (srcsize == 2 && (dst_addr & 1) == 0 && (src_addr & 1) == 0)
        memtile_value_host(reinterpret_cast<int16_t *>(dst), *reinterpret_cast<const int16_t *>(src), n);
    else if (srcsize == 1)
        memset(dst, *reinterpret_cast<const int8_t *>(src), n);
    else {
        // generic version
        while (n--) {
            memcpy(dst, src, srcsize);
            dst = (int8_t *)dst + srcsize;
        }
    }
}

void array_scan_int_host(uint64_t in, uint64_t out, int len, bool inclusive) {
    scan_host((const int *)in, (int *)out, len, inclusive);
}

void array_scan_float_host(uint64_t in, uint64_t out, int len, bool inclusive) {
    scan_host((const float *)in, (float *)out, len, inclusive);
}

static void array_copy_nd(void *dst, const void *src,
                          const int *dst_strides, const int *src_strides,
                          const int *const *dst_indices, const int *const *src_indices,
                          const int *shape, int ndim, int elem_size) {
    if (ndim == 1) {
        for (int i = 0; i < shape[0]; i++) {
            int src_idx = src_indices[0] ? src_indices[0][i] : i;
            int dst_idx = dst_indices[0] ? dst_indices[0][i] : i;
            const char *p = (const char *)src + src_idx * src_strides[0];
            char *q = (char *)dst + dst_idx * dst_strides[0];
            // copy element
            memcpy(q, p, elem_size);
        }
    } else {
        for (int i = 0; i < shape[0]; i++) {
            int src_idx = src_indices[0] ? src_indices[0][i] : i;
            int dst_idx = dst_indices[0] ? dst_indices[0][i] : i;
            const char *p = (const char *)src + src_idx * src_strides[0];
            char *q = (char *)dst + dst_idx * dst_strides[0];
            // recurse on next inner dimension
            array_copy_nd(q, p, dst_strides + 1, src_strides + 1, dst_indices + 1, src_indices + 1, shape + 1, ndim - 1, elem_size);
        }
    }
}

size_t array_copy_host(void *dst, void *src, int dst_type, int src_type, int elem_size) {
    if (!src || !dst)
        return 0;

    const void *src_data;
    const void *src_grad = nullptr;
    void *dst_data;
    void *dst_grad = nullptr;
    int src_ndim;
    int dst_ndim;
    const int *src_shape;
    const int *dst_shape;
    const int *src_strides;
    const int *dst_strides;
    const int *const *src_indices;
    const int *const *dst_indices;

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

    array_copy_nd(dst_data, src_data,
                  dst_strides, src_strides,
                  dst_indices, src_indices,
                  src_shape, src_ndim, elem_size);

    if (has_grad) {
        array_copy_nd(dst_grad, src_grad,
                      dst_strides, src_strides,
                      dst_indices, src_indices,
                      src_shape, src_ndim, elem_size);
    }

    return n;
}

static void array_fill_strided(void *data, const int *shape, const int *strides, int ndim, const void *value, int value_size) {
    if (ndim == 1) {
        char *p = (char *)data;
        for (int i = 0; i < shape[0]; i++) {
            memcpy(p, value, value_size);
            p += strides[0];
        }
    } else {
        for (int i = 0; i < shape[0]; i++) {
            char *p = (char *)data + i * strides[0];
            // recurse on next inner dimension
            array_fill_strided(p, shape + 1, strides + 1, ndim - 1, value, value_size);
        }
    }
}

static void array_fill_indexed(void *data, const int *shape, const int *strides, const int *const *indices, int ndim, const void *value, int value_size) {
    if (ndim == 1) {
        for (int i = 0; i < shape[0]; i++) {
            int idx = indices[0] ? indices[0][i] : i;
            char *p = (char *)data + idx * strides[0];
            memcpy(p, value, value_size);
        }
    } else {
        for (int i = 0; i < shape[0]; i++) {
            int idx = indices[0] ? indices[0][i] : i;
            char *p = (char *)data + idx * strides[0];
            // recurse on next inner dimension
            array_fill_indexed(p, shape + 1, strides + 1, indices + 1, ndim - 1, value, value_size);
        }
    }
}

void array_fill_host(void *arr_ptr, int arr_type, const void *value_ptr, int value_size) {
    if (!arr_ptr || !value_ptr)
        return;

    if (arr_type == wp::ARRAY_TYPE_REGULAR) {
        wp::array_t<void> &arr = *static_cast<wp::array_t<void> *>(arr_ptr);
        array_fill_strided(arr.data, arr.shape.dims, arr.strides, arr.ndim, value_ptr, value_size);
    } else if (arr_type == wp::ARRAY_TYPE_INDEXED) {
        wp::indexedarray_t<void> &ia = *static_cast<wp::indexedarray_t<void> *>(arr_ptr);
        array_fill_indexed(ia.arr.data, ia.shape.dims, ia.arr.strides, ia.indices, ia.arr.ndim, value_ptr, value_size);
    } else {
        fprintf(stderr, "Warp error: Invalid array type id %d\n", arr_type);
    }
}
