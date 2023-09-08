//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "mat.h"

namespace wp {

template<typename Type>
struct quat_t {
    // zero constructor for adjoint variable initialization
    inline __device__ explicit quat_t(Type x = Type(0), Type y = Type(0), Type z = Type(0), Type w = Type(0)) : x(x), y(y), z(z), w(w) {}
    explicit inline __device__ quat_t(const vec_t<3, Type> &v, Type w = Type(0)) : x(v[0]), y(v[1]), z(v[2]), w(w) {}

    // imaginary part
    Type x;
    Type y;
    Type z;

    // real part
    Type w;
};

using quat = quat_t<float>;
using quatf = quat_t<float>;
using quatd = quat_t<double>;

template<typename Type>
inline __device__ bool operator==(const quat_t<Type> &a, const quat_t<Type> &b) {
    return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w;
}

template<typename Type>
inline bool __device__ isfinite(const quat_t<Type> &q) {
    return isfinite(q.x) && isfinite(q.y) && isfinite(q.z) && isfinite(q.w);
}

template<typename Type>
inline __device__ quat_t<Type> atomic_add(quat_t<Type> *addr, quat_t<Type> value) {
    Type x = atomic_add(&(addr->x), value.x);
    Type y = atomic_add(&(addr->y), value.y);
    Type z = atomic_add(&(addr->z), value.z);
    Type w = atomic_add(&(addr->w), value.w);

    return quat_t<Type>(x, y, z, w);
}

// forward methods

template<typename Type>
inline __device__ quat_t<Type> quat_from_axis_angle(const vec_t<3, Type> &axis, Type angle) {
    Type half = angle * Type(Type(0.5));
    Type w = cos(half);

    Type sin_theta_over_two = sin(half);
    vec_t<3, Type> v = axis * sin_theta_over_two;

    return quat_t<Type>(v[0], v[1], v[2], w);
}

template<typename Type>
inline __device__ void quat_to_axis_angle(const quat_t<Type> &q, vec_t<3, Type> &axis, Type &angle) {
    vec_t<3, Type> v = vec_t<3, Type>(q.x, q.y, q.z);
    axis = q.w < Type(0) ? -normalize(v) : normalize(v);
    angle = Type(2) * atan2(length(v), abs(q.w));
}

template<typename Type>
inline __device__ quat_t<Type> quat_rpy(Type roll, Type pitch, Type yaw) {
    Type cy = cos(yaw * Type(0.5));
    Type sy = sin(yaw * Type(0.5));
    Type cr = cos(roll * Type(0.5));
    Type sr = sin(roll * Type(0.5));
    Type cp = cos(pitch * Type(0.5));
    Type sp = sin(pitch * Type(0.5));

    Type w = (cy * cr * cp + sy * sr * sp);
    Type x = (cy * sr * cp - sy * cr * sp);
    Type y = (cy * cr * sp + sy * sr * cp);
    Type z = (sy * cr * cp - cy * sr * sp);

    return quat_t<Type>(x, y, z, w);
}

template<typename Type>
inline __device__ quat_t<Type> quat_inverse(const quat_t<Type> &q) {
    return quat_t<Type>(-q.x, -q.y, -q.z, q.w);
}

template<typename Type>
inline __device__ Type dot(const quat_t<Type> &a, const quat_t<Type> &b) {
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

template<typename Type>
inline __device__ Type tensordot(const quat_t<Type> &a, const quat_t<Type> &b) {
    // corresponds to `np.tensordot()` with all axes being contracted
    return dot(a, b);
}

template<typename Type>
inline __device__ Type length(const quat_t<Type> &q) {
    return sqrt(dot(q, q));
}

template<typename Type>
inline __device__ Type length_sq(const quat_t<Type> &q) {
    return dot(q, q);
}

template<typename Type>
inline __device__ quat_t<Type> normalize(const quat_t<Type> &q) {
    Type l = length(q);
    if (l > Type(EPSILON)) {
        Type inv_l = Type(1) / l;

        return quat_t<Type>(q.x * inv_l, q.y * inv_l, q.z * inv_l, q.w * inv_l);
    } else {
        return quat_t<Type>(Type(0), Type(0), Type(0), Type(1));
    }
}

template<typename Type>
inline __device__ quat_t<Type> add(const quat_t<Type> &a, const quat_t<Type> &b) {
    return quat_t<Type>(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
}

template<typename Type>
inline __device__ quat_t<Type> sub(const quat_t<Type> &a, const quat_t<Type> &b) {
    return quat_t<Type>(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
}

template<typename Type>
inline __device__ quat_t<Type> mul(const quat_t<Type> &a, const quat_t<Type> &b) {
    return quat_t<Type>(a.w * b.x + b.w * a.x + a.y * b.z - b.y * a.z,
                        a.w * b.y + b.w * a.y + a.z * b.x - b.z * a.x,
                        a.w * b.z + b.w * a.z + a.x * b.y - b.x * a.y,
                        a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z);
}

template<typename Type>
inline __device__ quat_t<Type> mul(const quat_t<Type> &a, Type s) {
    return quat_t<Type>(a.x * s, a.y * s, a.z * s, a.w * s);
}

template<typename Type>
inline __device__ quat_t<Type> mul(Type s, const quat_t<Type> &a) {
    return mul(a, s);
}

// division
template<typename Type>
inline __device__ quat_t<Type> div(quat_t<Type> q, Type s) {
    return quat_t<Type>(q.x / s, q.y / s, q.z / s, q.w / s);
}

template<typename Type>
inline __device__ quat_t<Type> operator/(quat_t<Type> a, Type s) {
    return div(a, s);
}

template<typename Type>
inline __device__ quat_t<Type> operator*(Type s, const quat_t<Type> &a) {
    return mul(a, s);
}

template<typename Type>
inline __device__ quat_t<Type> operator*(const quat_t<Type> &a, Type s) {
    return mul(a, s);
}

template<typename Type>
inline __device__ vec_t<3, Type> quat_rotate(const quat_t<Type> &q, const vec_t<3, Type> &x) {
    Type c = (Type(2) * q.w * q.w - Type(1));
    Type d = Type(2) * (q.x * x.c[0] + q.y * x.c[1] + q.z * x.c[2]);
    return vec_t<3, Type>(
        x.c[0] * c + q.x * d + (q.y * x[2] - q.z * x[1]) * q.w * Type(2),
        x.c[1] * c + q.y * d + (q.z * x[0] - q.x * x[2]) * q.w * Type(2),
        x.c[2] * c + q.z * d + (q.x * x[1] - q.y * x[0]) * q.w * Type(2));
}

template<typename Type>
inline __device__ vec_t<3, Type> quat_rotate_inv(const quat_t<Type> &q, const vec_t<3, Type> &x) {
    Type c = (Type(2) * q.w * q.w - Type(1));
    Type d = Type(2) * (q.x * x.c[0] + q.y * x.c[1] + q.z * x.c[2]);
    return vec_t<3, Type>(
        x.c[0] * c + q.x * d - (q.y * x[2] - q.z * x[1]) * q.w * Type(2),
        x.c[1] * c + q.y * d - (q.z * x[0] - q.x * x[2]) * q.w * Type(2),
        x.c[2] * c + q.z * d - (q.x * x[1] - q.y * x[0]) * q.w * Type(2));
}

template<typename Type>
inline __device__ quat_t<Type> quat_slerp(const quat_t<Type> &q0, const quat_t<Type> &q1, Type t) {
    vec_t<3, Type> axis;
    Type angle;
    quat_to_axis_angle(mul(quat_inverse(q0), q1), axis, angle);
    return mul(q0, quat_from_axis_angle(axis, t * angle));
}

template<typename Type>
inline __device__ mat_t<3, 3, Type> quat_to_matrix(const quat_t<Type> &q) {
    vec_t<3, Type> c1 = quat_rotate(q, vec_t<3, Type>(1.0, 0.0, 0.0));
    vec_t<3, Type> c2 = quat_rotate(q, vec_t<3, Type>(0.0, 1.0, 0.0));
    vec_t<3, Type> c3 = quat_rotate(q, vec_t<3, Type>(0.0, 0.0, 1.0));

    return mat_t<3, 3, Type>(c1, c2, c3);
}

template<typename Type>
inline __device__ quat_t<Type> quat_from_matrix(const mat_t<3, 3, Type> &m) {
    const Type tr = m.data[0][0] + m.data[1][1] + m.data[2][2];
    Type x, y, z, w, h = Type(0);

    if (tr >= Type(0)) {
        h = sqrt(tr + Type(1));
        w = Type(0.5) * h;
        h = Type(0.5) / h;

        x = (m.data[2][1] - m.data[1][2]) * h;
        y = (m.data[0][2] - m.data[2][0]) * h;
        z = (m.data[1][0] - m.data[0][1]) * h;
    } else {
        size_t max_diag = 0;
        if (m.data[1][1] > m.data[0][0]) {
            max_diag = 1;
        }
        if (m.data[2][2] > m.data[max_diag][max_diag]) {
            max_diag = 2;
        }

        if (max_diag == 0) {
            h = sqrt((m.data[0][0] - (m.data[1][1] + m.data[2][2])) + Type(1));
            x = Type(0.5) * h;
            h = Type(0.5) / h;

            y = (m.data[0][1] + m.data[1][0]) * h;
            z = (m.data[2][0] + m.data[0][2]) * h;
            w = (m.data[2][1] - m.data[1][2]) * h;
        } else if (max_diag == 1) {
            h = sqrt((m.data[1][1] - (m.data[2][2] + m.data[0][0])) + Type(1));
            y = Type(0.5) * h;
            h = Type(0.5) / h;

            z = (m.data[1][2] + m.data[2][1]) * h;
            x = (m.data[0][1] + m.data[1][0]) * h;
            w = (m.data[0][2] - m.data[2][0]) * h;
        }
        if (max_diag == 2) {
            h = sqrt((m.data[2][2] - (m.data[0][0] + m.data[1][1])) + Type(1));
            z = Type(0.5) * h;
            h = Type(0.5) / h;

            x = (m.data[2][0] + m.data[0][2]) * h;
            y = (m.data[1][2] + m.data[2][1]) * h;
            w = (m.data[1][0] - m.data[0][1]) * h;
        }
    }

    return normalize(quat_t<Type>(x, y, z, w));
}

template<typename Type>
inline __device__ Type index(const quat_t<Type> &a, int idx) {
    /*
    * Because quat data is not stored in an array, we index the quaternion by checking all possible idx values.
    * (&a.x)[idx] would be the preferred access strategy, but this results in undefined behavior in the clang compiler
    * at optimization level 3.
    */
    if (idx == 0) {
        return a.x;
    } else if (idx == 1) {
        return a.y;
    } else if (idx == 2) {
        return a.z;
    } else {
        return a.w;
    }
}

template<typename Type>
__device__ inline quat_t<Type> lerp(const quat_t<Type> &a, const quat_t<Type> &b, Type t) {
    return a * (Type(1) - t) + b * t;
}

template<unsigned Rows, unsigned Cols, typename Type>
inline __device__ mat_t<Rows, Cols, Type>::mat_t(const vec_t<3, Type> &pos, const quat_t<Type> &rot, const vec_t<3, Type> &scale) {
    mat_t<3, 3, Type> R = quat_to_matrix(rot);

    data[0][0] = R.data[0][0] * scale[0];
    data[1][0] = R.data[1][0] * scale[0];
    data[2][0] = R.data[2][0] * scale[0];
    data[3][0] = Type(0);

    data[0][1] = R.data[0][1] * scale[1];
    data[1][1] = R.data[1][1] * scale[1];
    data[2][1] = R.data[2][1] * scale[1];
    data[3][1] = Type(0);

    data[0][2] = R.data[0][2] * scale[2];
    data[1][2] = R.data[1][2] * scale[2];
    data[2][2] = R.data[2][2] * scale[2];
    data[3][2] = Type(0);

    data[0][3] = pos[0];
    data[1][3] = pos[1];
    data[2][3] = pos[2];
    data[3][3] = Type(1);
}

template<typename Type = float>
inline __device__ quat_t<Type> quat_identity() {
    return quat_t<Type>(Type(0), Type(0), Type(0), Type(1));
}

template<typename Type>
inline CUDA_CALLABLE void print(quat_t<Type> i) {
    printf("%g %g %g %g\n", float(i.x), float(i.y), float(i.z), float(i.w));
}

}// namespace wp