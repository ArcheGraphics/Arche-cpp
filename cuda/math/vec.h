//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <cstdio>
#include <cstdint>
#include <cuda_runtime_api.h>
#include "initializer_array.h"
#include "constant.h"
#include "cuda_math_utils.h"

namespace wp {

template<unsigned Length, typename Type>
struct vec_t {
    Type c[Length] = {};

    inline vec_t() = default;

    inline __device__ __host__ explicit vec_t(Type s) {
        for (unsigned i = 0; i < Length; ++i) {
            c[i] = s;
        }
    }

    template<typename OtherType>
    inline explicit __device__ __host__ vec_t(const vec_t<Length, OtherType> &other) {
        for (unsigned i = 0; i < Length; ++i) {
            c[i] = other[i];
        }
    }

    inline __device__ __host__ vec_t(Type x, Type y) {
        c[0] = x;
        c[1] = y;
    }

    inline __device__ __host__ vec_t(Type x, Type y, Type z) {
        c[0] = x;
        c[1] = y;
        c[2] = z;
    }

    inline __device__ __host__ vec_t(Type x, Type y, Type z, Type w) {
        c[0] = x;
        c[1] = y;
        c[2] = z;
        c[3] = w;
    }

    inline __device__ __host__ vec_t(const initializer_array<Length, Type> &l) {
        for (unsigned i = 0; i < Length; ++i) {
            c[i] = l[i];
        }
    }

    // special screw vector constructor for spatial_vectors:
    inline __device__ __host__ vec_t(vec_t<3, Type> w, vec_t<3, Type> v) {
        c[0] = w[0];
        c[1] = w[1];
        c[2] = w[2];
        c[3] = v[0];
        c[4] = v[1];
        c[5] = v[2];
    }

    inline __device__ __host__ Type operator[](int index) const {
        return c[index];
    }

