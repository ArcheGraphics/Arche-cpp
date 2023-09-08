//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vec.h"
#include <cassert>

namespace wp {

//----------------------------------------------------------
// mat
template<typename T>
struct quat_t;

template<unsigned Rows, unsigned Cols, typename Type>
struct mat_t {
    inline mat_t() = default;

    inline __device__ explicit mat_t(Type s) {
        for (unsigned i = 0; i < Rows; ++i)
            for (unsigned j = 0; j < Cols; ++j)
                data[i][j] = s;
    }

    template<typename OtherType>
    inline explicit __device__ mat_t(const mat_t<Rows, Cols, OtherType> &other) {
        for (unsigned i = 0; i < Rows; ++i)
            for (unsigned j = 0; j < Cols; ++j)
                data[i][j] = other.data[i][j];
    }

    inline __device__ mat_t(vec_t<2, Type> c0, vec_t<2, Type> c1) {
        data[0][0] = c0[0];
        data[1][0] = c0[1];

        data[0][1] = c1[0];
        data[1][1] = c1[1];
    }

    inline __device__ mat_t(vec_t<3, Type> c0, vec_t<3, Type> c1, vec_t<3, Type> c2) {
        data[0][0] = c0[0];
        data[1][0] = c0[1];
        data[2][0] = c0[2];

        data[0][1] = c1[0];
        data[1][1] = c1[1];
        data[2][1] = c1[2];

        data[0][2] = c2[0];
        data[1][2] = c2[1];
        data[2][2] = c2[2];
    }

    inline __device__ mat_t(vec_t<4, Type> c0, vec_t<4, Type> c1, vec_t<4, Type> c2, vec_t<4, Type> c3) {
        data[0][0] = c0[0];
        data[1][0] = c0[1];
        data[2][0] = c0[2];
        data[3][0] = c0[3];

        data[0][1] = c1[0];
        data[1][1] = c1[1];
        data[2][1] = c1[2];
        data[3][1] = c1[3];

        data[0][2] = c2[0];
        data[1][2] = c2[1];
        data[2][2] = c2[2];
        data[3][2] = c2[3];

        data[0][3] = c3[0];
        data[1][3] = c3[1];
        data[2][3] = c3[2];
        data[3][3] = c3[3];
    }

    inline __device__ mat_t(Type m00, Type m01, Type m10, Type m11) {
        data[0][0] = m00;
        data[1][0] = m10;
        data[0][1] = m01;
        data[1][1] = m11;
    }

    inline __device__ mat_t(
        Type m00, Type m01, Type m02,
        Type m10, Type m11, Type m12,
        Type m20, Type m21, Type m22) {
        data[0][0] = m00;
        data[1][0] = m10;
        data[2][0] = m20;

        data[0][1] = m01;
        data[1][1] = m11;
        data[2][1] = m21;

        data[0][2] = m02;
        data[1][2] = m12;
        data[2][2] = m22;
    }

    inline __device__ mat_t(
        Type m00, Type m01, Type m02, Type m03,
        Type m10, Type m11, Type m12, Type m13,
        Type m20, Type m21, Type m22, Type m23,
        Type m30, Type m31, Type m32, Type m33) {
        data[0][0] = m00;
        data[1][0] = m10;
        data[2][0] = m20;
        data[3][0] = m30;

        data[0][1] = m01;
        data[1][1] = m11;
        data[2][1] = m21;
        data[3][1] = m31;

        data[0][2] = m02;
        data[1][2] = m12;
        data[2][2] = m22;
        data[3][2] = m32;

        data[0][3] = m03;
        data[1][3] = m13;
        data[2][3] = m23;
        data[3][3] = m33;
    }

    // implemented in quat.h
    inline __device__ mat_t(const vec_t<3, Type> &pos, const quat_t<Type> &rot, const vec_t<3, Type> &scale);

    inline __device__ explicit mat_t(const initializer_array<Rows * Cols, Type> &l) {
        for (unsigned i = 0; i < Rows; ++i) {
            for (unsigned j = 0; j < Cols; ++j) {
                data[i][j] = l[i * Cols + j];
            }
        }
    }

