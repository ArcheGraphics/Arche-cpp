//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vec.h"
#include "quat.h"

namespace wp {

//---------------------------------------------------------------------------------
// Represents a twist in se(3)
template<typename Type>
using spatial_vector_t = vec_t<6, Type>;

template<typename Type>
__device__ inline Type spatial_dot(const spatial_vector_t<Type> &a, const spatial_vector_t<Type> &b) {
    return dot(a, b);
}

template<typename Type>
__device__ inline vec_t<3, Type> &w_vec(spatial_vector_t<Type> &a) {
    return *(vec_t<3, Type> *)(&a);
}

template<typename Type>
__device__ inline vec_t<3, Type> &v_vec(spatial_vector_t<Type> &a) {
    return *(vec_t<3, Type> *)(&a.c[3]);
}

template<typename Type>
__device__ inline const vec_t<3, Type> &w_vec(const spatial_vector_t<Type> &a) {
    spatial_vector_t<Type> &non_const_vec = *(spatial_vector_t<Type> *)(const_cast<Type *>(&a.c[0]));
    return w_vec(non_const_vec);
}

template<typename Type>
__device__ inline const vec_t<3, Type> &v_vec(const spatial_vector_t<Type> &a) {
    spatial_vector_t<Type> &non_const_vec = *(spatial_vector_t<Type> *)(const_cast<Type *>(&a.c[0]));
    return v_vec(non_const_vec);
}

template<typename Type>
__device__ inline spatial_vector_t<Type> spatial_cross(const spatial_vector_t<Type> &a, const spatial_vector_t<Type> &b) {
    vec_t<3, Type> w = cross(w_vec(a), w_vec(b));
    vec_t<3, Type> v = cross(v_vec(a), w_vec(b)) + cross(w_vec(a), v_vec(b));

    return spatial_vector_t<Type>({w[0], w[1], w[2], v[0], v[1], v[2]});
}

template<typename Type>
__device__ inline spatial_vector_t<Type> spatial_cross_dual(const spatial_vector_t<Type> &a, const spatial_vector_t<Type> &b) {
    vec_t<3, Type> w = cross(w_vec(a), w_vec(b)) + cross(v_vec(a), v_vec(b));
    vec_t<3, Type> v = cross(w_vec(a), v_vec(b));

    return spatial_vector_t<Type>({w[0], w[1], w[2], v[0], v[1], v[2]});
}

template<typename Type>
__device__ inline vec_t<3, Type> spatial_top(const spatial_vector_t<Type> &a) {
    return w_vec(a);
}

template<typename Type>
__device__ inline vec_t<3, Type> spatial_bottom(const spatial_vector_t<Type> &a) {
    return v_vec(a);
}

//---------------------------------------------------------------------------------
// Represents a rigid body transform<Type>ation

template<typename Type>
struct transform_t {
    vec_t<3, Type> p;
    quat_t<Type> q;

    __device__ inline explicit transform_t(vec_t<3, Type> p = vec_t<3, Type>(), quat_t<Type> q = quat_t<Type>()) : p(p), q(q) {}
    __device__ inline explicit transform_t(Type) {}// helps uniform initialization

    __device__ inline Type operator[](int index) const {
        assert(index < 7);

        return p.c[index];
    }

    __device__ inline Type &operator[](int index) {
        assert(index < 7);

        return p.c[index];
    }
};

template<typename Type = float>
__device__ inline transform_t<Type> transform_identity() {
    return transform_t<Type>(vec_t<3, Type>(), quat_identity<Type>());
}

template<typename Type>
inline __device__ bool operator==(const transform_t<Type> &a, const transform_t<Type> &b) {
    return a.p == b.p && a.q == b.q;
}

template<typename Type>
inline bool __device__ isfinite(const transform_t<Type> &t) {
    return isfinite(t.p) && isfinite(t.q);
}

template<typename Type>
__device__ inline vec_t<3, Type> transform_get_translation(const transform_t<Type> &t) {
    return t.p;
}

template<typename Type>
__device__ inline quat_t<Type> transform_get_rotation(const transform_t<Type> &t) {
    return t.q;
}

template<typename Type>
__device__ inline transform_t<Type> transform_multiply(const transform_t<Type> &a, const transform_t<Type> &b) {
    return {quat_rotate(a.q, b.p) + a.p, mul(a.q, b.q)};
}

template<typename Type>
__device__ inline transform_t<Type> transform_inverse(const transform_t<Type> &t) {
    quat_t<Type> q_inv = quat_inverse(t.q);
    return transform_t<Type>(-quat_rotate(q_inv, t.p), q_inv);
}

template<typename Type>
__device__ inline vec_t<3, Type> transform_vector(const transform_t<Type> &t, const vec_t<3, Type> &x) {
    return quat_rotate(t.q, x);
}

template<typename Type>
__device__ inline vec_t<3, Type> transform_point(const transform_t<Type> &t, const vec_t<3, Type> &x) {
    return t.p + quat_rotate(t.q, x);
}

// not totally sure why you'd want to do this seeing as adding/subtracting two rotation
// quats doesn't seem to do anything meaningful
template<typename Type>
__device__ inline transform_t<Type> add(const transform_t<Type> &a, const transform_t<Type> &b) {
    return {a.p + b.p, a.q + b.q};
}

template<typename Type>
__device__ inline transform_t<Type> sub(const transform_t<Type> &a, const transform_t<Type> &b) {
    return {a.p - b.p, a.q - b.q};
}

// also not sure why you'd want to do this seeing as the quat would end up unnormalized
template<typename Type>
__device__ inline transform_t<Type> mul(const transform_t<Type> &a, Type s) {
    return {a.p * s, a.q * s};
}

template<typename Type>
__device__ inline transform_t<Type> mul(Type s, const transform_t<Type> &a) {
    return mul(a, s);
}

template<typename Type>
__device__ inline transform_t<Type> mul(const transform_t<Type> &a, const transform_t<Type> &b) {
    return transform_multiply(a, b);
}

template<typename Type>
__device__ inline transform_t<Type> operator*(const transform_t<Type> &a, Type s) {
    return mul(a, s);
}

template<typename Type>
__device__ inline transform_t<Type> operator*(Type s, const transform_t<Type> &a) {
    return mul(a, s);
}

template<typename Type>
inline __device__ Type tensordot(const transform_t<Type> &a, const transform_t<Type> &b) {
    // corresponds to `np.tensordot()` with all axes being contracted
    return tensordot(a.p, b.p) + tensordot(a.q, b.q);
}

template<typename Type>
inline __device__ Type index(const transform_t<Type> &t, int i) {
    return t[i];
}

template<typename Type>
inline __device__ transform_t<Type> atomic_add(transform_t<Type> *addr, const transform_t<Type> &value) {
    vec_t<3, Type> p = atomic_add(&addr->p, value.p);
    quat_t<Type> q = atomic_add(&addr->q, value.q);

    return transform_t<Type>(p, q);
}

template<typename Type>
__device__ inline transform_t<Type> lerp(const transform_t<Type> &a, const transform_t<Type> &b, Type t) {
    return a * (Type(1) - t) + b * t;
}

template<typename Type>
using spatial_matrix_t = mat_t<6, 6, Type>;

template<typename Type>
inline __device__ spatial_matrix_t<Type> spatial_adjoint(const mat_t<3, 3, Type> &R, const mat_t<3, 3, Type> &S) {
    spatial_matrix_t<Type> adT;

    // T = [Rah,   0]
    //     [S  R]

    // diagonal blocks
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            adT.data[i][j] = R.data[i][j];
            adT.data[i + 3][j + 3] = R.data[i][j];
        }
    }

    // lower off diagonal
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            adT.data[i + 3][j] = S.data[i][j];
        }
    }

    return adT;
}

