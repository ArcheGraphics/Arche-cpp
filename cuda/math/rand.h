//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vec.h"
#include "array.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

namespace wp {

inline __device__ uint32_t rand_pcg(uint32_t state) {
    uint32_t b = state * 747796405u + 2891336453u;
    uint32_t c = ((b >> ((b >> 28u) + 4u)) ^ b) * 277803737u;
    return (c >> 22u) ^ c;
}

inline __device__ uint32_t rand_init(int seed) { return rand_pcg(uint32_t(seed)); }
inline __device__ uint32_t rand_init(int seed, int offset) { return rand_pcg(uint32_t(seed) + rand_pcg(uint32_t(offset))); }

inline __device__ int randi(uint32_t &state) {
    state = rand_pcg(state);
    return int(state);
}
inline __device__ int randi(uint32_t &state, int min, int max) {
    state = rand_pcg(state);
    return state % (max - min) + min;
}

inline __device__ float randf(uint32_t &state) {
    state = rand_pcg(state);
    return (state >> 8) * (1.0f / 16777216.0f);
}
inline __device__ float randf(uint32_t &state, float min, float max) { return (max - min) * randf(state) + min; }

// Box-Muller method
inline __device__ float randn(uint32_t &state) { return sqrt(-2.f * log(randf(state))) * cos(2.f * M_PI * randf(state)); }

inline __device__ int sample_cdf(uint32_t &state, const array_t<float> &cdf) {
    float u = randf(state);
    return lower_bound<float>(cdf, u);
}

inline __device__ vec2 sample_triangle(uint32_t &state) {
    float r = sqrt(randf(state));
    float u = 1.0f - r;
    float v = randf(state) * r;
    return {u, v};
}

inline __device__ vec2 sample_unit_ring(uint32_t &state) {
    float theta = randf(state, 0.f, 2.f * M_PI);
    float x = cos(theta);
    float y = sin(theta);
    return {x, y};
}

inline __device__ vec2 sample_unit_disk(uint32_t &state) {
    float r = sqrt(randf(state));
    float theta = randf(state, 0.f, 2.f * M_PI);
    float x = r * cos(theta);
    float y = r * sin(theta);
    return {x, y};
}

inline __device__ vec3 sample_unit_sphere_surface(uint32_t &state) {
    float phi = acos(1.f - 2.f * randf(state));
    float theta = randf(state, 0.f, 2.f * M_PI);
    float x = cos(theta) * sin(phi);
    float y = sin(theta) * sin(phi);
    float z = cos(phi);
    return {x, y, z};
}

inline __device__ vec3 sample_unit_sphere(uint32_t &state) {
    float phi = acos(1.f - 2.f * randf(state));
    float theta = randf(state, 0.f, 2.f * M_PI);
    float r = pow(randf(state), 1.f / 3.f);
    float x = r * cos(theta) * sin(phi);
    float y = r * sin(theta) * sin(phi);
    float z = r * cos(phi);
    return {x, y, z};
}

inline __device__ vec3 sample_unit_hemisphere_surface(uint32_t &state) {
    float phi = acos(1.f - randf(state));
    float theta = randf(state, 0.f, 2.f * M_PI);
    float x = cos(theta) * sin(phi);
    float y = sin(theta) * sin(phi);
    float z = cos(phi);
    return {x, y, z};
}

inline __device__ vec3 sample_unit_hemisphere(uint32_t &state) {
    float phi = acos(1.f - randf(state));
    float theta = randf(state, 0.f, 2.f * M_PI);
    float r = pow(randf(state), 1.f / 3.f);
    float x = r * cos(theta) * sin(phi);
    float y = r * sin(theta) * sin(phi);
    float z = r * cos(phi);
    return {x, y, z};
}

inline __device__ vec2 sample_unit_square(uint32_t &state) {
    float x = randf(state) - 0.5f;
    float y = randf(state) - 0.5f;
    return {x, y};
}

inline __device__ vec3 sample_unit_cube(uint32_t &state) {
    float x = randf(state) - 0.5f;
    float y = randf(state) - 0.5f;
    float z = randf(state) - 0.5f;
    return {x, y, z};
}

/*
 * log-gamma function to support some of these distributions. The
 * algorithm comes from SPECFUN by Shanjie Zhang and Jianming Jin and their
 * book "Computation of Special Functions", 1996, John Wiley & Sons, Inc.
 *
 * If random_loggam(k+1) is being used to compute log(k!) for an integer k, consider
 * using logfactorial(k) instead.
 */
inline __device__ float random_loggam(float x) {
    float x0, x2, lg2pi, gl, gl0;
    uint32_t n;

    const float a[10] = {8.333333333333333e-02, -2.777777777777778e-03,
                         7.936507936507937e-04, -5.952380952380952e-04,
                         8.417508417508418e-04, -1.917526917526918e-03,
                         6.410256410256410e-03, -2.955065359477124e-02,
                         1.796443723688307e-01, -1.39243221690590e+00};

    if ((x == 1.0) || (x == 2.0)) {
        return 0.0;
    } else if (x < 7.0) {
        n = uint32_t((7 - x));
    } else {
        n = 0;
    }

    x0 = x + float(n);
    x2 = (1.0f / x0) * (1.0f / x0);
    // log(2 * M_PI)
    lg2pi = 1.8378770664093453f;
    gl0 = a[9];
    for (int i = 8; i >= 0; i--) {
        gl0 *= x2;
        gl0 += a[i];
    }
    gl = gl0 / x0 + 0.5f * lg2pi + (x0 - 0.5f) * log(x0) - x0;
    if (x < 7.0) {
        for (uint32_t k = 1; k <= n; k++) {
            gl -= log(x0 - 1.0f);
            x0 -= 1.0;
        }
    }
    return gl;
}

inline __device__ uint32_t random_poisson_mult(uint32_t &state, float lam) {
    uint32_t X;
    float prod, U, enlam;

    enlam = exp(-lam);
    X = 0;
    prod = 1.0;

    while (true) {
        U = randf(state);
        prod *= U;
        if (prod > enlam) {
            X += 1;
        } else {
            return X;
        }
    }
}

/*
 * The transformed rejection method for generating Poisson random variables
 * W. Hoermann
 * Insurance: Mathematics and Economics 12, 39-45 (1993)
 */
inline __device__ uint32_t random_poisson(uint32_t &state, float lam) {
    uint32_t k;
    float U, V, slam, loglam, a, b, invalpha, vr, us;

    slam = sqrt(lam);
    loglam = log(lam);
    b = 0.931f + 2.53f * slam;
    a = -0.059f + 0.02483f * b;
    invalpha = 1.1239f + 1.1328f / (b - 3.4f);
    vr = 0.9277f - 3.6224f / (b - 2.0f);

    while (true) {
        U = randf(state) - 0.5f;
        V = randf(state);
        us = 0.5f - abs(U);
        k = uint32_t(floor((2 * a / us + b) * U + lam + 0.43));
        if ((us >= 0.07) && (V <= vr)) {
            return k;
        }
        if ((us < 0.013) && (V > us)) {
            continue;
        }
        if ((log(V) + log(invalpha) - log(a / (us * us) + b)) <= (-lam + k * loglam - random_loggam(k + 1))) {
            return k;
        }
    }
}

/*
* Adpated from NumPy's implementation
* Warp's state variable is half the precision of NumPy's so
* poisson implementation uses half the precision used in NumPy's implementation
* both precisions appear to converge in the statistical limit
*/
inline __device__ uint32_t poisson(uint32_t &state, float lam) {
    if (lam >= 10.f) {
        return random_poisson(state, lam);
    } else if (lam == 0.f) {
        return 0;
    } else {
        return random_poisson_mult(state, lam);
    }
}

}// namespace wp