    inline __device__ explicit mat_t(const initializer_array<Cols, vec_t<Rows, Type>> &l) {
        for (unsigned j = 0; j < Cols; ++j) {
            for (unsigned i = 0; i < Rows; ++i) {
                data[i][j] = l[j][i];
            }
        }
    }

    __device__ vec_t<Cols, Type> get_row(int index) const {
        return (vec_t<Cols, Type> &)data[index];
    }

    __device__ void set_row(int index, const vec_t<Cols, Type> &v) {
        (vec_t<Cols, Type> &)data[index] = v;
    }

    __device__ vec_t<Rows, Type> get_col(int index) const {
        vec_t<Rows, Type> ret;
        for (unsigned i = 0; i < Rows; ++i) {
            ret[i] = data[i][index];
        }
        return ret;
    }

    __device__ void set_col(int index, const vec_t<Rows, Type> &v) {
        for (unsigned i = 0; i < Rows; ++i) {
            data[i][index] = v[i];
        }
    }

    // row major storage assumed to be compatible with PyTorch
    Type data[Rows][Cols] = {};
};

template<unsigned Rows, typename Type>
inline __device__ mat_t<Rows, Rows, Type> identity() {
    mat_t<Rows, Rows, Type> m;
    for (unsigned i = 0; i < Rows; ++i) {
        m.data[i][i] = Type(1);
    }
    return m;
}

template<unsigned Rows, unsigned Cols, typename Type>
inline __device__ bool operator==(const mat_t<Rows, Cols, Type> &a, const mat_t<Rows, Cols, Type> &b) {
    for (unsigned i = 0; i < Rows; ++i)
        for (unsigned j = 0; j < Cols; ++j)
            if (a.data[i][j] != b.data[i][j])
                return false;

    return true;
}

// negation:
template<unsigned Rows, unsigned Cols, typename Type>
inline __device__ mat_t<Rows, Cols, Type> operator-(mat_t<Rows, Cols, Type> a) {
    // NB: this constructor will initialize all ret's components to 0, which is
    // unnecessary...
    mat_t<Rows, Cols, Type> ret;
    for (unsigned i = 0; i < Rows; ++i)
        for (unsigned j = 0; j < Cols; ++j)
            ret.data[i][j] = -a.data[i][j];

    // Wonder if this does a load of copying when it returns... hopefully not as it's inlined?
    return ret;
}

template<unsigned Rows, unsigned Cols, typename Type>
__device__ inline mat_t<Rows, Cols, Type> pos(const mat_t<Rows, Cols, Type> &x) {
    return x;
}

template<unsigned Rows, unsigned Cols, typename Type>
__device__ inline mat_t<Rows, Cols, Type> neg(const mat_t<Rows, Cols, Type> &x) {
    return -x;
}

template<unsigned Rows, unsigned Cols, typename Type>
inline __device__ mat_t<Rows, Cols, Type> atomic_add(mat_t<Rows, Cols, Type> *addr, mat_t<Rows, Cols, Type> value) {
    mat_t<Rows, Cols, Type> m;

    for (unsigned i = 0; i < Rows; ++i)
        for (unsigned j = 0; j < Cols; ++j)
            m.data[i][j] = atomic_add(&addr->data[i][j], value.data[i][j]);

    return m;
}

template<unsigned Rows, unsigned Cols, typename Type>
inline __device__ mat_t<Rows, Cols, Type> atomic_min(mat_t<Rows, Cols, Type> *addr, mat_t<Rows, Cols, Type> value) {
    mat_t<Rows, Cols, Type> m;

    for (unsigned i = 0; i < Rows; ++i)
        for (unsigned j = 0; j < Cols; ++j)
            m.data[i][j] = atomic_min(&addr->data[i][j], value.data[i][j]);

    return m;
}

template<unsigned Rows, unsigned Cols, typename Type>
inline __device__ mat_t<Rows, Cols, Type> atomic_max(mat_t<Rows, Cols, Type> *addr, mat_t<Rows, Cols, Type> value) {
    mat_t<Rows, Cols, Type> m;

    for (unsigned i = 0; i < Rows; ++i)
        for (unsigned j = 0; j < Cols; ++j)
            m.data[i][j] = atomic_max(&addr->data[i][j], value.data[i][j]);

    return m;
}

template<unsigned Rows, unsigned Cols, typename Type>
inline __device__ vec_t<Cols, Type> index(const mat_t<Rows, Cols, Type> &m, int row) {
    vec_t<Cols, Type> ret;
    for (unsigned i = 0; i < Cols; ++i) {
        ret.c[i] = m.data[row][i];
    }
    return ret;
}

template<unsigned Rows, unsigned Cols, typename Type>
inline __device__ Type index(const mat_t<Rows, Cols, Type> &m, int row, int col) {
#ifndef NDEBUG
    if (row < 0 || row >= Rows) {
        printf("mat row index %d out of bounds at %s %d\n", row, __FILE__, __LINE__);
        assert(0);
    }
    if (col < 0 || col >= Cols) {
        printf("mat col index %d out of bounds at %s %d\n", col, __FILE__, __LINE__);
        assert(0);
    }
#endif
    return m.data[row][col];
}

template<unsigned Rows, unsigned Cols, typename Type>
inline __device__ void indexset(mat_t<Rows, Cols, Type> &m, int row, vec_t<Cols, Type> value) {
#ifndef NDEBUG
    if (row < 0 || row >= Rows) {
        printf("mat row index %d out of bounds at %s %d\n", row, __FILE__, __LINE__);
        assert(0);
    }
#endif

    for (unsigned i = 0; i < Cols; ++i)
        m.data[row][i] = value[i];
}

template<unsigned Rows, unsigned Cols, typename Type>
inline __device__ void indexset(mat_t<Rows, Cols, Type> &m, int row, int col, Type value) {
#ifndef NDEBUG
    if (row < 0 || row >= Rows) {
        printf("mat row index %d out of bounds at %s %d\n", row, __FILE__, __LINE__);
        assert(0);
    }
    if (col < 0 || col >= Cols) {
        printf("mat col index %d out of bounds at %s %d\n", col, __FILE__, __LINE__);
        assert(0);
    }
#endif
    m.data[row][col] = value;
}

template<unsigned Rows, unsigned Cols, typename Type>
inline bool __device__ isfinite(const mat_t<Rows, Cols, Type> &m) {
    for (unsigned i = 0; i < Rows; ++i)
        for (unsigned j = 0; j < Cols; ++j)
            if (!isfinite(m.data[i][j]))
                return false;
    return true;
}

template<unsigned Rows, unsigned Cols, typename Type>
inline __device__ mat_t<Rows, Cols, Type> add(const mat_t<Rows, Cols, Type> &a, const mat_t<Rows, Cols, Type> &b) {
    mat_t<Rows, Cols, Type> t;
    for (unsigned i = 0; i < Rows; ++i) {
        for (unsigned j = 0; j < Cols; ++j) {
            t.data[i][j] = a.data[i][j] + b.data[i][j];
        }
    }

    return t;
}

template<unsigned Rows, unsigned Cols, typename Type>
inline __device__ mat_t<Rows, Cols, Type> sub(const mat_t<Rows, Cols, Type> &a, const mat_t<Rows, Cols, Type> &b) {
    mat_t<Rows, Cols, Type> t;
    for (unsigned i = 0; i < Rows; ++i) {
        for (unsigned j = 0; j < Cols; ++j) {
            t.data[i][j] = a.data[i][j] - b.data[i][j];
        }
    }

    return t;
}

template<unsigned Rows, unsigned Cols, typename Type>
inline __device__ mat_t<Rows, Cols, Type> div(const mat_t<Rows, Cols, Type> &a, Type b) {
    mat_t<Rows, Cols, Type> t;
    for (unsigned i = 0; i < Rows; ++i) {
        for (unsigned j = 0; j < Cols; ++j) {
            t.data[i][j] = a.data[i][j] / b;
        }
    }

    return t;
}

template<unsigned Rows, unsigned Cols, typename Type>
inline __device__ mat_t<Rows, Cols, Type> mul(const mat_t<Rows, Cols, Type> &a, Type b) {
    mat_t<Rows, Cols, Type> t;
    for (unsigned i = 0; i < Rows; ++i) {
        for (unsigned j = 0; j < Cols; ++j) {
            t.data[i][j] = a.data[i][j] * b;
        }
    }

    return t;
}

template<unsigned Rows, unsigned Cols, typename Type>
inline __device__ mat_t<Rows, Cols, Type> mul(Type b, const mat_t<Rows, Cols, Type> &a) {
    return mul(a, b);
}

template<unsigned Rows, unsigned Cols, typename Type>
inline __device__ mat_t<Rows, Cols, Type> operator*(Type b, const mat_t<Rows, Cols, Type> &a) {
    return mul(a, b);
}

template<unsigned Rows, unsigned Cols, typename Type>
inline __device__ mat_t<Rows, Cols, Type> operator*(const mat_t<Rows, Cols, Type> &a, Type b) {
    return mul(a, b);
}

template<unsigned Rows, unsigned Cols, typename Type>
inline __device__ vec_t<Rows, Type> mul(const mat_t<Rows, Cols, Type> &a, const vec_t<Cols, Type> &b) {
    vec_t<Rows, Type> r = a.get_col(0) * b[0];
    for (unsigned i = 1; i < Cols; ++i) {
        r += a.get_col(i) * b[i];
    }
    return r;
}

template<unsigned Rows, unsigned Cols, unsigned ColsOut, typename Type>
inline __device__ mat_t<Rows, ColsOut, Type> mul(const mat_t<Rows, Cols, Type> &a, const mat_t<Cols, ColsOut, Type> &b) {
    mat_t<Rows, ColsOut, Type> t(0);
    for (unsigned i = 0; i < Rows; ++i) {
        for (unsigned j = 0; j < ColsOut; ++j) {
            for (unsigned k = 0; k < Cols; ++k) {
                t.data[i][j] += a.data[i][k] * b.data[k][j];
            }
        }
    }

    return t;
}

template<unsigned Rows, unsigned Cols, typename Type>
inline __device__ Type ddot(const mat_t<Rows, Cols, Type> &a, const mat_t<Rows, Cols, Type> &b) {
    // double dot product between a and b:
    Type r(0);
    for (unsigned i = 0; i < Rows; ++i) {
        for (unsigned j = 0; j < Cols; ++j) {
            r += a.data[i][j] * b.data[i][j];
        }
    }
    return r;
}

template<unsigned Rows, unsigned Cols, typename Type>
inline __device__ Type tensordot(const mat_t<Rows, Cols, Type> &a, const mat_t<Rows, Cols, Type> &b) {
    // corresponds to `np.tensordot()` with all axes being contracted
    return ddot(a, b);
}

template<unsigned Rows, unsigned Cols, typename Type>
inline __device__ mat_t<Cols, Rows, Type> transpose(const mat_t<Rows, Cols, Type> &a) {
    mat_t<Cols, Rows, Type> t;
    for (unsigned i = 0; i < Cols; ++i) {
        for (unsigned j = 0; j < Rows; ++j) {
            t.data[i][j] = a.data[j][i];
        }
    }

    return t;
}

// Only implementing determinants for 2x2, 3x3 and 4x4 matrices for now...
template<typename Type>
inline __device__ Type determinant(const mat_t<2, 2, Type> &m) {
    return m.data[0][0] * m.data[1][1] - m.data[1][0] * m.data[0][1];
}

template<typename Type>
inline __device__ Type determinant(const mat_t<3, 3, Type> &m) {
    return dot(
        vec_t<3, Type>(m.data[0][0], m.data[0][1], m.data[0][2]),
        cross(
            vec_t<3, Type>(m.data[1][0], m.data[1][1], m.data[1][2]),
            vec_t<3, Type>(m.data[2][0], m.data[2][1], m.data[2][2])));
}

template<typename Type>
inline __device__ Type determinant(const mat_t<4, 4, Type> &m) {
    // adapted from USD GfMatrix4f::Inverse()
    Type x00, x01, x02, x03;
    Type x10, x11, x12, x13;
    Type x20, x21, x22, x23;
    Type x30, x31, x32, x33;
    double y01, y02, y03, y12, y13, y23;
    Type z00, z10, z20, z30;

    // Pickle 1st two columns of matrix into registers
    x00 = m.data[0][0];
    x01 = m.data[0][1];
    x10 = m.data[1][0];
    x11 = m.data[1][1];
    x20 = m.data[2][0];
    x21 = m.data[2][1];
    x30 = m.data[3][0];
    x31 = m.data[3][1];

    // Compute all six 2x2 determinants of 1st two columns
    y01 = x00 * x11 - x10 * x01;
    y02 = x00 * x21 - x20 * x01;
    y03 = x00 * x31 - x30 * x01;
    y12 = x10 * x21 - x20 * x11;
    y13 = x10 * x31 - x30 * x11;
    y23 = x20 * x31 - x30 * x21;

    // Pickle 2nd two columns of matrix into registers
    x02 = m.data[0][2];
    x03 = m.data[0][3];
    x12 = m.data[1][2];
    x13 = m.data[1][3];
    x22 = m.data[2][2];
    x23 = m.data[2][3];
    x32 = m.data[3][2];
    x33 = m.data[3][3];

    // Compute all six 2x2 determinants of 2nd two columns
    y01 = x02 * x13 - x12 * x03;
    y02 = x02 * x23 - x22 * x03;
    y03 = x02 * x33 - x32 * x03;
    y12 = x12 * x23 - x22 * x13;
    y13 = x12 * x33 - x32 * x13;
    y23 = x22 * x33 - x32 * x23;

    // Compute all 3x3 cofactors for 1st two columns
    z30 = x11 * y02 - x21 * y01 - x01 * y12;
    z20 = x01 * y13 - x11 * y03 + x31 * y01;
    z10 = x21 * y03 - x31 * y02 - x01 * y23;
    z00 = x11 * y23 - x21 * y13 + x31 * y12;

    // compute 4x4 determinant & its reciprocal
    double det = x30 * z30 + x20 * z20 + x10 * z10 + x00 * z00;
    return det;
}

template<unsigned Rows, typename Type>
inline __device__ Type trace(const mat_t<Rows, Rows, Type> &m) {
    Type ret = m.data[0][0];
    for (unsigned i = 1; i < Rows; ++i) {
        ret += m.data[i][i];
    }
    return ret;
}

template<unsigned Rows, typename Type>
inline __device__ vec_t<Rows, Type> get_diag(const mat_t<Rows, Rows, Type> &m) {
    vec_t<Rows, Type> ret;
    for (unsigned i = 0; i < Rows; ++i) {
        ret[i] = m.data[i][i];
    }
    return ret;
}

// Only implementing inverses for 2x2, 3x3 and 4x4 matrices for now...
template<typename Type>
inline __device__ mat_t<2, 2, Type> inverse(const mat_t<2, 2, Type> &m) {
    Type det = determinant(m);
    if (det > Type(EPSILON) || det < -Type(EPSILON)) {
        return mat_t<2, 2, Type>(m.data[1][1], -m.data[0][1],
                                 -m.data[1][0], m.data[0][0]) *
               (Type(1.0f) / det);
    } else {
        return mat_t<2, 2, Type>();
    }
}

template<typename Type>
inline __device__ mat_t<3, 3, Type> inverse(const mat_t<3, 3, Type> &m) {
    Type det = determinant(m);

    if (det != Type(0.0f)) {
        mat_t<3, 3, Type> b;

        b.data[0][0] = m.data[1][1] * m.data[2][2] - m.data[1][2] * m.data[2][1];
        b.data[1][0] = m.data[1][2] * m.data[2][0] - m.data[1][0] * m.data[2][2];
        b.data[2][0] = m.data[1][0] * m.data[2][1] - m.data[1][1] * m.data[2][0];

        b.data[0][1] = m.data[0][2] * m.data[2][1] - m.data[0][1] * m.data[2][2];
        b.data[1][1] = m.data[0][0] * m.data[2][2] - m.data[0][2] * m.data[2][0];
        b.data[2][1] = m.data[0][1] * m.data[2][0] - m.data[0][0] * m.data[2][1];

        b.data[0][2] = m.data[0][1] * m.data[1][2] - m.data[0][2] * m.data[1][1];
        b.data[1][2] = m.data[0][2] * m.data[1][0] - m.data[0][0] * m.data[1][2];
        b.data[2][2] = m.data[0][0] * m.data[1][1] - m.data[0][1] * m.data[1][0];

        return b * (Type(1.0f) / det);
    } else {
        return mat_t<3, 3, Type>();
    }
}

template<typename Type>
inline __device__ mat_t<4, 4, Type> inverse(const mat_t<4, 4, Type> &m) {
    // adapted from USD GfMatrix4f::Inverse()
    Type x00, x01, x02, x03;
    Type x10, x11, x12, x13;
    Type x20, x21, x22, x23;
    Type x30, x31, x32, x33;
    double y01, y02, y03, y12, y13, y23;
    Type z00, z10, z20, z30;
    Type z01, z11, z21, z31;
    double z02, z03, z12, z13, z22, z23, z32, z33;

    // Pickle 1st two columns of matrix into registers
    x00 = m.data[0][0];
    x01 = m.data[0][1];
    x10 = m.data[1][0];
    x11 = m.data[1][1];
    x20 = m.data[2][0];
    x21 = m.data[2][1];
    x30 = m.data[3][0];
    x31 = m.data[3][1];

    // Compute all six 2x2 determinants of 1st two columns
    y01 = x00 * x11 - x10 * x01;
    y02 = x00 * x21 - x20 * x01;
    y03 = x00 * x31 - x30 * x01;
    y12 = x10 * x21 - x20 * x11;
    y13 = x10 * x31 - x30 * x11;
    y23 = x20 * x31 - x30 * x21;

    // Pickle 2nd two columns of matrix into registers
    x02 = m.data[0][2];
    x03 = m.data[0][3];
    x12 = m.data[1][2];
    x13 = m.data[1][3];
    x22 = m.data[2][2];
    x23 = m.data[2][3];
    x32 = m.data[3][2];
    x33 = m.data[3][3];

    // Compute all 3x3 cofactors for 2nd two columns */
    z33 = x02 * y12 - x12 * y02 + x22 * y01;
    z23 = x12 * y03 - x32 * y01 - x02 * y13;
    z13 = x02 * y23 - x22 * y03 + x32 * y02;
    z03 = x22 * y13 - x32 * y12 - x12 * y23;
    z32 = x13 * y02 - x23 * y01 - x03 * y12;
    z22 = x03 * y13 - x13 * y03 + x33 * y01;
    z12 = x23 * y03 - x33 * y02 - x03 * y23;
    z02 = x13 * y23 - x23 * y13 + x33 * y12;

    // Compute all six 2x2 determinants of 2nd two columns
    y01 = x02 * x13 - x12 * x03;
    y02 = x02 * x23 - x22 * x03;
    y03 = x02 * x33 - x32 * x03;
    y12 = x12 * x23 - x22 * x13;
    y13 = x12 * x33 - x32 * x13;
    y23 = x22 * x33 - x32 * x23;

    // Compute all 3x3 cofactors for 1st two columns
    z30 = x11 * y02 - x21 * y01 - x01 * y12;
    z20 = x01 * y13 - x11 * y03 + x31 * y01;
    z10 = x21 * y03 - x31 * y02 - x01 * y23;
    z00 = x11 * y23 - x21 * y13 + x31 * y12;
    z31 = x00 * y12 - x10 * y02 + x20 * y01;
    z21 = x10 * y03 - x30 * y01 - x00 * y13;
    z11 = x00 * y23 - x20 * y03 + x30 * y02;
    z01 = x20 * y13 - x30 * y12 - x10 * y23;

    // compute 4x4 determinant & its reciprocal
    double det = x30 * z30 + x20 * z20 + x10 * z10 + x00 * z00;

    if (fabs(det) > EPSILON) {
        mat_t<4, 4, Type> invm;

        double rcp = 1.0 / det;

        // Multiply all 3x3 cofactors by reciprocal & transpose
        invm.data[0][0] = Type(z00 * rcp);
        invm.data[0][1] = Type(z10 * rcp);
        invm.data[1][0] = Type(z01 * rcp);
        invm.data[0][2] = Type(z20 * rcp);
        invm.data[2][0] = Type(z02 * rcp);
        invm.data[0][3] = Type(z30 * rcp);
        invm.data[3][0] = Type(z03 * rcp);
        invm.data[1][1] = Type(z11 * rcp);
        invm.data[1][2] = Type(z21 * rcp);
        invm.data[2][1] = Type(z12 * rcp);
        invm.data[1][3] = Type(z31 * rcp);
        invm.data[3][1] = Type(z13 * rcp);
        invm.data[2][2] = Type(z22 * rcp);
        invm.data[2][3] = Type(z32 * rcp);
        invm.data[3][2] = Type(z23 * rcp);
        invm.data[3][3] = Type(z33 * rcp);

        return invm;
    } else {
        return mat_t<4, 4, Type>();
    }
}

template<unsigned Rows, typename Type>
inline __device__ mat_t<Rows, Rows, Type> diag(const vec_t<Rows, Type> &d) {
    mat_t<Rows, Rows, Type> ret(Type(0));
    for (unsigned i = 0; i < Rows; ++i) {
        ret.data[i][i] = d[i];
    }
    return ret;
}

template<unsigned Rows, unsigned Cols, typename Type>
inline __device__ mat_t<Rows, Cols, Type> outer(const vec_t<Rows, Type> &a, const vec_t<Cols, Type> &b) {
    // col 0 = a * b[0] etc...
    mat_t<Rows, Cols, Type> ret;
    for (unsigned row = 0; row < Rows; ++row) {
        for (unsigned col = 0; col < Cols; ++col)// columns
        {
            ret.data[row][col] = a[row] * b[col];
        }
    }
    return ret;
}

template<typename Type>
inline __device__ mat_t<3, 3, Type> skew(const vec_t<3, Type> &a) {
    mat_t<3, 3, Type> out(
        Type(0), -a[2], a[1],
        a[2], Type(0), -a[0],
        -a[1], a[0], Type(0));

    return out;
}

template<unsigned Rows, unsigned Cols, typename Type>
inline __device__ mat_t<Rows, Cols, Type> cw_mul(const mat_t<Rows, Cols, Type> &a, const mat_t<Rows, Cols, Type> &b) {
    mat_t<Rows, Cols, Type> t;
    for (unsigned i = 0; i < Rows; ++i) {
        for (unsigned j = 0; j < Cols; ++j) {
            t.data[i][j] = a.data[i][j] * b.data[i][j];
        }
    }

    return t;
}

template<unsigned Rows, unsigned Cols, typename Type>
inline __device__ mat_t<Rows, Cols, Type> cw_div(const mat_t<Rows, Cols, Type> &a, const mat_t<Rows, Cols, Type> &b) {
    mat_t<Rows, Cols, Type> t;
    for (unsigned i = 0; i < Rows; ++i) {
        for (unsigned j = 0; j < Cols; ++j) {
            t.data[i][j] = a.data[i][j] / b.data[i][j];
        }
    }

    return t;
}

template<typename Type>
inline __device__ vec_t<3, Type> transform_point(const mat_t<4, 4, Type> &m, const vec_t<3, Type> &v) {
    vec_t<4, Type> out = mul(m, vec_t<4, Type>(v[0], v[1], v[2], Type(1)));
    return vec_t<3, Type>(out[0], out[1], out[2]);
}

template<typename Type>
inline __device__ vec_t<3, Type> transform_vector(const mat_t<4, 4, Type> &m, const vec_t<3, Type> &v) {
    vec_t<4, Type> out = mul(m, vec_t<4, Type>(v[0], v[1], v[2], 0.f));
    return vec_t<3, Type>(out[0], out[1], out[2]);
}

template<unsigned Rows, unsigned Cols, typename Type>
__device__ inline mat_t<Rows, Cols, Type> lerp(const mat_t<Rows, Cols, Type> &a, const mat_t<Rows, Cols, Type> &b, Type t) {
    return a * (Type(1) - t) + b * t;
}

using mat22 = mat_t<2, 2, float>;
using mat33 = mat_t<3, 3, float>;
using mat44 = mat_t<4, 4, float>;

using mat22f = mat_t<2, 2, float>;
using mat33f = mat_t<3, 3, float>;
using mat44f = mat_t<4, 4, float>;

using mat22d = mat_t<2, 2, double>;
using mat33d = mat_t<3, 3, double>;
using mat44d = mat_t<4, 4, double>;

template<unsigned Rows, unsigned Cols, typename Type>
inline CUDA_CALLABLE void print(const mat_t<Rows, Cols, Type> &m) {
    for (unsigned i = 0; i < Rows; ++i) {
        for (unsigned j = 0; j < Cols; ++j) {
            printf("%g ", float(m.data[i][j]));
        }
        printf("\n");
    }
}

}// namespace wp