__device__ inline int row_index(int stride, int i, int j) {
    return i * stride + j;
}

// builds spatial Jacobian J which is an (joint_count*6)x(dof_count) matrix
template<typename Type>
__device__ inline void spatial_jacobian(
    const spatial_vector_t<Type> *S,
    const int *joint_parents,
    const int *joint_qd_start,
    int joint_start,// offset of the first joint for the articulation
    int joint_count,
    int J_start,
    Type *J) {
    const int articulation_dof_start = joint_qd_start[joint_start];
    const int articulation_dof_end = joint_qd_start[joint_start + joint_count];
    const int articulation_dof_count = articulation_dof_end - articulation_dof_start;

    // shift output pointers
    const int S_start = articulation_dof_start;

    S += S_start;
    J += J_start;

    for (int i = 0; i < joint_count; ++i) {
        const int row_start = i * 6;

        int j = joint_start + i;
        while (j != -1) {
            const int joint_dof_start = joint_qd_start[j];
            const int joint_dof_end = joint_qd_start[j + 1];
            const int joint_dof_count = joint_dof_end - joint_dof_start;

            // fill out each row of the Jacobian walking up the tree
            //for (int col=dof_start; col < dof_end; ++col)
            for (int dof = 0; dof < joint_dof_count; ++dof) {
                const int col = (joint_dof_start - articulation_dof_start) + dof;

                J[row_index(articulation_dof_count, row_start + 0, col)] = S[col].w[0];
                J[row_index(articulation_dof_count, row_start + 1, col)] = S[col].w[1];
                J[row_index(articulation_dof_count, row_start + 2, col)] = S[col].w[2];
                J[row_index(articulation_dof_count, row_start + 3, col)] = S[col].v[0];
                J[row_index(articulation_dof_count, row_start + 4, col)] = S[col].v[1];
                J[row_index(articulation_dof_count, row_start + 5, col)] = S[col].v[2];
            }

            j = joint_parents[j];
        }
    }
}

template<typename Type>
__device__ inline void spatial_mass(const spatial_matrix_t<Type> *I_s, int joint_start, int joint_count, int M_start, Type *M) {
    const int stride = joint_count * 6;

    for (int l = 0; l < joint_count; ++l) {
        for (int i = 0; i < 6; ++i) {
            for (int j = 0; j < 6; ++j) {
                M[M_start + row_index(stride, l * 6 + i, l * 6 + j)] = I_s[joint_start + l].data[i][j];
            }
        }
    }
}

using transform = transform_t<float>;
using transformf = transform_t<float>;
using transformd = transform_t<double>;

using spatial_vector = spatial_vector_t<float>;
using spatial_vectorf = spatial_vector_t<float>;
using spatial_vectord = spatial_vector_t<double>;

using spatial_matrix = spatial_matrix_t<float>;
using spatial_matrixf = spatial_matrix_t<float>;
using spatial_matrixd = spatial_matrix_t<double>;

template<typename Type>
inline CUDA_CALLABLE void print(transform_t<Type> t) {
    printf("(%g %g %g) (%g %g %g %g)\n", float(t.p[0]), float(t.p[1]), float(t.p[2]), float(t.q.x), float(t.q.y), float(t.q.z), float(t.q.w));
}

}// namespace wp