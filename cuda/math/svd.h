//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "quat.h"

namespace wp {

template<typename Type>
inline __device__
    Type
    accurateSqrt(Type x) {
    return x / sqrt(x);
}

template<typename Type>
inline __device__ void condSwap(bool c, Type &X, Type &Y) {
    // used in step 2
    Type Z = X;
    X = c ? Y : X;
    Y = c ? Z : Y;
}

template<typename Type>
inline __device__ void condNegSwap(bool c, Type &X, Type &Y) {
    // used in step 2 and 3
    Type Z = -X;
    X = c ? Y : X;
    Y = c ? Z : Y;
}

// matrix multiplication M = A * B
template<typename Type>
inline __device__ void multAB(Type a11, Type a12, Type a13,
                              Type a21, Type a22, Type a23,
                              Type a31, Type a32, Type a33,
                              //
                              Type b11, Type b12, Type b13,
                              Type b21, Type b22, Type b23,
                              Type b31, Type b32, Type b33,
                              //
                              Type &m11, Type &m12, Type &m13,
                              Type &m21, Type &m22, Type &m23,
                              Type &m31, Type &m32, Type &m33) {

    m11 = a11 * b11 + a12 * b21 + a13 * b31;
    m12 = a11 * b12 + a12 * b22 + a13 * b32;
    m13 = a11 * b13 + a12 * b23 + a13 * b33;
    m21 = a21 * b11 + a22 * b21 + a23 * b31;
    m22 = a21 * b12 + a22 * b22 + a23 * b32;
    m23 = a21 * b13 + a22 * b23 + a23 * b33;
    m31 = a31 * b11 + a32 * b21 + a33 * b31;
    m32 = a31 * b12 + a32 * b22 + a33 * b32;
    m33 = a31 * b13 + a32 * b23 + a33 * b33;
}

// matrix multiplication M = Transpose[A] * B
template<typename Type>
inline __device__ void multAtB(Type a11, Type a12, Type a13,
                               Type a21, Type a22, Type a23,
                               Type a31, Type a32, Type a33,
                               //
                               Type b11, Type b12, Type b13,
                               Type b21, Type b22, Type b23,
                               Type b31, Type b32, Type b33,
                               //
                               Type &m11, Type &m12, Type &m13,
                               Type &m21, Type &m22, Type &m23,
                               Type &m31, Type &m32, Type &m33) {
    m11 = a11 * b11 + a21 * b21 + a31 * b31;
    m12 = a11 * b12 + a21 * b22 + a31 * b32;
    m13 = a11 * b13 + a21 * b23 + a31 * b33;
    m21 = a12 * b11 + a22 * b21 + a32 * b31;
    m22 = a12 * b12 + a22 * b22 + a32 * b32;
    m23 = a12 * b13 + a22 * b23 + a32 * b33;
    m31 = a13 * b11 + a23 * b21 + a33 * b31;
    m32 = a13 * b12 + a23 * b22 + a33 * b32;
    m33 = a13 * b13 + a23 * b23 + a33 * b33;
}

template<typename Type>
inline __device__ void quatToMat3(const Type *qV,
                                  Type &m11, Type &m12, Type &m13,
                                  Type &m21, Type &m22, Type &m23,
                                  Type &m31, Type &m32, Type &m33) {
    Type w = qV[3];
    Type x = qV[0];
    Type y = qV[1];
    Type z = qV[2];

    Type qxx = x * x;
    Type qyy = y * y;
    Type qzz = z * z;
    Type qxz = x * z;
    Type qxy = x * y;
    Type qyz = y * z;
    Type qwx = w * x;
    Type qwy = w * y;
    Type qwz = w * z;

    m11 = Type(1) - Type(2) * (qyy + qzz);
    m12 = Type(2) * (qxy - qwz);
    m13 = Type(2) * (qxz + qwy);
    m21 = Type(2) * (qxy + qwz);
    m22 = Type(1) - Type(2) * (qxx + qzz);
    m23 = Type(2) * (qyz - qwx);
    m31 = Type(2) * (qxz - qwy);
    m32 = Type(2) * (qyz + qwx);
    m33 = Type(1) - Type(2) * (qxx + qyy);
}

template<typename Type>
inline __device__ void approximateGivensQuaternion(Type a11, Type a12, Type a22, Type &ch, Type &sh) {
    /*
     * Given givens angle computed by approximateGivensAngles,
     * compute the corresponding rotation quaternion.
     */
    ch = Type(2) * (a11 - a22);
    sh = a12;
    bool b = gamma * sh * sh < ch * ch;
    Type w = Type(1) / sqrt(ch * ch + sh * sh);
    ch = b ? w * ch : Type(CSTAR);
    sh = b ? w * sh : Type(SSTAR);
}

template<typename Type>
inline __device__ void jacobiConjugation(const int x, const int y, const int z,
                                         Type &s11,
                                         Type &s21, Type &s22,
                                         Type &s31, Type &s32, Type &s33,
                                         Type *qV) {
    Type ch, sh;
    approximateGivensQuaternion(s11, s21, s22, ch, sh);

    Type scale = ch * ch + sh * sh;
    Type a = (ch * ch - sh * sh) / scale;
    Type b = (Type(2) * sh * ch) / scale;

    // make temp copy of S
    Type _s11 = s11;
    Type _s21 = s21;
    Type _s22 = s22;
    Type _s31 = s31;
    Type _s32 = s32;
    Type _s33 = s33;

    // perform conjugation S = Q'*S*Q
    // Q already implicitly solved from a, b
    s11 = a * (a * _s11 + b * _s21) + b * (a * _s21 + b * _s22);
    s21 = a * (-b * _s11 + a * _s21) + b * (-b * _s21 + a * _s22);
    s22 = -b * (-b * _s11 + a * _s21) + a * (-b * _s21 + a * _s22);
    s31 = a * _s31 + b * _s32;
    s32 = -b * _s31 + a * _s32;
    s33 = _s33;

    // update cumulative rotation qV
    Type tmp[3];
    tmp[0] = qV[0] * sh;
    tmp[1] = qV[1] * sh;
    tmp[2] = qV[2] * sh;
    sh *= qV[3];

    qV[0] *= ch;
    qV[1] *= ch;
    qV[2] *= ch;
    qV[3] *= ch;

    // (x,y,z) corresponds to ((0,1,2),(1,2,0),(2,0,1))
    // for (p,q) = ((0,1),(1,2),(0,2))
    qV[z] += sh;
    qV[3] -= tmp[z];// w
    qV[x] += tmp[y];
    qV[y] -= tmp[x];

    // re-arrange matrix for next iteration
    _s11 = s22;
    _s21 = s32;
    _s22 = s33;
    _s31 = s21;
    _s32 = s31;
    _s33 = s11;
    s11 = _s11;
    s21 = _s21;
    s22 = _s22;
    s31 = _s31;
    s32 = _s32;
    s33 = _s33;
}

template<typename Type>
inline __device__
    Type
    dist2(Type x, Type y, Type z) {
    return x * x + y * y + z * z;
}

// finds transformation that diagonalizes a symmetric matrix
template<typename Type>
inline __device__ void jacobiEigenanlysis(// symmetric matrix
    Type &s11,
    Type &s21, Type &s22,
    Type &s31, Type &s32, Type &s33,
    // quaternion representation of V
    Type *qV) {
    qV[3] = 1;
    qV[0] = 0;
    qV[1] = 0;
    qV[2] = 0;// follow same indexing convention as GLM
    for (int i = 0; i < 4; i++) {
        // we wish to eliminate the maximum off-diagonal element
        // on every iteration, but cycling over all 3 possible rotations
        // in fixed order (p,q) = (1,2) , (2,3), (1,3) still retains
        //  asymptotic convergence
        jacobiConjugation(0, 1, 2, s11, s21, s22, s31, s32, s33, qV);// p,q = 0,1
        jacobiConjugation(1, 2, 0, s11, s21, s22, s31, s32, s33, qV);// p,q = 1,2
        jacobiConjugation(2, 0, 1, s11, s21, s22, s31, s32, s33, qV);// p,q = 0,2
    }
}

template<typename Type>
inline __device__ void sortSingularValues(// matrix that we want to decompose
    Type &b11, Type &b12, Type &b13,
    Type &b21, Type &b22, Type &b23,
    Type &b31, Type &b32, Type &b33,
    // sort V simultaneously
    Type &v11, Type &v12, Type &v13,
    Type &v21, Type &v22, Type &v23,
    Type &v31, Type &v32, Type &v33) {
    Type rho1 = dist2(b11, b21, b31);
    Type rho2 = dist2(b12, b22, b32);
    Type rho3 = dist2(b13, b23, b33);
    bool c;
    c = rho1 < rho2;
    condNegSwap(c, b11, b12);
    condNegSwap(c, v11, v12);
    condNegSwap(c, b21, b22);
    condNegSwap(c, v21, v22);
    condNegSwap(c, b31, b32);
    condNegSwap(c, v31, v32);
    condSwap(c, rho1, rho2);
    c = rho1 < rho3;
    condNegSwap(c, b11, b13);
    condNegSwap(c, v11, v13);
    condNegSwap(c, b21, b23);
    condNegSwap(c, v21, v23);
    condNegSwap(c, b31, b33);
    condNegSwap(c, v31, v33);
    condSwap(c, rho1, rho3);
    c = rho2 < rho3;
    condNegSwap(c, b12, b13);
    condNegSwap(c, v12, v13);
    condNegSwap(c, b22, b23);
    condNegSwap(c, v22, v23);
    condNegSwap(c, b32, b33);
    condNegSwap(c, v32, v33);
}

template<typename Type>
inline __device__ void QRGivensQuaternion(Type a1, Type a2, Type &ch, Type &sh) {
    // a1 = pivot point on diagonal
    // a2 = lower triangular entry we want to annihilate
    Type epsilon = EPSILON;
    Type rho = accurateSqrt(a1 * a1 + a2 * a2);

    sh = rho > epsilon ? a2 : Type(0);
    ch = abs(a1) + max(rho, epsilon);
    bool b = a1 < Type(0);
    condSwap(b, sh, ch);
    Type w = Type(1) / sqrt(ch * ch + sh * sh);
    ch *= w;
    sh *= w;
}

template<typename Type>
inline __device__ void QRDecomposition(// matrix that we want to decompose
    Type b11, Type b12, Type b13,
    Type b21, Type b22, Type b23,
    Type b31, Type b32, Type b33,
    // output Q
    Type &q11, Type &q12, Type &q13,
    Type &q21, Type &q22, Type &q23,
    Type &q31, Type &q32, Type &q33,
    // output R
    Type &r11, Type &r12, Type &r13,
    Type &r21, Type &r22, Type &r23,
    Type &r31, Type &r32, Type &r33) {
    Type ch1, sh1, ch2, sh2, ch3, sh3;
    Type a, b;

    // first givens rotation (ch,0,0,sh)
    QRGivensQuaternion(b11, b21, ch1, sh1);
    a = Type(1) - Type(2) * sh1 * sh1;
    b = Type(2) * ch1 * sh1;
    // apply B = Q' * B
    r11 = a * b11 + b * b21;
    r12 = a * b12 + b * b22;
    r13 = a * b13 + b * b23;
    r21 = -b * b11 + a * b21;
    r22 = -b * b12 + a * b22;
    r23 = -b * b13 + a * b23;
    r31 = b31;
    r32 = b32;
    r33 = b33;

    // second givens rotation (ch,0,-sh,0)
    QRGivensQuaternion(r11, r31, ch2, sh2);
    a = Type(1) - Type(2) * sh2 * sh2;
    b = Type(2) * ch2 * sh2;
    // apply B = Q' * B;
    b11 = a * r11 + b * r31;
    b12 = a * r12 + b * r32;
    b13 = a * r13 + b * r33;
    b21 = r21;
    b22 = r22;
    b23 = r23;
    b31 = -b * r11 + a * r31;
    b32 = -b * r12 + a * r32;
    b33 = -b * r13 + a * r33;

    // third givens rotation (ch,sh,0,0)
    QRGivensQuaternion(b22, b32, ch3, sh3);
    a = Type(1) - Type(2) * sh3 * sh3;
    b = Type(2) * ch3 * sh3;
    // R is now set to desired value
    r11 = b11;
    r12 = b12;
    r13 = b13;
    r21 = a * b21 + b * b31;
    r22 = a * b22 + b * b32;
    r23 = a * b23 + b * b33;
    r31 = -b * b21 + a * b31;
    r32 = -b * b22 + a * b32;
    r33 = -b * b23 + a * b33;

    // construct the cumulative rotation Q=Q1 * Q2 * Q3
    // the number of floating point operations for three quaternion multiplications
    // is more or less comparable to the explicit form of the joined matrix.
    // certainly more memory-efficient!
    Type sh12 = sh1 * sh1;
    Type sh22 = sh2 * sh2;
    Type sh32 = sh3 * sh3;

    q11 = (Type(-1) + Type(2) * sh12) * (Type(-1) + Type(2) * sh22);
    q12 = Type(4) * ch2 * ch3 * (Type(-1) + Type(2) * sh12) * sh2 * sh3 + Type(2) * ch1 * sh1 * (Type(-1) + Type(2) * sh32);
    q13 = Type(4) * ch1 * ch3 * sh1 * sh3 - Type(2) * ch2 * (Type(-1) + Type(2) * sh12) * sh2 * (Type(-1) + Type(2) * sh32);

    q21 = Type(2) * ch1 * sh1 * (Type(1) - Type(2) * sh22);
    q22 = Type(-8) * ch1 * ch2 * ch3 * sh1 * sh2 * sh3 + (Type(-1) + Type(2) * sh12) * (Type(-1) + Type(2) * sh32);
    q23 = Type(-2) * ch3 * sh3 + Type(4) * sh1 * (ch3 * sh1 * sh3 + ch1 * ch2 * sh2 * (Type(-1) + Type(2) * sh32));

    q31 = Type(2) * ch2 * sh2;
    q32 = Type(2) * ch3 * (Type(1) - Type(2) * sh22) * sh3;
    q33 = (Type(-1) + Type(2) * sh22) * (Type(-1) + Type(2) * sh32);
}

template<typename Type>
inline __device__ void _svd(// input A
    Type a11, Type a12, Type a13,
    Type a21, Type a22, Type a23,
    Type a31, Type a32, Type a33,
    // output U
    Type &u11, Type &u12, Type &u13,
    Type &u21, Type &u22, Type &u23,
    Type &u31, Type &u32, Type &u33,
    // output S
    Type &s11, Type &s12, Type &s13,
    Type &s21, Type &s22, Type &s23,
    Type &s31, Type &s32, Type &s33,
    // output V
    Type &v11, Type &v12, Type &v13,
    Type &v21, Type &v22, Type &v23,
    Type &v31, Type &v32, Type &v33) {
    // normal equations matrix
    Type ATA11, ATA12, ATA13;
    Type ATA21, ATA22, ATA23;
    Type ATA31, ATA32, ATA33;

    multAtB(a11, a12, a13, a21, a22, a23, a31, a32, a33,
            a11, a12, a13, a21, a22, a23, a31, a32, a33,
            ATA11, ATA12, ATA13, ATA21, ATA22, ATA23, ATA31, ATA32, ATA33);

    // symmetric eigenalysis
    Type qV[4];
    jacobiEigenanlysis(ATA11, ATA21, ATA22, ATA31, ATA32, ATA33, qV);
    quatToMat3(qV, v11, v12, v13, v21, v22, v23, v31, v32, v33);

    Type b11, b12, b13;
    Type b21, b22, b23;
    Type b31, b32, b33;
    multAB(a11, a12, a13, a21, a22, a23, a31, a32, a33,
           v11, v12, v13, v21, v22, v23, v31, v32, v33,
           b11, b12, b13, b21, b22, b23, b31, b32, b33);

    // sort singular values and find V
    sortSingularValues(b11, b12, b13, b21, b22, b23, b31, b32, b33,
                       v11, v12, v13, v21, v22, v23, v31, v32, v33);

    // QR decomposition
    QRDecomposition(b11, b12, b13, b21, b22, b23, b31, b32, b33,
                    u11, u12, u13, u21, u22, u23, u31, u32, u33,
                    s11, s12, s13, s21, s22, s23, s31, s32, s33);
}

template<typename Type>
inline __device__ void svd3(const mat_t<3, 3, Type> &A, mat_t<3, 3, Type> &U, vec_t<3, Type> &sigma, mat_t<3, 3, Type> &V) {
    Type s12, s13, s21, s23, s31, s32;
    _svd(A.data[0][0], A.data[0][1], A.data[0][2],
         A.data[1][0], A.data[1][1], A.data[1][2],
         A.data[2][0], A.data[2][1], A.data[2][2],

         U.data[0][0], U.data[0][1], U.data[0][2],
         U.data[1][0], U.data[1][1], U.data[1][2],
         U.data[2][0], U.data[2][1], U.data[2][2],

         sigma[0], s12, s13,
         s21, sigma[1], s23,
         s31, s32, sigma[2],

         V.data[0][0], V.data[0][1], V.data[0][2],
         V.data[1][0], V.data[1][1], V.data[1][2],
         V.data[2][0], V.data[2][1], V.data[2][2]);
}

template<typename Type>
inline __device__ void qr3(const mat_t<3, 3, Type> &A, mat_t<3, 3, Type> &Q, mat_t<3, 3, Type> &R) {
    QRDecomposition(A.data[0][0], A.data[0][1], A.data[0][2],
                    A.data[1][0], A.data[1][1], A.data[1][2],
                    A.data[2][0], A.data[2][1], A.data[2][2],

                    Q.data[0][0], Q.data[0][1], Q.data[0][2],
                    Q.data[1][0], Q.data[1][1], Q.data[1][2],
                    Q.data[2][0], Q.data[2][1], Q.data[2][2],

                    R.data[0][0], R.data[0][1], R.data[0][2],
                    R.data[1][0], R.data[1][1], R.data[1][2],
                    R.data[2][0], R.data[2][1], R.data[2][2]);
}

template<typename Type>
inline __device__ void eig3(const mat_t<3, 3, Type> &A, mat_t<3, 3, Type> &Q, vec_t<3, Type> &d) {
    Type qV[4];
    Type s11 = A.data[0][0];
    Type s21 = A.data[1][0];
    Type s22 = A.data[1][1];
    Type s31 = A.data[2][0];
    Type s32 = A.data[2][1];
    Type s33 = A.data[2][2];

    jacobiEigenanlysis(s11, s21, s22, s31, s32, s33, qV);
    quatToMat3(qV, Q.data[0][0], Q.data[0][1], Q.data[0][2], Q.data[1][0], Q.data[1][1], Q.data[1][2], Q.data[2][0], Q.data[2][1], Q.data[2][2]);
    mat_t<3, 3, Type> t;
    multAtB(Q.data[0][0], Q.data[0][1], Q.data[0][2], Q.data[1][0], Q.data[1][1], Q.data[1][2], Q.data[2][0], Q.data[2][1], Q.data[2][2],
            A.data[0][0], A.data[0][1], A.data[0][2], A.data[1][0], A.data[1][1], A.data[1][2], A.data[2][0], A.data[2][1], A.data[2][2],
            t.data[0][0], t.data[0][1], t.data[0][2], t.data[1][0], t.data[1][1], t.data[1][2], t.data[2][0], t.data[2][1], t.data[2][2]);

    mat_t<3, 3, Type> u;
    multAB(t.data[0][0], t.data[0][1], t.data[0][2], t.data[1][0], t.data[1][1], t.data[1][2], t.data[2][0], t.data[2][1], t.data[2][2],
           Q.data[0][0], Q.data[0][1], Q.data[0][2], Q.data[1][0], Q.data[1][1], Q.data[1][2], Q.data[2][0], Q.data[2][1], Q.data[2][2],
           u.data[0][0], u.data[0][1], u.data[0][2], u.data[1][0], u.data[1][1], u.data[1][2], u.data[2][0], u.data[2][1], u.data[2][2]);
    d = vec_t<3, Type>(u.data[0][0], u.data[1][1], u.data[2][2]);
}

}// namespace wp