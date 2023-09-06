//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "math/math_utils.h"
#include "math/matrix4x4.h"
#include "math/quaternion.h"

namespace vox {
//! Makes scale matrix.
template <typename T>
inline Matrix<T, 4, 4> makeScaleMatrix(T sx, T sy, T sz) {
    return Matrix<T, 4, 4>(sx, 0, 0, 0, 0, sy, 0, 0, 0, 0, sz, 0, 0, 0, 0, 1);
}

//! Makes scale matrix.
template <typename T>
inline Matrix<T, 4, 4> makeScaleMatrix(const Vector3<T> &s) {
    return ::vox::makeScaleMatrix(s.x, s.y, s.z);
}

//! Makes rotation matrix.
//! \warning Input angle should be radian.
template <typename T>
inline Matrix<T, 4, 4> makeRotationMatrix(const Vector3<T> &axis, T rad) {
    return Matrix<T, 4, 4>(Matrix<T, 3, 3>::makeRotationMatrix(axis, rad));
}

//! Makes translation matrix.
template <typename T>
inline Matrix<T, 4, 4> makeTranslationMatrix(const Point3<T> &t) {
    return Matrix<T, 4, 4>(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, t.x, t.y, t.z, 1);
}

//! Makes rotation && translation matrix.
template <typename T>
inline Matrix<T, 4, 4> makeRotationTranslationMatrix(const Quaternion<T> &q, const Point3<T> &t) {
    auto mat = q.matrix4();
    mat[12] = t.x;
    mat[13] = t.y;
    mat[14] = t.z;
    return mat;
}

//! Makes affine matrix.
template <typename T>
inline Matrix<T, 4, 4> makeAffineMatrix(const Vector3<T> &s, const Quaternion<T> &q, const Point3<T> &t) {
    T x = q.x;
    T y = q.y;
    T z = q.z;
    T w = q.w;

    T x_2 = x + x;
    T y_2 = y + y;
    T z_2 = z + z;

    T xx = x * x_2;
    T xy = x * y_2;
    T xz = x * z_2;
    T yy = y * y_2;
    T yz = y * z_2;
    T zz = z * z_2;
    T wx = w * x_2;
    T wy = w * y_2;
    T wz = w * z_2;
    T sx = s.x;
    T sy = s.y;
    T sz = s.z;

    return Matrix<T, 4, 4>((1 - (yy + zz)) * sx, (xy + wz) * sx, (xz - wy) * sx, 0,

                           (xy - wz) * sy, (1 - (xx + zz)) * sy, (yz + wx) * sy, 0,

                           (xz + wy) * sz, (yz - wx) * sz, (1 - (xx + yy)) * sz, 0,

                           t.x, t.y, t.z, 1);
}

/**
 * Calculate a right-handed look-at matrix.
 * @param eye - The position of the viewer's eye
 * @param target - The camera look-at target
 * @param up - The camera's up vector
 */
template <typename T>
inline Matrix<T, 4, 4> makeLookAtMatrix(const Point3<T> &eye, const Point3<T> &target, const Vector3<T> &up) {
    Vector3<T> z_axis = eye - target;
    z_axis.normalize();
    Vector3<T> x_axis = up.cross(z_axis);
    x_axis.normalize();
    Vector3<T> y_axis = z_axis.cross(x_axis);

    return Matrix<T, 4, 4>(x_axis.x, y_axis.x, z_axis.x, 0,

                           x_axis.y, y_axis.y, z_axis.y, 0,

                           x_axis.z, y_axis.z, z_axis.z, 0,

                           -eye.dot(x_axis), -eye.dot(y_axis), -eye.dot(z_axis), 1);
}

/**
 * Calculate an orthographic projection matrix.
 * @param left - The left edge of the viewing
 * @param right - The right edge of the viewing
 * @param bottom - The bottom edge of the viewing
 * @param top - The top edge of the viewing
 * @param near - The depth of the near plane
 * @param far - The depth of the far plane
 */
template <typename T>
inline Matrix<T, 4, 4> makeOrtho(T left, T right, T bottom, T top, T near, T far) {
    T lr = (T)1 / (left - right);
    T bt = (T)1 / (bottom - top);
    T nf = (T)1 / (near - far);

    return Matrix<T, 4, 4>(-2 * lr, 0, 0, 0,

                           0, 2 * bt, 0, 0,

                           0, 0, nf, 0,

                           (left + right) * lr, (top + bottom) * bt, near * nf, 1);
}

/**
 * Calculate a perspective projection matrix.
 * @param fovy - Field of view in the y direction, in radians
 * @param aspect - Aspect ratio, defined as view space width divided by height
 * @param near - The depth of the near plane
 * @param far - The depth of the far plane
 */
template <typename T>
inline Matrix<T, 4, 4> makePerspective(T fovy, T aspect, T near, T far) {
    T f = (T)1 / std::tan(fovy / 2);
    T nf = (T)1 / (near - far);

    return Matrix<T, 4, 4>(f / aspect, 0, 0, 0,

                           0, -f, 0, 0,

                           0, 0, far * nf, -1,

                           0, 0, far * near * nf, 0);
}

// MARK: - Decompose
/**
 * Decompose this matrix to translation, rotation and scale elements.
 * @param translation - Translation vector as an output parameter
 * @param rotation - Rotation quaternion as an output parameter
 * @param scale - Scale vector as an output parameter
 * @returns True if this matrix can be decomposed, false otherwise
 */
template <typename T>
bool decompose(const Matrix<T, 4, 4> &matrix, Point3<T> &translation, Quaternion<T> &rotation, Vector3<T> &scale) {
    Matrix<T, 3, 3> rm;
    const auto &m_11 = matrix[0];
    const auto &m_12 = matrix[1];
    const auto &m_13 = matrix[2];
    const auto &m_14 = matrix[3];
    const auto &m_21 = matrix[4];
    const auto &m_22 = matrix[5];
    const auto &m_23 = matrix[6];
    const auto &m_24 = matrix[7];
    const auto &m_31 = matrix[8];
    const auto &m_32 = matrix[9];
    const auto &m_33 = matrix[10];
    const auto &m_34 = matrix[11];

    translation.x = matrix[12];
    translation.y = matrix[13];
    translation.z = matrix[14];

    const auto kXs = sign(m_11 * m_12 * m_13 * m_14);
    const auto kYs = sign(m_21 * m_22 * m_23 * m_24);
    const auto kZs = sign(m_31 * m_32 * m_33 * m_34);

    const auto kSx = kXs * std::sqrt(m_11 * m_11 + m_12 * m_12 + m_13 * m_13);
    const auto kSy = kYs * std::sqrt(m_21 * m_21 + m_22 * m_22 + m_23 * m_23);
    const auto kSz = kZs * std::sqrt(m_31 * m_31 + m_32 * m_32 + m_33 * m_33);

    scale.x = kSx;
    scale.y = kSy;
    scale.z = kSz;

    if (std::abs(kSx) < std::numeric_limits<T>::epsilon() || std::abs(kSy) < std::numeric_limits<T>::epsilon() ||
        std::abs(kSz) < std::numeric_limits<T>::epsilon()) {
        rotation = Quaternion<T>::makeIdentity();
        return false;
    } else {
        const auto kInvSx = 1 / kSx;
        const auto kInvSy = 1 / kSy;
        const auto kInvSz = 1 / kSz;

        rm[0] = m_11 * kInvSx;
        rm[1] = m_12 * kInvSx;
        rm[2] = m_13 * kInvSx;
        rm[3] = m_21 * kInvSy;
        rm[4] = m_22 * kInvSy;
        rm[5] = m_23 * kInvSy;
        rm[6] = m_31 * kInvSz;
        rm[7] = m_32 * kInvSz;
        rm[8] = m_33 * kInvSz;
        rotation = Quaternion<T>(rm);
        return true;
    }
}

/**
 * Get rotation from this matrix.
 * @returns Rotation quaternion as an output parameter
 */
template <typename T>
Quaternion<T> getRotation(const Matrix<T, 4, 4> &matrix) {
    T trace = matrix[0] + matrix[5] + matrix[10];
    Quaternion<T> quat;

    if (trace > std::numeric_limits<T>::epsilon()) {
        T s = std::sqrt(trace + 1.0) * 2;
        quat.w = 0.25 * s;
        quat.x = (matrix[6] - matrix[9]) / s;
        quat.y = (matrix[8] - matrix[2]) / s;
        quat.z = (matrix[1] - matrix[4]) / s;
    } else if (matrix[0] > matrix[5] && matrix[0] > matrix[10]) {
        T s = std::sqrt(1.0 + matrix[0] - matrix[5] - matrix[10]) * 2;
        quat.w = (matrix[6] - matrix[9]) / s;
        quat.x = 0.25 * s;
        quat.y = (matrix[1] + matrix[4]) / s;
        quat.z = (matrix[8] + matrix[2]) / s;
    } else if (matrix[5] > matrix[10]) {
        T s = std::sqrt(1.0 + matrix[5] - matrix[0] - matrix[10]) * 2;
        quat.w = (matrix[8] - matrix[2]) / s;
        quat.x = (matrix[1] + matrix[4]) / s;
        quat.y = 0.25 * s;
        quat.z = (matrix[6] + matrix[9]) / s;
    } else {
        T s = std::sqrt(1.0 + matrix[10] - matrix[0] - matrix[5]) * 2;
        quat.w = (matrix[1] - matrix[4]) / s;
        quat.x = (matrix[8] + matrix[2]) / s;
        quat.y = (matrix[6] + matrix[9]) / s;
        quat.z = 0.25 * s;
    }

    return quat;
}

/**
 * Get scale from this matrix.
 * @returns Scale vector as an output parameter
 */
template <typename T>
Vector3<T> getScaling(const Matrix<T, 4, 4> &matrix) {
    T m_11 = matrix[0], m_12 = matrix[1], m_13 = matrix[2];
    T m_21 = matrix[4], m_22 = matrix[5], m_23 = matrix[6];
    T m_31 = matrix[8], m_32 = matrix[9], m_33 = matrix[10];

    Vector3<T> scaling;
    scaling.x = std::sqrt(m_11 * m_11 + m_12 * m_12 + m_13 * m_13);
    scaling.y = std::sqrt(m_21 * m_21 + m_22 * m_22 + m_23 * m_23);
    scaling.z = std::sqrt(m_31 * m_31 + m_32 * m_32 + m_33 * m_33);

    return scaling;
}

/**
 * Get translation from this matrix.
 * @returns Translation vector as an output parameter
 */
template <typename T>
Point3<T> getTranslation(const Matrix<T, 4, 4> &matrix) {
    return Point3<T>(matrix[12], matrix[13], matrix[14]);
}

}  // namespace vox
