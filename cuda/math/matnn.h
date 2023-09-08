/** Copyright (c) 2022 NVIDIA CORPORATION.  All rights reserved.
 * NVIDIA CORPORATION and its licensors retain all intellectual property
 * and proprietary rights in and to this software, related documentation
 * and any modifications thereto.  Any use, reproduction, disclosure or
 * distribution of this software and related documentation without an express
 * license agreement from NVIDIA CORPORATION is strictly prohibited.
 */

#pragma once

#include "vec.h"
#include "array.h"

namespace wp {

__device__ inline int dense_index(int stride, int i, int j) {
    return i * stride + j;
}

template<bool transpose>
__device__ inline int dense_index(int rows, int cols, int i, int j) {
    if (transpose)
        return j * rows + i;
    else
        return i * cols + j;
}

template<bool t1, bool t2, bool add>
__device__ inline void dense_gemm_impl(int m, int n, int p, const float *__restrict__ A, const float *__restrict__ B, float *__restrict__ C) {
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; ++j) {
            float sum = 0.0f;

            for (int k = 0; k < p; ++k) {
                sum += A[dense_index<t1>(m, p, i, k)] * B[dense_index<t2>(p, n, k, j)];
            }

            if (add)
                C[i * n + j] += sum;
            else
                C[i * n + j] = sum;
        }
    }
}

template<bool add = false>
__device__ inline void dense_gemm(int m, int n, int p, int t1, int t2, const array_t<float> &A, const array_t<float> &B, array_t<float> &C) {
    if (t1 == 0 && t2 == 0)
        dense_gemm_impl<false, false, add>(m, n, p, A.data, B.data, C.data);
    else if (t1 == 1 && t2 == 0)
        dense_gemm_impl<true, false, add>(m, n, p, A.data, B.data, C.data);
    else if (t1 == 0 && t2 == 1)
        dense_gemm_impl<false, true, add>(m, n, p, A.data, B.data, C.data);
    else if (t1 == 1 && t2 == 1)
        dense_gemm_impl<true, true, add>(m, n, p, A.data, B.data, C.data);
}

void __device__ inline dense_chol(int n, const array_t<float> &A, float regularization, array_t<float> &L) {
    for (int j = 0; j < n; ++j) {
        float s = A.data[dense_index(n, j, j)] + regularization;

        for (int k = 0; k < j; ++k) {
            float r = L.data[dense_index(n, j, k)];
            s -= r * r;
        }

        s = sqrt(s);
        const float invS = 1.0f / s;

        L.data[dense_index(n, j, j)] = s;

        for (int i = j + 1; i < n; ++i) {
            s = A.data[dense_index(n, i, j)];

            for (int k = 0; k < j; ++k) {
                s -= L.data[dense_index(n, i, k)] * L.data[dense_index(n, j, k)];
            }

            L.data[dense_index(n, i, j)] = s * invS;
        }
    }
}

// Solves (L*L^T)x = b given the Cholesky factor L
__device__ inline void dense_subs(int n, const array_t<float> &L, const array_t<float> &b, array_t<float> &x) {
    // forward substitution
    for (int i = 0; i < n; ++i) {
        float s = b.data[i];

        for (int j = 0; j < i; ++j) {
            s -= L.data[dense_index(n, i, j)] * x.data[j];
        }

        x.data[i] = s / L.data[dense_index(n, i, i)];
    }

    // backward substitution
    for (int i = n - 1; i >= 0; --i) {
        float s = x.data[i];

        for (int j = i + 1; j < n; ++j) {
            s -= L.data[dense_index(n, j, i)] * x.data[j];
        }

        x.data[i] = s / L.data[dense_index(n, i, i)];
    }
}

__device__ inline void dense_solve(int n, const array_t<float> &A, const array_t<float> &L, const array_t<float> &b, array_t<float> &x) {
    dense_subs(n, L, b, x);
}

template<typename F>
__device__ inline void mlp(const array_t<float> &weights, const array_t<float> &bias, F activation, int index, const array_t<float> &x, array_t<float> &out) {
    const int m = weights.shape[0];
    const int n = weights.shape[1];
    const int b = x.shape[1];

    for (int i = 0; i < m; ++i) {
        float tmp = bias.data[i];

        for (int j = 0; j < n; ++j) {
            tmp += weights.data[i * n + j] * x.data[index + b * j];
        }

        out.data[index + b * i] = activation(tmp);
    }
}

}// namespace wp