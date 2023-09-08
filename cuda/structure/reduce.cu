//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "cuda_util.h"
#include "cuda_context.h"
#include "reduce.h"
#include "temp_buffer.h"

#define THRUST_IGNORE_CUB_VERSION_CHECK
#include <cub/device/device_reduce.cuh>
#include <cub/iterator/counting_input_iterator.cuh>

namespace {

template<typename T>
__global__ void cwise_mult_kernel(int len, int stride_a, int stride_b, const T *a, const T *b, T *out) {
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    if (i >= len)
        return;
    out[i] = a[i * stride_a] * b[i * stride_b];
}

/// Custom iterator for allowing strided access with CUB
template<typename T>
struct cub_strided_iterator {
    typedef cub_strided_iterator<T> self_type;
    typedef std::ptrdiff_t difference_type;
    typedef T value_type;
    typedef T *pointer;
    typedef T &reference;

    typedef std::random_access_iterator_tag iterator_category;///< The iterator category

    T *ptr = nullptr;
    int stride = 1;

    __device__ self_type operator++(int) {
        return ++(self_type(*this));
    }

    __device__ self_type &operator++() {
        ptr += stride;
        return *this;
    }

    __host__ __device__ __forceinline__ reference operator*() const {
        return *ptr;
    }

    __device__ self_type operator+(difference_type n) const {
        return self_type(*this) += n;
    }

    __device__ self_type &operator+=(difference_type n) {
        ptr += n * stride;
        return *this;
    }

    __device__ self_type operator-(difference_type n) const {
        return self_type(*this) -= n;
    }

    __device__ self_type &operator-=(difference_type n) {
        ptr -= n * stride;
        return *this;
    }

    __device__ difference_type operator-(const self_type &other) const {
        return (ptr - other.ptr) / stride;
    }

    __device__ reference operator[](difference_type n) const {
        return *(ptr + n * stride);
    }

    __device__ pointer operator->() const {
        return ptr;
    }

    __device__ bool operator==(const self_type &rhs) const {
        return (ptr == rhs.ptr);
    }

    __device__ bool operator!=(const self_type &rhs) const {
        return (ptr != rhs.ptr);
    }
};

template<typename T>
void array_sum_device(const T *ptr_a, T *ptr_out, int count, int byte_stride, int type_length) {
    assert((byte_stride % sizeof(T)) == 0);
    const int stride = byte_stride / sizeof(T);

    void *context = cuda_context_get_current();
    TemporaryBuffer &cub_temp = g_temp_buffer_map[context];

    ContextGuard guard(context);
    auto stream = static_cast<cudaStream_t>(cuda_stream_get_current());

    cub_strided_iterator<const T> ptr_strided{ptr_a, stride};

    size_t buff_size = 0;
    check_cuda(cub::DeviceReduce::Sum(nullptr, buff_size, ptr_strided, ptr_out, count, stream));
    cub_temp.ensure_fits(buff_size);

    for (int k = 0; k < type_length; ++k) {
        cub_strided_iterator<const T> ptr_strided{ptr_a + k, stride};
        check_cuda(cub::DeviceReduce::Sum(cub_temp.buffer, buff_size, ptr_strided, ptr_out + k, count, stream));
    }
}

template<typename T>
void array_sum_device_dispatch(const T *ptr_a, T *ptr_out, int count, int byte_stride, int type_length) {
    using vec2 = wp::vec_t<2, T>;
    using vec3 = wp::vec_t<3, T>;
    using vec4 = wp::vec_t<4, T>;

    // specialized calls for common vector types

    if ((type_length % 4) == 0 && (byte_stride % sizeof(vec4)) == 0) {
        return array_sum_device(reinterpret_cast<const vec4 *>(ptr_a), reinterpret_cast<vec4 *>(ptr_out), count,
                                byte_stride, type_length / 4);
    }

    if ((type_length % 3) == 0 && (byte_stride % sizeof(vec3)) == 0) {
        return array_sum_device(reinterpret_cast<const vec3 *>(ptr_a), reinterpret_cast<vec3 *>(ptr_out), count,
                                byte_stride, type_length / 3);
    }

    if ((type_length % 2) == 0 && (byte_stride % sizeof(vec2)) == 0) {
        return array_sum_device(reinterpret_cast<const vec2 *>(ptr_a), reinterpret_cast<vec2 *>(ptr_out), count,
                                byte_stride, type_length / 2);
    }

    return array_sum_device(ptr_a, ptr_out, count, byte_stride, type_length);
}

template<typename T>
__device__ T element_inner_product(const T &a, const T &b) {
    return a * b;
}

template<unsigned Length, typename T>
__device__ T element_inner_product(const wp::vec_t<Length, T> &a, const wp::vec_t<Length, T> &b) {
    return wp::dot(a, b);
}

/// Custom iterator for allowing strided access with CUB
template<typename ElemT, typename ScalarT>
struct cub_inner_product_iterator {
    typedef cub_inner_product_iterator<ElemT, ScalarT> self_type;
    typedef std::ptrdiff_t difference_type;
    typedef ScalarT value_type;
    typedef ScalarT *pointer;
    typedef ScalarT reference;

    typedef std::random_access_iterator_tag iterator_category;///< The iterator category

    const ElemT *ptr_a = nullptr;
    const ElemT *ptr_b = nullptr;

    int stride_a = 1;
    int stride_b = 1;
    int type_length = 1;

    __device__ self_type operator++(int) {
        return ++(self_type(*this));
    }

    __device__ self_type &operator++() {
        ptr_a += stride_a;
        ptr_b += stride_b;
        return *this;
    }

    __host__ __device__ __forceinline__ reference operator*() const {
        return compute_value(0);
    }

