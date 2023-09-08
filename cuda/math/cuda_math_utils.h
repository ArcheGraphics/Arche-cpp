/** Copyright (c) 2022 NVIDIA CORPORATION.  All rights reserved.
* NVIDIA CORPORATION and its licensors retain all intellectual property
* and proprietary rights in and to this software, related documentation
* and any modifications thereto.  Any use, reproduction, disclosure or
* distribution of this software and related documentation without an express
* license agreement from NVIDIA CORPORATION is strictly prohibited.
*/

#pragma once

// All built-in types and functions. To be compatible with runtime NVRTC compilation
// this header must be independently compilable (i.e.: without external SDK headers)
// to achieve this we redefine a subset of CRT functions (printf, pow, sin, cos, etc)

#include <cuda_runtime_api.h>
#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <cassert>
#include <cfloat>
#include <cstring>

#ifdef _WIN32
#define __restrict__ __restrict
#endif

#if !defined(__CUDACC__)
#define CUDA_CALLABLE
#define CUDA_CALLABLE_DEVICE
#else
#define CUDA_CALLABLE __host__ __device__
#define CUDA_CALLABLE_DEVICE __device__
#endif

#ifdef WP_VERIFY_FP
#define FP_CHECK 1
#define DO_IF_FPCHECK(X) \
    { X }
#define DO_IF_NO_FPCHECK(X)
#else
#define FP_CHECK 0
#define DO_IF_FPCHECK(X)
#define DO_IF_NO_FPCHECK(X) \
    { X }
#endif

#define RAD_TO_DEG 57.29577951308232087679
#define DEG_TO_RAD 0.01745329251994329577

#if !defined(__CUDACC__)

// Helper for implementing isfinite()
bool isfinite(double x);

#endif// !__CUDA_ARCH__

namespace wp {

struct half;

CUDA_CALLABLE half float_to_half(float x);
CUDA_CALLABLE float half_to_float(half x);

struct half {
    CUDA_CALLABLE inline half() : u(0) {}

    CUDA_CALLABLE inline half(float f) {
        *this = float_to_half(f);
    }

    unsigned short u;

    CUDA_CALLABLE inline bool operator==(const half &h) const { return u == h.u; }
    CUDA_CALLABLE inline bool operator!=(const half &h) const { return u != h.u; }
    CUDA_CALLABLE inline bool operator>(const half &h) const { return half_to_float(*this) > half_to_float(h); }
    CUDA_CALLABLE inline bool operator>=(const half &h) const { return half_to_float(*this) >= half_to_float(h); }
    CUDA_CALLABLE inline bool operator<(const half &h) const { return half_to_float(*this) < half_to_float(h); }
    CUDA_CALLABLE inline bool operator<=(const half &h) const { return half_to_float(*this) <= half_to_float(h); }

    CUDA_CALLABLE inline bool operator!() const {
        return float(*this) == 0;
    }

    CUDA_CALLABLE inline half operator*=(const half &h) {
        half prod = half(float(*this) * float(h));
        this->u = prod.u;
        return *this;
    }

    CUDA_CALLABLE inline half operator/=(const half &h) {
        half quot = half(float(*this) / float(h));
        this->u = quot.u;
        return *this;
    }

    CUDA_CALLABLE inline half operator+=(const half &h) {
        half sum = half(float(*this) + float(h));
        this->u = sum.u;
        return *this;
    }

    CUDA_CALLABLE inline half operator-=(const half &h) {
        half diff = half(float(*this) - float(h));
        this->u = diff.u;
        return *this;
    }