    inline __device__ __host__ Type &operator[](int index) {
        return c[index];
    }
};

using vec2b = vec_t<2, int8_t>;
using vec3b = vec_t<3, int8_t>;
using vec4b = vec_t<4, int8_t>;
using vec2ub = vec_t<2, uint8_t>;
using vec3ub = vec_t<3, uint8_t>;
using vec4ub = vec_t<4, uint8_t>;

using vec2s = vec_t<2, int16_t>;
using vec3s = vec_t<3, int16_t>;
using vec4s = vec_t<4, int16_t>;
using vec2us = vec_t<2, uint16_t>;
using vec3us = vec_t<3, uint16_t>;
using vec4us = vec_t<4, uint16_t>;

using vec2i = vec_t<2, int32_t>;
using vec3i = vec_t<3, int32_t>;
using vec4i = vec_t<4, int32_t>;
using vec2ui = vec_t<2, uint32_t>;
using vec3ui = vec_t<3, uint32_t>;
using vec4ui = vec_t<4, uint32_t>;

using vec2l = vec_t<2, int64_t>;
using vec3l = vec_t<3, int64_t>;
using vec4l = vec_t<4, int64_t>;
using vec2ul = vec_t<2, uint64_t>;
using vec3ul = vec_t<3, uint64_t>;
using vec4ul = vec_t<4, uint64_t>;

using vec2 = vec_t<2, float>;
using vec3 = vec_t<3, float>;
using vec4 = vec_t<4, float>;

using vec2f = vec_t<2, float>;
using vec3f = vec_t<3, float>;
using vec4f = vec_t<4, float>;

using vec2d = vec_t<2, double>;
using vec3d = vec_t<3, double>;
using vec4d = vec_t<4, double>;

//--------------
// vec<Length, Type> methods

// Should these accept const references as arguments? It's all
// inlined so maybe it doesn't matter? Even if it does, it
// probably depends on the Length of the vector...

// negation:
template<unsigned Length, typename Type>
inline __device__ vec_t<Length, Type> operator-(vec_t<Length, Type> a) {
    // NB: this constructor will initialize all ret's components to 0, which is
    // unnecessary...
    vec_t<Length, Type> ret;
    for (unsigned i = 0; i < Length; ++i) {
        ret[i] = -a[i];
    }

    // Wonder if this does a load of copying when it returns... hopefully not as it's inlined?
    return ret;
}

template<unsigned Length, typename Type>
__device__ inline vec_t<Length, Type> pos(const vec_t<Length, Type> &x) {
    return x;
}

template<unsigned Length, typename Type>
__device__ inline vec_t<Length, Type> neg(const vec_t<Length, Type> &x) {
    return -x;
}

template<typename Type>
__device__ inline vec_t<3, Type> neg(const vec_t<3, Type> &x) {
    return vec_t<3, Type>(-x.c[0], -x.c[1], -x.c[2]);
}

template<typename Type>
__device__ inline vec_t<2, Type> neg(const vec_t<2, Type> &x) {
    return vec_t<2, Type>(-x.c[0], -x.c[1]);
}

// equality:
template<unsigned Length, typename Type>
inline __device__ bool operator==(const vec_t<Length, Type> &a, const vec_t<Length, Type> &b) {
    for (unsigned i = 0; i < Length; ++i) {
        if (a[i] != b[i]) {
            return false;
        }
    }
    return true;
}

// scalar multiplication:
template<unsigned Length, typename Type>
inline __device__ vec_t<Length, Type> mul(vec_t<Length, Type> a, Type s) {
    vec_t<Length, Type> ret;
    for (unsigned i = 0; i < Length; ++i) {
        ret[i] = a[i] * s;
    }
    return ret;
}

template<typename Type>
inline __device__ vec_t<3, Type> mul(vec_t<3, Type> a, Type s) {
    return vec_t<3, Type>(a.c[0] * s, a.c[1] * s, a.c[2] * s);
}

template<typename Type>
inline __device__ vec_t<2, Type> mul(vec_t<2, Type> a, Type s) {
    return vec_t<2, Type>(a.c[0] * s, a.c[1] * s);
}

template<unsigned Length, typename Type>
inline __device__ vec_t<Length, Type> mul(Type s, vec_t<Length, Type> a) {
    return mul(a, s);
}

template<unsigned Length, typename Type>
inline __device__ vec_t<Length, Type> operator*(Type s, vec_t<Length, Type> a) {
    return mul(a, s);
}

template<unsigned Length, typename Type>
inline __device__ vec_t<Length, Type> operator*(vec_t<Length, Type> a, Type s) {
    return mul(a, s);
}

// component wise multiplication:
template<unsigned Length, typename Type>
inline __device__ vec_t<Length, Type> cw_mul(vec_t<Length, Type> a, vec_t<Length, Type> b) {
    vec_t<Length, Type> ret;
    for (unsigned i = 0; i < Length; ++i) {
        ret[i] = a[i] * b[i];
    }
    return ret;
}

// division
template<unsigned Length, typename Type>
inline __device__ vec_t<Length, Type> div(vec_t<Length, Type> a, Type s) {
    vec_t<Length, Type> ret;
    for (unsigned i = 0; i < Length; ++i) {
        ret[i] = a[i] / s;
    }
    return ret;
}

template<typename Type>
inline __device__ vec_t<3, Type> div(vec_t<3, Type> a, Type s) {
    return vec_t<3, Type>(a.c[0] / s, a.c[1] / s, a.c[2] / s);
}

template<typename Type>
inline __device__ vec_t<2, Type> div(vec_t<2, Type> a, Type s) {
    return vec_t<2, Type>(a.c[0] / s, a.c[1] / s);
}

template<unsigned Length, typename Type>
inline __device__ vec_t<Length, Type> operator/(vec_t<Length, Type> a, Type s) {
    return div(a, s);
}

// component wise division
template<unsigned Length, typename Type>
inline __device__ vec_t<Length, Type> cw_div(vec_t<Length, Type> a, vec_t<Length, Type> b) {
    vec_t<Length, Type> ret;
    for (unsigned i = 0; i < Length; ++i) {
        ret[i] = a[i] / b[i];
    }
    return ret;
}

// addition
template<unsigned Length, typename Type>
inline __device__ vec_t<Length, Type> add(vec_t<Length, Type> a, vec_t<Length, Type> b) {
    vec_t<Length, Type> ret;
    for (unsigned i = 0; i < Length; ++i) {
        ret[i] = a[i] + b[i];
    }
    return ret;
}

template<typename Type>
inline __device__ vec_t<2, Type> add(vec_t<2, Type> a, vec_t<2, Type> b) {
    return vec_t<2, Type>(a.c[0] + b.c[0], a.c[1] + b.c[1]);
}

template<typename Type>
inline __device__ vec_t<3, Type> add(vec_t<3, Type> a, vec_t<3, Type> b) {
    return vec_t<3, Type>(a.c[0] + b.c[0], a.c[1] + b.c[1], a.c[2] + b.c[2]);
}

// subtraction
template<unsigned Length, typename Type>
inline __device__ vec_t<Length, Type> sub(vec_t<Length, Type> a, vec_t<Length, Type> b) {
    vec_t<Length, Type> ret;
    for (unsigned i = 0; i < Length; ++i) {
        ret[i] = Type(a[i] - b[i]);
    }
    return ret;
}

template<typename Type>
inline __device__ vec_t<2, Type> sub(vec_t<2, Type> a, vec_t<2, Type> b) {
    return vec_t<2, Type>(a.c[0] - b.c[0], a.c[1] - b.c[1]);
}

template<typename Type>
inline __device__ vec_t<3, Type> sub(vec_t<3, Type> a, vec_t<3, Type> b) {
    return vec_t<3, Type>(a.c[0] - b.c[0], a.c[1] - b.c[1], a.c[2] - b.c[2]);
}

// dot product:
template<unsigned Length, typename Type>
inline __device__ Type dot(vec_t<Length, Type> a, vec_t<Length, Type> b) {
    Type ret(0);
    for (unsigned i = 0; i < Length; ++i) {
        ret += a[i] * b[i];
    }
    return ret;
}

template<typename Type>
inline __device__ Type dot(vec_t<2, Type> a, vec_t<2, Type> b) {
    return a.c[0] * b.c[0] + a.c[1] * b.c[1];
}

template<typename Type>
inline __device__ Type dot(vec_t<3, Type> a, vec_t<3, Type> b) {
    return a.c[0] * b.c[0] + a.c[1] * b.c[1] + a.c[2] * b.c[2];
}

template<unsigned Length, typename Type>
inline __device__ Type tensordot(vec_t<Length, Type> a, vec_t<Length, Type> b) {
    // corresponds to `np.tensordot()` with all axes being contracted
    return dot(a, b);
}

template<unsigned Length, typename Type>
inline __device__ Type index(const vec_t<Length, Type> &a, int idx) {
#ifndef NDEBUG
    if (idx < 0 || idx >= Length) {
        printf("vec index %d out of bounds at %s %d\n", idx, __FILE__, __LINE__);
    }
#endif

    return a[idx];
}

template<unsigned Length, typename Type>
inline __device__ void indexset(vec_t<Length, Type> &v, int idx, Type value) {
#ifndef NDEBUG
    if (idx < 0 || idx >= Length) {
        printf("vec store %d out of bounds at %s %d\n", idx, __FILE__, __LINE__);
    }
#endif

    v[idx] = value;
}

template<unsigned Length, typename Type>
inline __device__ Type length(vec_t<Length, Type> a) {
    return sqrt(dot(a, a));
}

template<unsigned Length, typename Type>
inline __device__ Type length_sq(vec_t<Length, Type> a) {
    return dot(a, a);
}

template<typename Type>
inline __device__ Type length(vec_t<2, Type> a) {
    return sqrt(a.c[0] * a.c[0] + a.c[1] * a.c[1]);
}

template<typename Type>
inline __device__ Type length(vec_t<3, Type> a) {
    return sqrt(a.c[0] * a.c[0] + a.c[1] * a.c[1] + a.c[2] * a.c[2]);
}

template<unsigned Length, typename Type>
inline __device__ vec_t<Length, Type> normalize(vec_t<Length, Type> a) {
    Type l = length(a);
    if (l > Type(EPSILON))
        return div(a, l);
    else
        return vec_t<Length, Type>();
}

template<typename Type>
inline __device__ vec_t<2, Type> normalize(vec_t<2, Type> a) {
    Type l = sqrt(a.c[0] * a.c[0] + a.c[1] * a.c[1]);
    if (l > Type(EPSILON))
        return vec_t<2, Type>(a.c[0] / l, a.c[1] / l);
    else
        return vec_t<2, Type>();
}

template<typename Type>
inline __device__ vec_t<3, Type> normalize(vec_t<3, Type> a) {
    Type l = sqrt(a.c[0] * a.c[0] + a.c[1] * a.c[1] + a.c[2] * a.c[2]);
    if (l > Type(EPSILON))
        return vec_t<3, Type>(a.c[0] / l, a.c[1] / l, a.c[2] / l);
    else
        return vec_t<3, Type>();
}

template<typename Type>
inline __device__ vec_t<3, Type> cross(vec_t<3, Type> a, vec_t<3, Type> b) {
    return {
        Type(a[1] * b[2] - a[2] * b[1]),
        Type(a[2] * b[0] - a[0] * b[2]),
        Type(a[0] * b[1] - a[1] * b[0])};
}

template<unsigned Length, typename Type>
inline bool __device__ isfinite(vec_t<Length, Type> x) {
    for (unsigned i = 0; i < Length; ++i) {
        if (!isfinite(x[i])) {
            return false;
        }
    }
    return true;
}

// These two functions seem to compile very slowly
template<unsigned Length, typename Type>
inline __device__ vec_t<Length, Type> min(vec_t<Length, Type> a, vec_t<Length, Type> b) {
    vec_t<Length, Type> ret;
    for (unsigned i = 0; i < Length; ++i) {
        ret[i] = a[i] < b[i] ? a[i] : b[i];
    }
    return ret;
}

template<unsigned Length, typename Type>
inline __device__ vec_t<Length, Type> max(vec_t<Length, Type> a, vec_t<Length, Type> b) {
    vec_t<Length, Type> ret;
    for (unsigned i = 0; i < Length; ++i) {
        ret[i] = a[i] > b[i] ? a[i] : b[i];
    }
    return ret;
}

template<unsigned Length, typename Type>
inline __device__ Type min(vec_t<Length, Type> v) {
    Type ret = v[0];
    for (unsigned i = 1; i < Length; ++i) {
        if (v[i] < ret)
            ret = v[i];
    }
    return ret;
}

template<unsigned Length, typename Type>
inline __device__ Type max(vec_t<Length, Type> v) {
    Type ret = v[0];
    for (unsigned i = 1; i < Length; ++i) {
        if (v[i] > ret)
            ret = v[i];
    }
    return ret;
}

template<unsigned Length, typename Type>
inline __device__ unsigned argmin(vec_t<Length, Type> v) {
    unsigned ret = 0;
    for (unsigned i = 1; i < Length; ++i) {
        if (v[i] < v[ret])
            ret = i;
    }
    return ret;
}

template<unsigned Length, typename Type>
inline __device__ unsigned argmax(vec_t<Length, Type> v) {
    unsigned ret = 0;
    for (unsigned i = 1; i < Length; ++i) {
        if (v[i] > v[ret])
            ret = i;
    }
    return ret;
}

template<unsigned Length, typename Type>
inline __device__ void expect_near(const vec_t<Length, Type> &actual, const vec_t<Length, Type> &expected, const Type &tolerance) {
    const Type diff(0);
    for (size_t i = 0; i < Length; ++i) {
        diff = max(diff, abs(actual[i] - expected[i]));
    }
    if (diff > tolerance) {
        printf("Error, expect_near() failed with tolerance ");
        print(tolerance);
        printf("\t Expected: ");
        print(expected);
        printf("\t Actual: ");
        print(actual);
    }
}

// Do I need to specialize these for different lengths?
template<unsigned Length, typename Type>
inline __device__ vec_t<Length, Type> atomic_add(vec_t<Length, Type> *addr, vec_t<Length, Type> value) {

    vec_t<Length, Type> ret;
    for (unsigned i = 0; i < Length; ++i) {
        ret[i] = atomic_add(&(addr->c[i]), value[i]);
    }

    return ret;
}

template<unsigned Length, typename Type>
inline __device__ vec_t<Length, Type> atomic_min(vec_t<Length, Type> *addr, vec_t<Length, Type> value) {

    vec_t<Length, Type> ret;
    for (unsigned i = 0; i < Length; ++i) {
        ret[i] = atomic_min(&(addr->c[i]), value[i]);
    }

    return ret;
}

template<unsigned Length, typename Type>
inline __device__ vec_t<Length, Type> atomic_max(vec_t<Length, Type> *addr, vec_t<Length, Type> value) {

    vec_t<Length, Type> ret;
    for (unsigned i = 0; i < Length; ++i) {
        ret[i] = atomic_max(&(addr->c[i]), value[i]);
    }

    return ret;
}

// ok, the original implementation of this didn't take the absolute values.
// I wouldn't consider this expected behavior. It looks like it's only
// being used for bounding boxes at the moment, where this doesn't matter,
// but you often use it for ray tracing where it does. Not sure if the
// fabs() incurs a performance hit...
template<unsigned Length, typename Type>
__device__ inline int longest_axis(const vec_t<Length, Type> &v) {
    Type lmax = abs(v[0]);
    int ret(0);
    for (unsigned i = 1; i < Length; ++i) {
        Type l = abs(v[i]);
        if (l > lmax) {
            ret = i;
            lmax = l;
        }
    }
    return ret;
}

template<unsigned Length, typename Type>
__device__ inline vec_t<Length, Type> lerp(const vec_t<Length, Type> &a, const vec_t<Length, Type> &b, Type t) {
    return a * (Type(1) - t) + b * t;
}

template<unsigned Length, typename Type>
inline CUDA_CALLABLE void print(vec_t<Length, Type> v) {
    for (unsigned i = 0; i < Length; ++i) {
        printf("%g ", float(v[i]));
    }
    printf("\n");
}

inline CUDA_CALLABLE void expect_near(const vec3 &actual, const vec3 &expected, const float &tolerance) {
    const float diff = max(max(abs(actual[0] - expected[0]), abs(actual[1] - expected[1])), abs(actual[2] - expected[2]));
    if (diff > tolerance) {
        printf("Error, expect_near() failed with tolerance ");
        print(tolerance);
        printf("\t Expected: ");
        print(expected);
        printf("\t Actual: ");
        print(actual);
    }
}

}// namespace wp