    __device__ self_type operator+(difference_type n) const {
        return self_type(*this) += n;
    }

    __device__ self_type &operator+=(difference_type n) {
        ptr_a += n * stride_a;
        ptr_b += n * stride_b;
        return *this;
    }

    __device__ self_type operator-(difference_type n) const {
        return self_type(*this) -= n;
    }

    __device__ self_type &operator-=(difference_type n) {
        ptr_a -= n * stride_a;
        ptr_b -= n * stride_b;
        return *this;
    }

    __device__ difference_type operator-(const self_type &other) const {
        return (ptr_a - other.ptr_a) / stride_a;
    }

    __device__ reference operator[](difference_type n) const {
        return compute_value(n);
    }

    __device__ bool operator==(const self_type &rhs) const {
        return (ptr_a == rhs.ptr_a);
    }

    __device__ bool operator!=(const self_type &rhs) const {
        return (ptr_a != rhs.ptr_a);
    }

private:
    __device__ ScalarT compute_value(difference_type n) const {
        ScalarT val(0);
        const ElemT *a = ptr_a + n * stride_a;
        const ElemT *b = ptr_b + n * stride_b;
        for (int k = 0; k < type_length; ++k) {
            val += element_inner_product(a[k], b[k]);
        }
        return val;
    }
};

template<typename ElemT, typename ScalarT>
void array_inner_device(const ElemT *ptr_a, const ElemT *ptr_b, ScalarT *ptr_out, int count, int byte_stride_a,
                        int byte_stride_b, int type_length) {
    assert((byte_stride_a % sizeof(ElemT)) == 0);
    assert((byte_stride_b % sizeof(ElemT)) == 0);
    const int stride_a = byte_stride_a / sizeof(ElemT);
    const int stride_b = byte_stride_b / sizeof(ElemT);

    void *context = cuda_context_get_current();
    TemporaryBuffer &cub_temp = g_temp_buffer_map[context];

    ContextGuard guard(context);
    auto stream = static_cast<cudaStream_t>(cuda_stream_get_current());

    cub_inner_product_iterator<ElemT, ScalarT> inner_iterator{ptr_a, ptr_b, stride_a, stride_b, type_length};

    size_t buff_size = 0;
    check_cuda(cub::DeviceReduce::Sum(nullptr, buff_size, inner_iterator, ptr_out, count, stream));
    cub_temp.ensure_fits(buff_size);

    check_cuda(cub::DeviceReduce::Sum(cub_temp.buffer, buff_size, inner_iterator, ptr_out, count, stream));
}

template<typename T>
void array_inner_device_dispatch(const T *ptr_a, const T *ptr_b, T *ptr_out, int count, int byte_stride_a,
                                 int byte_stride_b, int type_length) {
    using vec2 = wp::vec_t<2, T>;
    using vec3 = wp::vec_t<3, T>;
    using vec4 = wp::vec_t<4, T>;

    // specialized calls for common vector types

    if ((type_length % 4) == 0 && (byte_stride_a % sizeof(vec4)) == 0 && (byte_stride_b % sizeof(vec4)) == 0) {
        return array_inner_device(reinterpret_cast<const vec4 *>(ptr_a), reinterpret_cast<const vec4 *>(ptr_b), ptr_out,
                                  count, byte_stride_a, byte_stride_b, type_length / 4);
    }

    if ((type_length % 3) == 0 && (byte_stride_a % sizeof(vec3)) == 0 && (byte_stride_b % sizeof(vec3)) == 0) {
        return array_inner_device(reinterpret_cast<const vec3 *>(ptr_a), reinterpret_cast<const vec3 *>(ptr_b), ptr_out,
                                  count, byte_stride_a, byte_stride_b, type_length / 3);
    }

    if ((type_length % 2) == 0 && (byte_stride_a % sizeof(vec2)) == 0 && (byte_stride_b % sizeof(vec2)) == 0) {
        return array_inner_device(reinterpret_cast<const vec2 *>(ptr_a), reinterpret_cast<const vec2 *>(ptr_b), ptr_out,
                                  count, byte_stride_a, byte_stride_b, type_length / 2);
    }

    return array_inner_device(ptr_a, ptr_b, ptr_out, count, byte_stride_a, byte_stride_b, type_length);
}

}// anonymous namespace

void array_inner_float_device(uint64_t a, uint64_t b, uint64_t out, int count, int byte_stride_a, int byte_stride_b,
                              int type_len) {
    void *context = cuda_context_get_current();

    const auto *ptr_a = (const float *)(a);
    const auto *ptr_b = (const float *)(b);
    auto *ptr_out = (float *)(out);

    array_inner_device_dispatch(ptr_a, ptr_b, ptr_out, count, byte_stride_a, byte_stride_b, type_len);
}

void array_inner_double_device(uint64_t a, uint64_t b, uint64_t out, int count, int byte_stride_a, int byte_stride_b,
                               int type_len) {
    const auto *ptr_a = (const double *)(a);
    const auto *ptr_b = (const double *)(b);
    auto *ptr_out = (double *)(out);

    array_inner_device_dispatch(ptr_a, ptr_b, ptr_out, count, byte_stride_a, byte_stride_b, type_len);
}

void array_sum_float_device(uint64_t a, uint64_t out, int count, int byte_stride, int type_length) {
    const auto *ptr_a = (const float *)(a);
    auto *ptr_out = (float *)(out);
    array_sum_device_dispatch(ptr_a, ptr_out, count, byte_stride, type_length);
}

void array_sum_double_device(uint64_t a, uint64_t out, int count, int byte_stride, int type_length) {
    const auto *ptr_a = (const double *)(a);
    auto *ptr_out = (double *)(out);
    array_sum_device_dispatch(ptr_a, ptr_out, count, byte_stride, type_length);
}
