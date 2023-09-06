//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <limits>

namespace vox {

// Constructors
template <typename T>
inline Quaternion<T>::Quaternion() {
    setIdentity();
}

template <typename T>
inline Quaternion<T>::Quaternion(T new_x, T new_y, T new_z, T new_w) {
    set(new_x, new_y, new_z, new_w);
}

template <typename T>
inline Quaternion<T>::Quaternion(const std::initializer_list<T> &lst) {
    set(lst);
}

template <typename T>
inline Quaternion<T>::Quaternion(const Vector3<T> &axis, T angle) {
    set(axis, angle);
}

template <typename T>
inline Quaternion<T>::Quaternion(const Vector3<T> &from, const Vector3<T> &to) {
    set(from, to);
}

template <typename T>
inline Quaternion<T>::Quaternion(const Vector3<T> &rotation_basis_0,
                                 const Vector3<T> &rotation_basis_1,
                                 const Vector3<T> &rotation_basis_2) {
    set(rotation_basis_0, rotation_basis_1, rotation_basis_2);
}

template <typename T>
inline Quaternion<T>::Quaternion(const Matrix3x3<T> &matrix) {
    set(matrix);
}

template <typename T>
inline Quaternion<T>::Quaternion(const Quaternion &other) {
    set(other);
}

// Basic setters
template <typename T>
inline void Quaternion<T>::set(const Quaternion &other) {
    set(other.x, other.y, other.z, other.w);
}

template <typename T>
inline void Quaternion<T>::set(T new_x, T new_y, T new_z, T new_w) {
    x = new_x;
    y = new_y;
    z = new_z;
    w = new_w;
}

template <typename T>
inline void Quaternion<T>::set(const std::initializer_list<T> &lst) {
    assert(lst.size() == 4);

    auto input_elem = lst.begin();
    x = *input_elem;
    y = *(++input_elem);
    z = *(++input_elem);
    w = *(++input_elem);
}

template <typename T>
inline void Quaternion<T>::set(const Vector3<T> &axis, T angle) {
    static const T kEps = std::numeric_limits<T>::epsilon();

    T axis_length_squared = axis.lengthSquared();

    if (axis_length_squared < kEps) {
        setIdentity();
    } else {
        Vector3<T> normalized_axis = axis.normalized();
        T s = std::sin(angle / 2);

        x = normalized_axis.x * s;
        y = normalized_axis.y * s;
        z = normalized_axis.z * s;
        w = std::cos(angle / 2);
    }
}

template <typename T>
inline void Quaternion<T>::set(const Vector3<T> &from, const Vector3<T> &to) {
    static const T kEps = std::numeric_limits<T>::epsilon();

    Vector3<T> axis = from.cross(to);

    T from_length_squared = from.lengthSquared();
    T to_length_squared = to.lengthSquared();

    if (from_length_squared < kEps || to_length_squared < kEps) {
        setIdentity();
    } else {
        T axis_length_squared = axis.lengthSquared();

        // In case two vectors are exactly the opposite, pick orthogonal vector
        // for axis.
        if (axis_length_squared < kEps) {
            axis = std::get<0>(from.tangential());
        }

        set(axis.x, axis.y, axis.z, from.dot(to));
        w += l2Norm();

        normalize();
    }
}

template <typename T>
inline void Quaternion<T>::set(const Vector3<T> &rotation_basis_0,
                               const Vector3<T> &rotation_basis_1,
                               const Vector3<T> &rotation_basis_2) {
    Matrix3x3<T> matrix_3;

    matrix_3.setColumn(0, rotation_basis_0.normalized());
    matrix_3.setColumn(1, rotation_basis_1.normalized());
    matrix_3.setColumn(2, rotation_basis_2.normalized());

    set(matrix_3);
}

template <typename T>
inline void Quaternion<T>::set(const Matrix3x3<T> &m) {
    static const T kEps = std::numeric_limits<T>::epsilon();
    static const T kQuater = static_cast<T>(0.25);

    T one_plus_trace = m.trace() + 1;

    if (one_plus_trace > kEps) {
        T s = std::sqrt(one_plus_trace) * 2;
        w = kQuater * s;
        x = (m(2, 1) - m(1, 2)) / s;
        y = (m(0, 2) - m(2, 0)) / s;
        z = (m(1, 0) - m(0, 1)) / s;
    } else if (m(0, 0) > m(1, 1) && m(0, 0) > m(2, 2)) {
        T s = std::sqrt(1 + m(0, 0) - m(1, 1) - m(2, 2)) * 2;
        w = (m(2, 1) - m(1, 2)) / s;
        x = kQuater * s;
        y = (m(0, 1) + m(1, 0)) / s;
        z = (m(0, 2) + m(2, 0)) / s;
    } else if (m(1, 1) > m(2, 2)) {
        T s = std::sqrt(1 + m(1, 1) - m(0, 0) - m(2, 2)) * 2;
        w = (m(0, 2) - m(2, 0)) / s;
        x = (m(0, 1) + m(1, 0)) / s;
        y = kQuater * s;
        z = (m(1, 2) + m(2, 1)) / s;
    } else {
        T s = std::sqrt(1 + m(2, 2) - m(0, 0) - m(1, 1)) * 2;
        w = (m(1, 0) - m(0, 1)) / s;
        x = (m(0, 2) + m(2, 0)) / s;
        y = (m(1, 2) + m(2, 1)) / s;
        z = kQuater * s;
    }
}

// Basic getters
template <typename T>
template <typename U>
Quaternion<U> Quaternion<T>::castTo() const {
    return Quaternion<U>(static_cast<U>(x), static_cast<U>(y), static_cast<U>(z), static_cast<U>(w));
}

//! Returns normalized quaternion.
template <typename T>
Quaternion<T> Quaternion<T>::normalized() const {
    Quaternion q(*this);
    q.normalize();
    return q;
}

template <typename T>
Quaternion<T> Quaternion<T>::add(const Quaternion &v) const {
    return Quaternion(x + v.x, y + v.y, z + v.z, w + v.w);
}

template <typename T>
Quaternion<T> Quaternion<T>::mul(T v) const {
    return Quaternion(x * v, y * v, z * v, w * v);
}

// Binary operator methods - new instance = this instance (+) input
template <typename T>
inline Point3<T> Quaternion<T>::mul(const Point3<T> &v) const {
    // calculate quat * vec
    T ix = w * v.x + y * v.z - z * v.y;
    T iy = w * v.y + z * v.x - x * v.z;
    T iz = w * v.z + x * v.y - y * v.x;
    T iw = -x * v.x - y * v.y - z * v.z;

    // calculate result * inverse quat
    return Point3<T>(ix * w - iw * x - iy * z + iz * y, iy * w - iw * y - iz * x + ix * z,
                     iz * w - iw * z - ix * y + iy * x);
}

// Binary operator methods - new instance = this instance (+) input
template <typename T>
inline Vector3<T> Quaternion<T>::mul(const Vector3<T> &v) const {
    // calculate quat * vec
    T ix = w * v.x + y * v.z - z * v.y;
    T iy = w * v.y + z * v.x - x * v.z;
    T iz = w * v.z + x * v.y - y * v.x;
    T iw = -x * v.x - y * v.y - z * v.z;

    // calculate result * inverse quat
    return Vector3<T>(ix * w - iw * x - iy * z + iz * y, iy * w - iw * y - iz * x + ix * z,
                      iz * w - iw * z - ix * y + iy * x);
}

// Binary operator methods - new instance = this instance (+) input
template <typename T>
inline Vector4<T> Quaternion<T>::mul(const Vector4<T> &v) const {
    // calculate quat * vec
    T ix = w * v.x + y * v.z - z * v.y;
    T iy = w * v.y + z * v.x - x * v.z;
    T iz = w * v.z + x * v.y - y * v.x;
    T iw = -x * v.x - y * v.y - z * v.z;

    // calculate result * inverse quat
    return Vector4<T>(ix * w - iw * x - iy * z + iz * y, iy * w - iw * y - iz * x + ix * z,
                      iz * w - iw * z - ix * y + iy * x, v.w);
}

template <typename T>
inline Quaternion<T> Quaternion<T>::mul(const Quaternion &other) const {
    return Quaternion(w * other.x + x * other.w + y * other.z - z * other.y,
                      w * other.y - x * other.z + y * other.w + z * other.x,
                      w * other.z + x * other.y - y * other.x + z * other.w,
                      w * other.w - x * other.x - y * other.y - z * other.z);
}

template <typename T>
inline T Quaternion<T>::dot(const Quaternion<T> &other) const {
    return w * other.w + x * other.x + y * other.y + z * other.z;
}

// Binary operator methods - new instance = input (+) this instance
template <typename T>
inline Quaternion<T> Quaternion<T>::rmul(const Quaternion &other) const {
    return Quaternion(other.w * x + other.x * w + other.y * z - other.z * y,
                      other.w * y - other.x * z + other.y * w + other.z * x,
                      other.w * z + other.x * y - other.y * x + other.z * w,
                      other.w * w - other.x * x - other.y * y - other.z * z);
}

template <typename T>
void Quaternion<T>::iadd(const Quaternion &v) {
    x += v.x;
    y += v.y;
    z += v.z;
    w += v.w;
}

template <typename T>
void Quaternion<T>::imul(T v) {
    x *= v;
    y *= v;
    z *= v;
    w *= v;
}

// Augmented operator methods - this instance (+)= input
template <typename T>
inline void Quaternion<T>::imul(const Quaternion &other) {
    *this = mul(other);
}

// Modifiers
template <typename T>
inline void Quaternion<T>::setIdentity() {
    set(0, 0, 0, 1);
}

template <typename T>
inline void Quaternion<T>::rotate(T angle_in_radians) {
    Vector3<T> axis;
    T currentAngle;

    getAxisAngle(&axis, &currentAngle);

    currentAngle += angle_in_radians;

    set(axis, currentAngle);
}

template <typename T>
inline void Quaternion<T>::normalize() {
    T norm = l2Norm();

    if (norm > 0) {
        w /= norm;
        x /= norm;
        y /= norm;
        z /= norm;
    }
}

template <typename T>
inline Quaternion<T> Quaternion<T>::rotateX(T rad) const {
    rad *= 0.5;
    T bx = std::sin(rad);
    T bw = std::cos(rad);

    return Quaternion<T>(x * bw + w * bx, y * bw + z * bx, z * bw - y * bx, w * bw - x * bx);
}

template <typename T>
inline void Quaternion<T>::rotateX(T rad) {
    const T kNewX = x;
    const T kNewY = y;
    const T kNewZ = z;
    const T kNewW = w;

    rad *= 0.5;
    T bx = std::sin(rad);
    T bw = std::cos(rad);

    x = kNewX * bw + kNewW * bx;
    y = kNewY * bw + kNewZ * bx;
    z = kNewZ * bw - kNewY * bx;
    w = kNewW * bw - kNewX * bx;
}

template <typename T>
inline Quaternion<T> Quaternion<T>::rotateY(T rad) const {
    rad *= 0.5;
    T by = std::sin(rad);
    T bw = std::cos(rad);

    return Quaternion<T>(x * bw - z * by, y * bw + w * by, z * bw + x * by, w * bw - y * by);
}

template <typename T>
inline void Quaternion<T>::rotateY(T rad) {
    const T kNewX = x;
    const T kNewY = y;
    const T kNewZ = z;
    const T kNewW = w;

    rad *= 0.5;
    T by = std::sin(rad);
    T bw = std::cos(rad);

    x = kNewX * bw - kNewZ * by;
    y = kNewY * bw + kNewW * by;
    z = kNewZ * bw + kNewX * by;
    w = kNewW * bw - kNewY * by;
}

template <typename T>
inline Quaternion<T> Quaternion<T>::rotateZ(T rad) const {
    rad *= 0.5;
    T bz = std::sin(rad);
    T bw = std::cos(rad);

    return Quaternion<T>(x * bw + y * bz, y * bw - x * bz, z * bw + w * bz, w * bw - z * bz);
}

template <typename T>
inline void Quaternion<T>::rotateZ(T rad) {
    const T kNewX = x;
    const T kNewY = y;
    const T kNewZ = z;
    const T kNewW = w;

    rad *= 0.5;
    T bz = std::sin(rad);
    T bw = std::cos(rad);

    x = kNewX * bw + kNewY * bz;
    y = kNewY * bw - kNewX * bz;
    z = kNewZ * bw + kNewW * bz;
    w = kNewW * bw - kNewZ * bz;
}

template <typename T>
void Quaternion<T>::rotateAxisAngle(const Vector3<T> &axis, T rad) {
    Quaternion temp_quat = Quaternion(axis, rad);
    *this *= temp_quat;
}

// Complex getters
template <typename T>
inline Vector3<T> Quaternion<T>::axis() const {
    Vector3<T> result(x, y, z);
    result.normalize();

    if (2 * std::acos(w) < pi<T>()) {
        return result;
    } else {
        return -result;
    }
}

template <typename T>
inline T Quaternion<T>::angle() const {
    T result = 2 * std::acos(w);

    if (result < pi<T>()) {
        return result;
    } else {
        // Wrap around
        return 2 * pi<T>() - result;
    }
}

template <typename T>
inline void Quaternion<T>::getAxisAngle(Vector3<T> *axis, T *angle) const {
    axis->set(x, y, z);
    axis->normalize();
    *angle = 2 * std::acos(w);

    if (*angle > pi<T>()) {
        // Wrap around
        (*axis) = -(*axis);
        *angle = 2 * pi<T>() - (*angle);
    }
}

template <typename T>
inline Vector3<T> Quaternion<T>::toEuler() const {
    Vector3<T> euler = toYawPitchRoll();
    T t = euler.x;
    euler.x = euler.y;
    euler.y = t;
    return euler;
}

template <typename T>
inline Vector3<T> Quaternion<T>::toYawPitchRoll() const {
    T xx = x * x;
    T yy = y * y;
    T zz = z * z;
    T xy = x * y;
    T zw = z * w;
    T zx = z * x;
    T yw = y * w;
    T yz = y * z;
    T xw = x * w;

    Vector3<T> euler;
    euler.y = std::asin(2.0 * (xw - yz));
    if (std::cos(euler.y) > std::numeric_limits<T>::epsilon()) {
        euler.z = std::atan2(2.0 * (xy + zw), 1.0 - 2.0 * (zz + xx));
        euler.x = std::atan2(2.0 * (zx + yw), 1.0 - 2.0 * (yy + xx));
    } else {
        euler.z = std::atan2(-2.0 * (xy - zw), 1.0 - 2.0 * (yy + zz));
        euler.x = 0.0;
    }

    return euler;
}

template <typename T>
inline Quaternion<T> Quaternion<T>::conjugate() const {
    return Quaternion<T>(-x, -y, -z, w);
}

template <typename T>
inline T Quaternion<T>::length() const {
    return std::sqrt(x * x + y * y + z * z + w * w);
}

template <typename T>
inline T Quaternion<T>::lengthSquared() const {
    return x * x + y * y + z * z + w * w;
}

template <typename T>
inline Quaternion<T> Quaternion<T>::inverse() const {
    T denom = w * w + x * x + y * y + z * z;
    return Quaternion(-x / denom, -y / denom, -z / denom, w / denom);
}

template <typename T>
inline Matrix3x3<T> Quaternion<T>::matrix3() const {
    T _2xx = 2 * x * x;
    T _2yy = 2 * y * y;
    T _2zz = 2 * z * z;
    T _2xy = 2 * x * y;
    T _2xz = 2 * x * z;
    T _2xw = 2 * x * w;
    T _2yz = 2 * y * z;
    T _2yw = 2 * y * w;
    T _2zw = 2 * z * w;

    Matrix3x3<T> m(1 - _2yy - _2zz, _2xy + _2zw, _2xz - _2yw, _2xy - _2zw, 1 - _2zz - _2xx, _2yz + _2xw, _2xz + _2yw,
                   _2yz - _2xw, 1 - _2yy - _2xx);

    return m;
}

template <typename T>
inline Matrix4x4<T> Quaternion<T>::matrix4() const {
    T _2xx = 2 * x * x;
    T _2yy = 2 * y * y;
    T _2zz = 2 * z * z;
    T _2xy = 2 * x * y;
    T _2xz = 2 * x * z;
    T _2xw = 2 * x * w;
    T _2yz = 2 * y * z;
    T _2yw = 2 * y * w;
    T _2zw = 2 * z * w;

    Matrix4x4<T> m(1 - _2yy - _2zz, _2xy + _2zw, _2xz - _2yw, 0, _2xy - _2zw, 1 - _2zz - _2xx, _2yz + _2xw, 0,
                   _2xz + _2yw, _2yz - _2xw, 1 - _2yy - _2xx, 0, 0, 0, 0, 1);

    return m;
}

template <typename T>
inline T Quaternion<T>::l2Norm() const {
    return std::sqrt(w * w + x * x + y * y + z * z);
}

// Setter operators
template <typename T>
inline Quaternion<T> &Quaternion<T>::operator=(const Quaternion &other) {
    set(other);
    return *this;
}

template <typename T>
Quaternion<T> &Quaternion<T>::operator+=(const Quaternion &v) {
    iadd(v);
    return (*this);
}

template <typename T>
Quaternion<T> &Quaternion<T>::operator*=(T v) {
    imul(v);
    return (*this);
}

template <typename T>
inline Quaternion<T> &Quaternion<T>::operator*=(const Quaternion &other) {
    imul(other);
    return *this;
}

// Getter operators
template <typename T>
inline T &Quaternion<T>::operator[](size_t i) {
    return (&w)[i];
}

template <typename T>
inline const T &Quaternion<T>::operator[](size_t i) const {
    return (&w)[i];
}

template <typename T>
bool Quaternion<T>::operator==(const Quaternion &other) const {
    return w == other.w && x == other.x && y == other.y && z == other.z;
}

template <typename T>
bool Quaternion<T>::operator!=(const Quaternion &other) const {
    return w != other.w || x != other.x || y != other.y || z != other.z;
}

template <typename T>
Quaternion<T> Quaternion<T>::makeIdentity() {
    return Quaternion();
}

template <typename T>
Quaternion<T> Quaternion<T>::makeRotationEuler(T pitch, T yaw, T roll) {
    return Quaternion<T>::makeRotationYawPitchRoll(yaw, pitch, roll);
}

template <typename T>
Quaternion<T> Quaternion<T>::makeRotationYawPitchRoll(T yaw, T pitch, T roll) {
    T half_roll = roll * 0.5;
    T half_pitch = pitch * 0.5;
    T half_yaw = yaw * 0.5;

    T sin_roll = std::sin(half_roll);
    T cos_roll = std::cos(half_roll);
    T sin_pitch = std::sin(half_pitch);
    T cos_pitch = std::cos(half_pitch);
    T sin_yaw = std::sin(half_yaw);
    T cos_yaw = std::cos(half_yaw);

    T cos_yaw_pitch = cos_yaw * cos_pitch;
    T sin_yaw_pitch = sin_yaw * sin_pitch;

    return Quaternion<T>(cos_yaw * sin_pitch * cos_roll + sin_yaw * cos_pitch * sin_roll,
                         sin_yaw * cos_pitch * cos_roll - cos_yaw * sin_pitch * sin_roll,
                         cos_yaw_pitch * sin_roll - sin_yaw_pitch * cos_roll,
                         cos_yaw_pitch * cos_roll + sin_yaw_pitch * sin_roll);
}

template <typename T>
Quaternion<T> Quaternion<T>::makeRotationX(T rad) {
    rad *= 0.5;
    T s = std::sin(rad);
    T c = std::cos(rad);
    return Quaternion<T>(s, 0, 0, c);
}

template <typename T>
Quaternion<T> Quaternion<T>::makeRotationY(T rad) {
    rad *= 0.5;
    T s = std::sin(rad);
    T c = std::cos(rad);
    return Quaternion<T>(0, s, 0, c);
}

template <typename T>
Quaternion<T> Quaternion<T>::makeRotationZ(T rad) {
    rad *= 0.5;
    T s = std::sin(rad);
    T c = std::cos(rad);
    return Quaternion<T>(0, 0, s, c);
}

template <typename T>
inline Quaternion<T> slerp(const Quaternion<T> &a, const Quaternion<T> &b, T t) {
    static const double kThreshold = 0.01;
    static const T kEps = std::numeric_limits<T>::epsilon();

    T cos_half_angle = a.dot(b);
    T weight_a, weight_b;

    // For better accuracy, return lerp result when a and b are close enough.
    if (1.0 - std::fabs(cos_half_angle) < kThreshold) {
        weight_a = 1.0 - t;
        weight_b = t;
    } else {
        T half_angle = std::acos(cos_half_angle);
        T sin_half_angle = std::sqrt(1 - cos_half_angle * cos_half_angle);

        // In case of angle ~ 180, pick middle value.
        // If not, perform slerp.
        if (std::fabs(sin_half_angle) < kEps) {
            weight_a = static_cast<T>(0.5);
            weight_b = static_cast<T>(0.5);
        } else {
            weight_a = std::sin((1 - t) * half_angle) / sin_half_angle;
            weight_b = std::sin(t * half_angle) / sin_half_angle;
        }
    }

    return Quaternion<T>(weight_a * a.x + weight_b * b.x, weight_a * a.y + weight_b * b.y,
                         weight_a * a.z + weight_b * b.z, weight_a * a.w + weight_b * b.w);
}

// Operator overloadings
template <typename T>
Quaternion<T> operator+(const Quaternion<T> &a, const Quaternion<T> &b) {
    return a.add(b);
}

template <typename T>
Quaternion<T> operator*(const Quaternion<T> &a, T b) {
    return a.mul(b);
}

template <typename T>
Quaternion<T> operator*(T a, const Quaternion<T> &b) {
    return b.mul(a);
}

template <typename T>
inline Point<T, 3> operator*(const Quaternion<T> &q, const Point<T, 3> &v) {
    return q.mul(v);
}

template <typename T>
inline Vector<T, 3> operator*(const Quaternion<T> &q, const Vector<T, 3> &v) {
    return q.mul(v);
}

template <typename T>
inline Vector<T, 4> operator*(const Quaternion<T> &q, const Vector<T, 4> &v) {
    return q.mul(v);
}

template <typename T>
inline Quaternion<T> operator*(const Quaternion<T> &a, const Quaternion<T> &b) {
    return a.mul(b);
}

}  // namespace vox