    CUDA_CALLABLE inline operator float() const { return float(half_to_float(*this)); }
    CUDA_CALLABLE inline operator double() const { return double(half_to_float(*this)); }
    CUDA_CALLABLE inline operator int8_t() const { return int8_t(half_to_float(*this)); }
    CUDA_CALLABLE inline operator uint8_t() const { return uint8_t(half_to_float(*this)); }
    CUDA_CALLABLE inline operator int16_t() const { return int16_t(half_to_float(*this)); }
    CUDA_CALLABLE inline operator uint16_t() const { return uint16_t(half_to_float(*this)); }
    CUDA_CALLABLE inline operator int32_t() const { return int32_t(half_to_float(*this)); }
    CUDA_CALLABLE inline operator uint32_t() const { return uint32_t(half_to_float(*this)); }
    CUDA_CALLABLE inline operator int64_t() const { return int64_t(half_to_float(*this)); }
    CUDA_CALLABLE inline operator uint64_t() const { return uint64_t(half_to_float(*this)); }
};

static_assert(sizeof(half) == 2, "Size of half / float16 type must be 2-bytes");

typedef half float16;

#if __CUDA_ARCH__

CUDA_CALLABLE inline half float_to_half(float x) {
    half h;
    asm("{  cvt.rn.f16.f32 %0, %1;}\n"
        : "=h"(h.u)
        : "f"(x));
    return h;
}

CUDA_CALLABLE inline float half_to_float(half x) {
    float val;
    asm("{  cvt.f32.f16 %0, %1;}\n"
        : "=f"(val)
        : "h"(x.u));
    return val;
}

#else

// adapted from Fabien Giesen's post: https://gist.github.com/rygorous/2156668
inline half float_to_half(float x) {
    union fp32 {
        uint32_t u;
        float f;

        struct
        {
            unsigned int mantissa : 23;
            unsigned int exponent : 8;
            unsigned int sign : 1;
        };
    };

    fp32 f;
    f.f = x;

    fp32 f32infty = {255 << 23};
    fp32 f16infty = {31 << 23};
    fp32 magic = {15 << 23};
    uint32_t sign_mask = 0x80000000u;
    uint32_t round_mask = ~0xfffu;
    half o;

    uint32_t sign = f.u & sign_mask;
    f.u ^= sign;

    // NOTE all the integer compares in this function can be safely
    // compiled into signed compares since all operands are below
    // 0x80000000. Important if you want fast straight SSE2 code
    // (since there's no unsigned PCMPGTD).

    if (f.u >= f32infty.u)                         // Inf or NaN (all exponent bits set)
        o.u = (f.u > f32infty.u) ? 0x7e00 : 0x7c00;// NaN->qNaN and Inf->Inf
    else                                           // (De)normalized number or zero
    {
        f.u &= round_mask;
        f.f *= magic.f;
        f.u -= round_mask;
        if (f.u > f16infty.u) f.u = f16infty.u;// Clamp to signed infinity if overflowed

        o.u = f.u >> 13;// Take the bits!
    }

    o.u |= sign >> 16;
    return o;
}

inline float half_to_float(half h) {
    union fp32 {
        uint32_t u;
        float f;

        struct
        {
            unsigned int mantissa : 23;
            unsigned int exponent : 8;
            unsigned int sign : 1;
        };
    };

    static const fp32 magic = {113 << 23};
    static const uint32_t shifted_exp = 0x7c00 << 13;// exponent mask after shift
    fp32 o;

    o.u = (h.u & 0x7fff) << 13;      // exponent/mantissa bits
    uint32_t exp = shifted_exp & o.u;// just the exponent
    o.u += (127 - 15) << 23;         // exponent adjust

    // handle exponent special cases
    if (exp == shifted_exp)     // Inf/NaN?
        o.u += (128 - 16) << 23;// extra exp adjust
    else if (exp == 0)          // Zero/Denormal?
    {
        o.u += 1 << 23;// extra exp adjust
        o.f -= magic.f;// renormalize
    }

    o.u |= (h.u & 0x8000) << 16;// sign bit
    return o.f;
}

#endif

// BAD operator implementations for fp16 arithmetic...

// negation:
inline CUDA_CALLABLE half operator-(half a) {
    return float_to_half(-half_to_float(a));
}

inline CUDA_CALLABLE half operator+(half a, half b) {
    return float_to_half(half_to_float(a) + half_to_float(b));
}

inline CUDA_CALLABLE half operator-(half a, half b) {
    return float_to_half(half_to_float(a) - half_to_float(b));
}

inline CUDA_CALLABLE half operator*(half a, half b) {
    return float_to_half(half_to_float(a) * half_to_float(b));
}

inline CUDA_CALLABLE half operator*(half a, double b) {
    return float_to_half(half_to_float(a) * b);
}

inline CUDA_CALLABLE half operator*(double a, half b) {
    return float_to_half(a * half_to_float(b));
}

inline CUDA_CALLABLE half operator/(half a, half b) {
    return float_to_half(half_to_float(a) / half_to_float(b));
}

template<typename T>
CUDA_CALLABLE float cast_float(T x) { return (float)(x); }

template<typename T>
CUDA_CALLABLE int cast_int(T x) { return (int)(x); }

// basic ops for integer types
#define DECLARE_INT_OPS(T)                                                    \
    inline CUDA_CALLABLE T mul(T a, T b) { return a * b; }                    \
    inline CUDA_CALLABLE T div(T a, T b) { return a / b; }                    \
    inline CUDA_CALLABLE T add(T a, T b) { return a + b; }                    \
    inline CUDA_CALLABLE T sub(T a, T b) { return a - b; }                    \
    inline CUDA_CALLABLE T mod(T a, T b) { return a % b; }                    \
    inline CUDA_CALLABLE T min(T a, T b) { return a < b ? a : b; }            \
    inline CUDA_CALLABLE T max(T a, T b) { return a > b ? a : b; }            \
    inline CUDA_CALLABLE T clamp(T x, T a, T b) { return min(max(a, x), b); } \
    inline CUDA_CALLABLE T floordiv(T a, T b) { return a / b; }               \
    inline CUDA_CALLABLE T nonzero(T x) { return x == T(0) ? T(0) : T(1); }   \
    inline CUDA_CALLABLE T sqrt(T x) { return 0; }                            \
    inline CUDA_CALLABLE T bit_and(T a, T b) { return a & b; }                \
    inline CUDA_CALLABLE T bit_or(T a, T b) { return a | b; }                 \
    inline CUDA_CALLABLE T bit_xor(T a, T b) { return a ^ b; }                \
    inline CUDA_CALLABLE T lshift(T a, T b) { return a << b; }                \
    inline CUDA_CALLABLE T rshift(T a, T b) { return a >> b; }                \
    inline CUDA_CALLABLE T invert(T x) { return ~x; }                         \
    inline CUDA_CALLABLE bool isfinite(T x) { return true; }

inline CUDA_CALLABLE int8_t abs(int8_t x) { return ::abs(x); }
inline CUDA_CALLABLE int16_t abs(int16_t x) { return ::abs(x); }
inline CUDA_CALLABLE int32_t abs(int32_t x) { return ::abs(x); }
inline CUDA_CALLABLE int64_t abs(int64_t x) { return ::llabs(x); }
inline CUDA_CALLABLE uint8_t abs(uint8_t x) { return x; }
inline CUDA_CALLABLE uint16_t abs(uint16_t x) { return x; }
inline CUDA_CALLABLE uint32_t abs(uint32_t x) { return x; }
inline CUDA_CALLABLE uint64_t abs(uint64_t x) { return x; }

DECLARE_INT_OPS(int8_t)
DECLARE_INT_OPS(int16_t)
DECLARE_INT_OPS(int32_t)
DECLARE_INT_OPS(int64_t)
DECLARE_INT_OPS(uint8_t)
DECLARE_INT_OPS(uint16_t)
DECLARE_INT_OPS(uint32_t)
DECLARE_INT_OPS(uint64_t)

inline CUDA_CALLABLE int8_t step(int8_t x) { return x < 0 ? 1 : 0; }
inline CUDA_CALLABLE int16_t step(int16_t x) { return x < 0 ? 1 : 0; }
inline CUDA_CALLABLE int32_t step(int32_t x) { return x < 0 ? 1 : 0; }
inline CUDA_CALLABLE int64_t step(int64_t x) { return x < 0 ? 1 : 0; }
inline CUDA_CALLABLE uint8_t step(uint8_t x) { return 0; }
inline CUDA_CALLABLE uint16_t step(uint16_t x) { return 0; }
inline CUDA_CALLABLE uint32_t step(uint32_t x) { return 0; }
inline CUDA_CALLABLE uint64_t step(uint64_t x) { return 0; }

inline CUDA_CALLABLE int8_t sign(int8_t x) { return x < 0 ? -1 : 1; }
inline CUDA_CALLABLE int8_t sign(int16_t x) { return x < 0 ? -1 : 1; }
inline CUDA_CALLABLE int8_t sign(int32_t x) { return x < 0 ? -1 : 1; }
inline CUDA_CALLABLE int8_t sign(int64_t x) { return x < 0 ? -1 : 1; }
inline CUDA_CALLABLE uint8_t sign(uint8_t x) { return 1; }
inline CUDA_CALLABLE uint16_t sign(uint16_t x) { return 1; }
inline CUDA_CALLABLE uint32_t sign(uint32_t x) { return 1; }
inline CUDA_CALLABLE uint64_t sign(uint64_t x) { return 1; }

inline bool CUDA_CALLABLE isfinite(half x) {
    return ::isfinite(float(x));
}
inline bool CUDA_CALLABLE isfinite(float x) {
    return ::isfinite(x);
}
inline bool CUDA_CALLABLE isfinite(double x) {
    return ::isfinite(x);
}

inline CUDA_CALLABLE void print(float16 f) {
    printf("%g\n", half_to_float(f));
}

inline CUDA_CALLABLE void print(float f) {
    printf("%g\n", f);
}

inline CUDA_CALLABLE void print(double f) {
    printf("%g\n", f);
}

// basic ops for float types
#define DECLARE_FLOAT_OPS(T)                                                           \
    inline CUDA_CALLABLE T mul(T a, T b) { return a * b; }                             \
    inline CUDA_CALLABLE T add(T a, T b) { return a + b; }                             \
    inline CUDA_CALLABLE T sub(T a, T b) { return a - b; }                             \
    inline CUDA_CALLABLE T min(T a, T b) { return a < b ? a : b; }                     \
    inline CUDA_CALLABLE T max(T a, T b) { return a > b ? a : b; }                     \
    inline CUDA_CALLABLE T sign(T x) { return x < T(0) ? -1 : 1; }                     \
    inline CUDA_CALLABLE T step(T x) { return x < T(0) ? T(1) : T(0); }                \
    inline CUDA_CALLABLE T nonzero(T x) { return x == T(0) ? T(0) : T(1); }            \
    inline CUDA_CALLABLE T clamp(T x, T a, T b) { return min(max(a, x), b); }          \
    inline CUDA_CALLABLE T div(T a, T b) {                                             \
        DO_IF_FPCHECK(                                                                 \
            if (!isfinite(a) || !isfinite(b) || b == T(0)) {                           \
                printf("%s:%d div(%f, %f)\n", __FILE__, __LINE__, float(a), float(b)); \
                assert(0);                                                             \
            })                                                                         \
        return a / b;                                                                  \
    }

DECLARE_FLOAT_OPS(float16)
DECLARE_FLOAT_OPS(float)
DECLARE_FLOAT_OPS(double)

// basic ops for float types
inline CUDA_CALLABLE float16 mod(float16 a, float16 b) {
#if FP_CHECK
    if (!isfinite(a) || !isfinite(b) || float(b) == 0.0f) {
        printf("%s:%d mod(%f, %f)\n", __FILE__, __LINE__, float(a), float(b));
        assert(0);
    }
#endif
    return fmodf(float(a), float(b));
}

inline CUDA_CALLABLE float mod(float a, float b) {
#if FP_CHECK
    if (!isfinite(a) || !isfinite(b) || b == 0.0f) {
        printf("%s:%d mod(%f, %f)\n", __FILE__, __LINE__, a, b);
        assert(0);
    }
#endif
    return fmodf(a, b);
}

inline CUDA_CALLABLE double mod(double a, double b) {
#if FP_CHECK
    if (!isfinite(a) || !isfinite(b) || b == 0.0f) {
        printf("%s:%d mod(%f, %f)\n", __FILE__, __LINE__, a, b);
        assert(0);
    }
#endif
    return fmod(a, b);
}

inline CUDA_CALLABLE half log(half a) {
#if FP_CHECK
    if (!isfinite(a) || float(a) < 0.0f) {
        printf("%s:%d log(%f)\n", __FILE__, __LINE__, float(a));
        assert(0);
    }
#endif
    return ::logf(a);
}

inline CUDA_CALLABLE float log(float a) {
#if FP_CHECK
    if (!isfinite(a) || a < 0.0f) {
        printf("%s:%d log(%f)\n", __FILE__, __LINE__, a);
        assert(0);
    }
#endif
    return ::logf(a);
}

inline CUDA_CALLABLE double log(double a) {
#if FP_CHECK
    if (!isfinite(a) || a < 0.0) {
        printf("%s:%d log(%f)\n", __FILE__, __LINE__, a);
        assert(0);
    }
#endif
    return ::log(a);
}

inline CUDA_CALLABLE half log2(half a) {
#if FP_CHECK
    if (!isfinite(a) || float(a) < 0.0f) {
        printf("%s:%d log2(%f)\n", __FILE__, __LINE__, float(a));
        assert(0);
    }
#endif

    return ::log2f(float(a));
}

inline CUDA_CALLABLE float log2(float a) {
#if FP_CHECK
    if (!isfinite(a) || a < 0.0f) {
        printf("%s:%d log2(%f)\n", __FILE__, __LINE__, a);
        assert(0);
    }
#endif

    return ::log2f(a);
}

inline CUDA_CALLABLE double log2(double a) {
#if FP_CHECK
    if (!isfinite(a) || a < 0.0) {
        printf("%s:%d log2(%f)\n", __FILE__, __LINE__, a);
        assert(0);
    }
#endif

    return ::log2(a);
}

inline CUDA_CALLABLE half log10(half a) {
#if FP_CHECK
    if (!isfinite(a) || float(a) < 0.0f) {
        printf("%s:%d log10(%f)\n", __FILE__, __LINE__, float(a));
        assert(0);
    }
#endif

    return ::log10f(float(a));
}

inline CUDA_CALLABLE float log10(float a) {
#if FP_CHECK
    if (!isfinite(a) || a < 0.0f) {
        printf("%s:%d log10(%f)\n", __FILE__, __LINE__, a);
        assert(0);
    }
#endif

    return ::log10f(a);
}

inline CUDA_CALLABLE double log10(double a) {
#if FP_CHECK
    if (!isfinite(a) || a < 0.0) {
        printf("%s:%d log10(%f)\n", __FILE__, __LINE__, a);
        assert(0);
    }
#endif

    return ::log10(a);
}

inline CUDA_CALLABLE half exp(half a) {
    half result = ::expf(float(a));
#if FP_CHECK
    if (!isfinite(a) || !isfinite(result)) {
        printf("%s:%d exp(%f) = %f\n", __FILE__, __LINE__, float(a), float(result));
        assert(0);
    }
#endif
    return result;
}
inline CUDA_CALLABLE float exp(float a) {
    float result = ::expf(a);
#if FP_CHECK
    if (!isfinite(a) || !isfinite(result)) {
        printf("%s:%d exp(%f) = %f\n", __FILE__, __LINE__, a, result);
        assert(0);
    }
#endif
    return result;
}
inline CUDA_CALLABLE double exp(double a) {
    double result = ::exp(a);
#if FP_CHECK
    if (!isfinite(a) || !isfinite(result)) {
        printf("%s:%d exp(%f) = %f\n", __FILE__, __LINE__, a, result);
        assert(0);
    }
#endif
    return result;
}

inline CUDA_CALLABLE half pow(half a, half b) {
    float result = ::powf(float(a), float(b));
#if FP_CHECK
    if (!isfinite(float(a)) || !isfinite(float(b)) || !isfinite(result)) {
        printf("%s:%d pow(%f, %f) = %f\n", __FILE__, __LINE__, float(a), float(b), result);
        assert(0);
    }
#endif
    return result;
}

inline CUDA_CALLABLE float pow(float a, float b) {
    float result = ::powf(a, b);
#if FP_CHECK
    if (!isfinite(a) || !isfinite(b) || !isfinite(result)) {
        printf("%s:%d pow(%f, %f) = %f\n", __FILE__, __LINE__, a, b, result);
        assert(0);
    }
#endif
    return result;
}

inline CUDA_CALLABLE double pow(double a, double b) {
    double result = ::pow(a, b);
#if FP_CHECK
    if (!isfinite(a) || !isfinite(b) || !isfinite(result)) {
        printf("%s:%d pow(%f, %f) = %f\n", __FILE__, __LINE__, a, b, result);
        assert(0);
    }
#endif
    return result;
}

inline CUDA_CALLABLE half floordiv(half a, half b) {
#if FP_CHECK
    if (!isfinite(a) || !isfinite(b) || float(b) == 0.0f) {
        printf("%s:%d mod(%f, %f)\n", __FILE__, __LINE__, float(a), float(b));
        assert(0);
    }
#endif
    return floorf(float(a / b));
}
inline CUDA_CALLABLE float floordiv(float a, float b) {
#if FP_CHECK
    if (!isfinite(a) || !isfinite(b) || b == 0.0f) {
        printf("%s:%d mod(%f, %f)\n", __FILE__, __LINE__, a, b);
        assert(0);
    }
#endif
    return floorf(a / b);
}
inline CUDA_CALLABLE double floordiv(double a, double b) {
#if FP_CHECK
    if (!isfinite(a) || !isfinite(b) || b == 0.0) {
        printf("%s:%d mod(%f, %f)\n", __FILE__, __LINE__, a, b);
        assert(0);
    }
#endif
    return ::floor(a / b);
}

inline CUDA_CALLABLE float leaky_min(float a, float b, float r) { return min(a, b); }
inline CUDA_CALLABLE float leaky_max(float a, float b, float r) { return max(a, b); }

inline CUDA_CALLABLE half abs(half x) { return ::fabsf(float(x)); }
inline CUDA_CALLABLE float abs(float x) { return ::fabsf(x); }
inline CUDA_CALLABLE double abs(double x) { return ::fabs(x); }

inline CUDA_CALLABLE float acos(float x) { return ::acosf(min(max(x, -1.0f), 1.0f)); }
inline CUDA_CALLABLE float asin(float x) { return ::asinf(min(max(x, -1.0f), 1.0f)); }
inline CUDA_CALLABLE float atan(float x) { return ::atanf(x); }
inline CUDA_CALLABLE float atan2(float y, float x) { return ::atan2f(y, x); }
inline CUDA_CALLABLE float sin(float x) { return ::sinf(x); }
inline CUDA_CALLABLE float cos(float x) { return ::cosf(x); }

inline CUDA_CALLABLE double acos(double x) { return ::acos(min(max(x, -1.0), 1.0)); }
inline CUDA_CALLABLE double asin(double x) { return ::asin(min(max(x, -1.0), 1.0)); }
inline CUDA_CALLABLE double atan(double x) { return ::atan(x); }
inline CUDA_CALLABLE double atan2(double y, double x) { return ::atan2(y, x); }
inline CUDA_CALLABLE double sin(double x) { return ::sin(x); }
inline CUDA_CALLABLE double cos(double x) { return ::cos(x); }

inline CUDA_CALLABLE half acos(half x) { return ::acosf(min(max(float(x), -1.0f), 1.0f)); }
inline CUDA_CALLABLE half asin(half x) { return ::asinf(min(max(float(x), -1.0f), 1.0f)); }
inline CUDA_CALLABLE half atan(half x) { return ::atanf(float(x)); }
inline CUDA_CALLABLE half atan2(half y, half x) { return ::atan2f(float(y), float(x)); }
inline CUDA_CALLABLE half sin(half x) { return ::sinf(float(x)); }
inline CUDA_CALLABLE half cos(half x) { return ::cosf(float(x)); }

inline CUDA_CALLABLE float sqrt(float x) {
#if FP_CHECK
    if (x < 0.0f) {
        printf("%s:%d sqrt(%f)\n", __FILE__, __LINE__, x);
        assert(0);
    }
#endif
    return ::sqrtf(x);
}
inline CUDA_CALLABLE double sqrt(double x) {
#if FP_CHECK
    if (x < 0.0) {
        printf("%s:%d sqrt(%f)\n", __FILE__, __LINE__, x);
        assert(0);
    }
#endif
    return ::sqrt(x);
}
inline CUDA_CALLABLE half sqrt(half x) {
#if FP_CHECK
    if (float(x) < 0.0f) {
        printf("%s:%d sqrt(%f)\n", __FILE__, __LINE__, float(x));
        assert(0);
    }
#endif
    return ::sqrtf(float(x));
}

inline CUDA_CALLABLE float tan(float x) { return ::tanf(x); }
inline CUDA_CALLABLE float sinh(float x) { return ::sinhf(x); }
inline CUDA_CALLABLE float cosh(float x) { return ::coshf(x); }
inline CUDA_CALLABLE float tanh(float x) { return ::tanhf(x); }
inline CUDA_CALLABLE float degrees(float x) { return x * RAD_TO_DEG; }
inline CUDA_CALLABLE float radians(float x) { return x * DEG_TO_RAD; }

inline CUDA_CALLABLE double tan(double x) { return ::tan(x); }
inline CUDA_CALLABLE double sinh(double x) { return ::sinh(x); }
inline CUDA_CALLABLE double cosh(double x) { return ::cosh(x); }
inline CUDA_CALLABLE double tanh(double x) { return ::tanh(x); }
inline CUDA_CALLABLE double degrees(double x) { return x * RAD_TO_DEG; }
inline CUDA_CALLABLE double radians(double x) { return x * DEG_TO_RAD; }

inline CUDA_CALLABLE half tan(half x) { return ::tanf(float(x)); }
inline CUDA_CALLABLE half sinh(half x) { return ::sinhf(float(x)); }
inline CUDA_CALLABLE half cosh(half x) { return ::coshf(float(x)); }
inline CUDA_CALLABLE half tanh(half x) { return ::tanhf(float(x)); }
inline CUDA_CALLABLE half degrees(half x) { return x * RAD_TO_DEG; }
inline CUDA_CALLABLE half radians(half x) { return x * DEG_TO_RAD; }

inline CUDA_CALLABLE float round(float x) { return ::roundf(x); }
inline CUDA_CALLABLE float rint(float x) { return ::rintf(x); }
inline CUDA_CALLABLE float trunc(float x) { return ::truncf(x); }
inline CUDA_CALLABLE float floor(float x) { return ::floorf(x); }
inline CUDA_CALLABLE float ceil(float x) { return ::ceilf(x); }

template<typename C, typename T>
CUDA_CALLABLE inline T select(const C &cond, const T &a, const T &b) {
    // The double NOT operator !! casts to bool without compiler warnings.
    return (!!cond) ? b : a;
}

template<typename T>
CUDA_CALLABLE inline void copy(T &dest, const T &src) {
    dest = src;
}

// some helpful operator overloads (just for C++ use, these are not adjointed)

template<typename T>
CUDA_CALLABLE inline T &operator+=(T &a, const T &b) {
    a = add(a, b);
    return a;
}

template<typename T>
CUDA_CALLABLE inline T &operator-=(T &a, const T &b) {
    a = sub(a, b);
    return a;
}

template<typename T>
CUDA_CALLABLE inline T operator+(const T &a, const T &b) { return add(a, b); }

template<typename T>
CUDA_CALLABLE inline T operator-(const T &a, const T &b) { return sub(a, b); }

template<typename T>
CUDA_CALLABLE inline T pos(const T &x) { return x; }

// unary negation implemented as negative multiply, not sure the fp implications of this
// may be better as 0.0 - x?
template<typename T>
CUDA_CALLABLE inline T neg(const T &x) { return T(0.0) - x; }

// unary boolean negation
template<typename T>
CUDA_CALLABLE inline bool unot(const T &b) { return !b; }

const int LAUNCH_MAX_DIMS = 4;// should match types.py

struct launch_bounds_t {
    int shape[LAUNCH_MAX_DIMS];// size of each dimension
    int ndim;                  // number of valid dimension
    size_t size;               // total number of threads
};

#ifdef __CUDACC__

// store launch bounds in shared memory so
// we can access them from any user func
// this is to avoid having to explicitly
// set another piece of __constant__ memory
// from the host
__shared__ launch_bounds_t s_launchBounds;

__device__ inline void set_launch_bounds(const launch_bounds_t &b) {
    if (threadIdx.x == 0)
        s_launchBounds = b;

    __syncthreads();
}

#else

// for single-threaded CPU we store launch
// bounds in static memory to share globally
static launch_bounds_t s_launchBounds;
static size_t s_threadIdx;

inline void set_launch_bounds(const launch_bounds_t &b) {
    s_launchBounds = b;
}
#endif

inline CUDA_CALLABLE size_t grid_index() {
#ifdef __CUDACC__
    // Need to cast at least one of the variables being multiplied so that type promotion happens before the multiplication
    size_t grid_index = static_cast<size_t>(blockDim.x) * static_cast<size_t>(blockIdx.x) + static_cast<size_t>(threadIdx.x);
    return grid_index;
#else
    return s_threadIdx;
#endif
}

inline CUDA_CALLABLE int tid() {
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

inline CUDA_CALLABLE_DEVICE void tid(int &i, int &j) {
    const size_t index = grid_index();

    const size_t n = s_launchBounds.shape[1];

    // convert to work item
    i = index / n;
    j = index % n;
}

inline CUDA_CALLABLE_DEVICE void tid(int &i, int &j, int &k) {
    const size_t index = grid_index();

    const size_t n = s_launchBounds.shape[1];
    const size_t o = s_launchBounds.shape[2];

    // convert to work item
    i = index / (n * o);
    j = index % (n * o) / o;
    k = index % o;
}

inline CUDA_CALLABLE_DEVICE void tid(int &i, int &j, int &k, int &l) {
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

template<typename T>
inline CUDA_CALLABLE T atomic_add(T *buf, T value) {
#if !defined(__CUDA_ARCH__)
    T old = buf[0];
    buf[0] += value;
    return old;
#else
    return atomicAdd(buf, value);
#endif
}

template<>
inline CUDA_CALLABLE float16 atomic_add(float16 *buf, float16 value) {
#if !defined(__CUDA_ARCH__)
    float16 old = buf[0];
    buf[0] += value;
    return old;
#elif defined(__clang__)// CUDA compiled by Clang
    __half r = atomicAdd(reinterpret_cast<__half *>(buf), *reinterpret_cast<__half *>(&value));
    return *reinterpret_cast<float16 *>(&r);
#else                   // CUDA compiled by NVRTC
    //return atomicAdd(buf, value);

    /* Define __PTR for atomicAdd prototypes below, undef after done */
#if (defined(_MSC_VER) && defined(_WIN64)) || defined(__LP64__) || defined(__CUDACC_RTC__)
#define __PTR "l"
#else
#define __PTR "r"
#endif /*(defined(_MSC_VER) && defined(_WIN64)) || defined(__LP64__) || defined(__CUDACC_RTC__)*/

    half r = 0.0;

#if __CUDA_ARCH__ >= 700

    asm volatile("{ atom.add.noftz.f16 %0,[%1],%2; }\n"
                 : "=h"(r.u)
                 : __PTR(buf), "h"(value.u)
                 : "memory");
#endif

    return r;

#undef __PTR

#endif// CUDA compiled by NVRTC
}

// emulate atomic float max
inline CUDA_CALLABLE float atomic_max(float *address, float val) {
#if defined(__CUDA_ARCH__)
    int *address_as_int = (int *)address;
    int old = *address_as_int, assumed;

    while (val > __int_as_float(old)) {
        assumed = old;
        old = atomicCAS(address_as_int, assumed,
                        __float_as_int(val));
    }

    return __int_as_float(old);

#else
    float old = *address;
    *address = max(old, val);
    return old;
#endif
}

// emulate atomic float min/max with atomicCAS()
inline CUDA_CALLABLE float atomic_min(float *address, float val) {
#if defined(__CUDA_ARCH__)
    int *address_as_int = (int *)address;
    int old = *address_as_int, assumed;

    while (val < __int_as_float(old)) {
        assumed = old;
        old = atomicCAS(address_as_int, assumed,
                        __float_as_int(val));
    }

    return __int_as_float(old);

#else
    float old = *address;
    *address = min(old, val);
    return old;
#endif
}

inline CUDA_CALLABLE int atomic_max(int *address, int val) {
#if defined(__CUDA_ARCH__)
    return atomicMax(address, val);

#else
    int old = *address;
    *address = max(old, val);
    return old;
#endif
}

// atomic int min
inline CUDA_CALLABLE int atomic_min(int *address, int val) {
#if defined(__CUDA_ARCH__)
    return atomicMin(address, val);

#else
    int old = *address;
    *address = min(old, val);
    return old;
#endif
}

// dot for scalar types just to make some templates compile for scalar/vector
inline CUDA_CALLABLE float dot(float a, float b) { return mul(a, b); }
inline CUDA_CALLABLE float tensordot(float a, float b) { return mul(a, b); }

#define DECLARE_INTERP_FUNCS(T)                                \
    CUDA_CALLABLE inline T smoothstep(T edge0, T edge1, T x) { \
        x = clamp((x - edge0) / (edge1 - edge0), T(0), T(1));  \
        return x * x * (T(3) - T(2) * x);                      \
    }                                                          \
    CUDA_CALLABLE inline T lerp(const T &a, const T &b, T t) { \
        return a * (T(1) - t) + b * t;                         \
    }

DECLARE_INTERP_FUNCS(float16)
DECLARE_INTERP_FUNCS(float)
DECLARE_INTERP_FUNCS(double)

inline CUDA_CALLABLE void print(const char *s) {
    printf("%s\n", s);
}

inline CUDA_CALLABLE void print(int i) {
    printf("%d\n", i);
}

inline CUDA_CALLABLE void print(short i) {
    printf("%hd\n", i);
}

inline CUDA_CALLABLE void print(long i) {
    printf("%ld\n", i);
}

inline CUDA_CALLABLE void print(long long i) {
    printf("%lld\n", i);
}

inline CUDA_CALLABLE void print(unsigned i) {
    printf("%u\n", i);
}

inline CUDA_CALLABLE void print(unsigned short i) {
    printf("%hu\n", i);
}

inline CUDA_CALLABLE void print(unsigned long i) {
    printf("%lu\n", i);
}

inline CUDA_CALLABLE void print(unsigned long long i) {
    printf("%llu\n", i);
}

template<typename T>
inline CUDA_CALLABLE void expect_eq(const T &actual, const T &expected) {
    if (!(actual == expected)) {
        printf("Error, expect_eq() failed:\n");
        printf("\t Expected: ");
        print(expected);
        printf("\t Actual: ");
        print(actual);
    }
}

template<typename T>
inline CUDA_CALLABLE void expect_neq(const T &actual, const T &expected) {
    if (actual == expected) {
        printf("Error, expect_neq() failed:\n");
        printf("\t Expected: ");
        print(expected);
        printf("\t Actual: ");
        print(actual);
    }
}

template<typename T>
inline CUDA_CALLABLE void expect_near(const T &actual, const T &expected, const T &tolerance) {
    if (abs(actual - expected) > tolerance) {
        printf("Error, expect_near() failed with tolerance ");
        print(tolerance);
        printf("\t Expected: ");
        print(expected);
        printf("\t Actual: ");
        print(actual);
    }
}

}// namespace wp
