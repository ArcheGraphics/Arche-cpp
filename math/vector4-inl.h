//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <algorithm>
#include <limits>
#include <tuple>

#include "math/macros.h"
#include "math/math_utils.h"

namespace vox {

// Constructors
template <typename T>
template <typename U>
Vector<T, 4>::Vector(const std::initializer_list<U> &lst) {
    set(lst);
}

// Basic setters
template <typename T>
void Vector<T, 4>::set(T s) {
    x = s;
    y = s;
    z = s;
    w = s;
}

template <typename T>
void Vector<T, 4>::set(T new_x, T new_y, T new_z, T new_w) {
    x = new_x;
    y = new_y;
    z = new_z;
    w = new_w;
}

template <typename T>
void Vector<T, 4>::set(const Vector<T, 3> &pt, T new_w) {
    x = pt.x;
    y = pt.y;
    z = pt.z;
    w = new_w;
}

template <typename T>
template <typename U>
void Vector<T, 4>::set(const std::initializer_list<U> &lst) {
    VOX_ASSERT(lst.size() >= 4);

    auto input_elem = lst.begin();
    x = static_cast<T>(*input_elem);
    y = static_cast<T>(*(++input_elem));
    z = static_cast<T>(*(++input_elem));
    w = static_cast<T>(*(++input_elem));
}

template <typename T>
void Vector<T, 4>::set(const Vector &v) {
    x = v.x;
    y = v.y;
    z = v.z;
    w = v.w;
}

template <typename T>
void Vector<T, 4>::setZero() {
    x = y = z = w = 0;
}

template <typename T>
void Vector<T, 4>::normalize() {
    T l = length();
    x /= l;
    y /= l;
    z /= l;
    w /= l;
}

// Binary operators: new instance = this (+) v
template <typename T>
Vector<T, 4> Vector<T, 4>::add(T v) const {
    return Vector(x + v, y + v, z + v, w + v);
}

template <typename T>
Vector<T, 4> Vector<T, 4>::add(const Vector &v) const {
    return Vector(x + v.x, y + v.y, z + v.z, w + v.w);
}

template <typename T>
Vector<T, 4> Vector<T, 4>::sub(T v) const {
    return Vector(x - v, y - v, z - v, w - v);
}

template <typename T>
Vector<T, 4> Vector<T, 4>::sub(const Vector &v) const {
    return Vector(x - v.x, y - v.y, z - v.z, w - v.w);
}

template <typename T>
Vector<T, 4> Vector<T, 4>::mul(T v) const {
    return Vector(x * v, y * v, z * v, w * v);
}

template <typename T>
Vector<T, 4> Vector<T, 4>::mul(const Vector &v) const {
    return Vector(x * v.x, y * v.y, z * v.z, w * v.w);
}

template <typename T>
Vector<T, 4> Vector<T, 4>::div(T v) const {
    return Vector(x / v, y / v, z / v, w / v);
}

template <typename T>
Vector<T, 4> Vector<T, 4>::div(const Vector &v) const {
    return Vector(x / v.x, y / v.y, z / v.z, w / v.w);
}

template <typename T>
T Vector<T, 4>::dot(const Vector &v) const {
    return x * v.x + y * v.y + z * v.z + w * v.w;
}

// Binary operators: new instance = v (+) this
template <typename T>
Vector<T, 4> Vector<T, 4>::rsub(T v) const {
    return Vector(v - x, v - y, v - z, v - w);
}

template <typename T>
Vector<T, 4> Vector<T, 4>::rsub(const Vector &v) const {
    return Vector(v.x - x, v.y - y, v.z - z, v.w - w);
}

template <typename T>
Vector<T, 4> Vector<T, 4>::rdiv(T v) const {
    return Vector(v / x, v / y, v / z, v / w);
}

template <typename T>
Vector<T, 4> Vector<T, 4>::rdiv(const Vector &v) const {
    return Vector(v.x / x, v.y / y, v.z / z, v.w / w);
}

// Augmented operators: this (+)= v
template <typename T>
void Vector<T, 4>::iadd(T v) {
    x += v;
    y += v;
    z += v;
    w += v;
}

template <typename T>
void Vector<T, 4>::iadd(const Vector &v) {
    x += v.x;
    y += v.y;
    z += v.z;
    w += v.w;
}

template <typename T>
void Vector<T, 4>::isub(T v) {
    x -= v;
    y -= v;
    z -= v;
    w -= v;
}

template <typename T>
void Vector<T, 4>::isub(const Vector &v) {
    x -= v.x;
    y -= v.y;
    z -= v.z;
    w -= v.w;
}

template <typename T>
void Vector<T, 4>::imul(T v) {
    x *= v;
    y *= v;
    z *= v;
    w *= v;
}

template <typename T>
void Vector<T, 4>::imul(const Vector &v) {
    x *= v.x;
    y *= v.y;
    z *= v.z;
    w *= v.w;
}

template <typename T>
void Vector<T, 4>::idiv(T v) {
    x /= v;
    y /= v;
    z /= v;
    w /= v;
}

template <typename T>
void Vector<T, 4>::idiv(const Vector &v) {
    x /= v.x;
    y /= v.y;
    z /= v.z;
    w /= v.w;
}

// Basic getters
template <typename T>
const T &Vector<T, 4>::at(size_t i) const {
    VOX_ASSERT(i < 4);
    return (&x)[i];
}

template <typename T>
T &Vector<T, 4>::at(size_t i) {
    VOX_ASSERT(i < 4);
    return (&x)[i];
}

template <typename T>
T Vector<T, 4>::sum() const {
    return x + y + z + w;
}

template <typename T>
T Vector<T, 4>::avg() const {
    return (x + y + z + w) / 4;
}

template <typename T>
T Vector<T, 4>::min() const {
    return std::min(std::min(x, y), std::min(z, w));
}

template <typename T>
T Vector<T, 4>::max() const {
    return std::max(std::max(x, y), std::max(z, w));
}

template <typename T>
T Vector<T, 4>::absmin() const {
    return vox::absmin(vox::absmin(x, y), vox::absmin(z, w));
}

template <typename T>
T Vector<T, 4>::absmax() const {
    return vox::absmax(vox::absmax(x, y), vox::absmax(z, w));
}

template <typename T>
size_t Vector<T, 4>::dominantAxis() const {
    return (std::fabs(x) > std::fabs(y)) ? ((std::fabs(x) > std::fabs(z)) ? ((std::fabs(x) > std::fabs(w)) ? 0 : 3)
                                                                          : ((std::fabs(z) > std::fabs(w)) ? 2 : 3))
                                         : ((std::fabs(y) > std::fabs(z)) ? ((std::fabs(y) > std::fabs(w)) ? 1 : 3)
                                                                          : ((std::fabs(z) > std::fabs(w)) ? 2 : 3));
}

template <typename T>
size_t Vector<T, 4>::subminantAxis() const {
    return (std::fabs(x) < std::fabs(y)) ? ((std::fabs(x) < std::fabs(z)) ? ((std::fabs(x) < std::fabs(w)) ? 0 : 3)
                                                                          : ((std::fabs(z) < std::fabs(w)) ? 2 : 3))
                                         : ((std::fabs(y) < std::fabs(z)) ? ((std::fabs(y) < std::fabs(w)) ? 1 : 3)
                                                                          : ((std::fabs(z) < std::fabs(w)) ? 2 : 3));
}

template <typename T>
Vector<T, 4> Vector<T, 4>::normalized() const {
    T l = length();
    return Vector(x / l, y / l, z / l, w / l);
}

template <typename T>
T Vector<T, 4>::length() const {
    return std::sqrt(x * x + y * y + z * z + w * w);
}

template <typename T>
T Vector<T, 4>::lengthSquared() const {
    return x * x + y * y + z * z + w * w;
}

template <typename T>
T Vector<T, 4>::distanceTo(const Vector<T, 4> &other) const {
    return sub(other).length();
}

template <typename T>
T Vector<T, 4>::distanceSquaredTo(const Vector<T, 4> &other) const {
    return sub(other).lengthSquared();
}

template <typename T>
template <typename U>
Vector<U, 4> Vector<T, 4>::castTo() const {
    return Vector<U, 4>(static_cast<U>(x), static_cast<U>(y), static_cast<U>(z), static_cast<U>(w));
}

template <typename T>
bool Vector<T, 4>::isEqual(const Vector &other) const {
    return x == other.x && y == other.y && z == other.z && w == other.w;
}

template <typename T>
bool Vector<T, 4>::isSimilar(const Vector &other, T epsilon) const {
    return (std::fabs(x - other.x) < epsilon) && (std::fabs(y - other.y) < epsilon) &&
           (std::fabs(z - other.z) < epsilon) && (std::fabs(w - other.w) < epsilon);
}

// Operators
template <typename T>
T &Vector<T, 4>::operator[](size_t i) {
    VOX_ASSERT(i < 4);
    return (&x)[i];
}

template <typename T>
const T &Vector<T, 4>::operator[](size_t i) const {
    VOX_ASSERT(i < 4);
    return (&x)[i];
}

template <typename T>
template <typename U>
Vector<T, 4> &Vector<T, 4>::operator=(const std::initializer_list<U> &lst) {
    set(lst);
    return (*this);
}

template <typename T>
Vector<T, 4> &Vector<T, 4>::operator=(const Vector &v) {
    set(v);
    return (*this);
}

template <typename T>
Vector<T, 4> &Vector<T, 4>::operator+=(T v) {
    iadd(v);
    return (*this);
}

template <typename T>
Vector<T, 4> &Vector<T, 4>::operator+=(const Vector &v) {
    iadd(v);
    return (*this);
}

template <typename T>
Vector<T, 4> &Vector<T, 4>::operator-=(T v) {
    isub(v);
    return (*this);
}

template <typename T>
Vector<T, 4> &Vector<T, 4>::operator-=(const Vector &v) {
    isub(v);
    return (*this);
}

template <typename T>
Vector<T, 4> &Vector<T, 4>::operator*=(T v) {
    imul(v);
    return (*this);
}

template <typename T>
Vector<T, 4> &Vector<T, 4>::operator*=(const Vector &v) {
    imul(v);
    return (*this);
}

template <typename T>
Vector<T, 4> &Vector<T, 4>::operator/=(T v) {
    idiv(v);
    return (*this);
}

template <typename T>
Vector<T, 4> &Vector<T, 4>::operator/=(const Vector &v) {
    idiv(v);
    return (*this);
}

template <typename T>
bool Vector<T, 4>::operator==(const Vector &v) const {
    return x == v.x && y == v.y && z == v.z && w == v.w;
}

template <typename T>
bool Vector<T, 4>::operator!=(const Vector &v) const {
    return x != v.x || y != v.y || z != v.z || w != v.w;
}

template <typename T>
Vector<T, 4> operator+(const Vector<T, 4> &a) {
    return a;
}

template <typename T>
Vector<T, 4> operator-(const Vector<T, 4> &a) {
    return Vector<T, 4>(-a.x, -a.y, -a.z, -a.w);
}

template <typename T>
Vector<T, 4> operator+(const Vector<T, 4> &a, T b) {
    return a.add(b);
}

template <typename T>
Vector<T, 4> operator+(T a, const Vector<T, 4> &b) {
    return b.add(a);
}

template <typename T>
Vector<T, 4> operator+(const Vector<T, 4> &a, const Vector<T, 4> &b) {
    return a.add(b);
}

template <typename T>
Vector<T, 4> operator-(const Vector<T, 4> &a, T b) {
    return a.sub(b);
}

template <typename T>
Vector<T, 4> operator-(T a, const Vector<T, 4> &b) {
    return b.rsub(a);
}

template <typename T>
Vector<T, 4> operator-(const Vector<T, 4> &a, const Vector<T, 4> &b) {
    return a.sub(b);
}

template <typename T>
Vector<T, 4> operator*(const Vector<T, 4> &a, T b) {
    return a.mul(b);
}

template <typename T>
Vector<T, 4> operator*(T a, const Vector<T, 4> &b) {
    return b.mul(a);
}

template <typename T>
Vector<T, 4> operator*(const Vector<T, 4> &a, const Vector<T, 4> &b) {
    return a.mul(b);
}

template <typename T>
Vector<T, 4> operator/(const Vector<T, 4> &a, T b) {
    return a.div(b);
}

template <typename T>
Vector<T, 4> operator/(T a, const Vector<T, 4> &b) {
    return b.rdiv(a);
}

template <typename T>
Vector<T, 4> operator/(const Vector<T, 4> &a, const Vector<T, 4> &b) {
    return a.div(b);
}

template <typename T>
Vector<T, 4> min(const Vector<T, 4> &a, const Vector<T, 4> &b) {
    return Vector<T, 4>(std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z), std::min(a.w, b.w));
}

template <typename T>
Vector<T, 4> max(const Vector<T, 4> &a, const Vector<T, 4> &b) {
    return Vector<T, 4>(std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z), std::max(a.w, b.w));
}

template <typename T>
Vector<T, 4> clamp(const Vector<T, 4> &v, const Vector<T, 4> &low, const Vector<T, 4> &high) {
    return Vector<T, 4>(clamp(v.x, low.x, high.x), clamp(v.y, low.y, high.y), clamp(v.z, low.z, high.z),
                        clamp(v.w, low.w, high.w));
}

template <typename T>
Vector<T, 4> ceil(const Vector<T, 4> &a) {
    return Vector<T, 4>(std::ceil(a.x), std::ceil(a.y), std::ceil(a.z), std::ceil(a.w));
}

template <typename T>
Vector<T, 4> floor(const Vector<T, 4> &a) {
    return Vector<T, 4>(std::floor(a.x), std::floor(a.y), std::floor(a.z), std::floor(a.w));
}

// Extensions
template <typename T>
Vector<T, 4> monotonicCatmullRom(
        const Vector<T, 4> &v_0, const Vector<T, 4> &v_1, const Vector<T, 4> &v_2, const Vector<T, 4> &v_3, T f) {
    static const T kTwo = static_cast<T>(2);
    static const T kThree = static_cast<T>(3);

    Vector<T, 4> d_1 = (v_2 - v_0) / kTwo;
    Vector<T, 4> d_2 = (v_3 - v_1) / kTwo;
    Vector<T, 4> dv_1 = v_2 - v_1;

    if (std::fabs(dv_1.x) < std::numeric_limits<float>::epsilon() || sign(dv_1.x) != sign(d_1.x) ||
        sign(dv_1.x) != sign(d_2.x)) {
        d_1.x = d_2.x = 0;
    }

    if (std::fabs(dv_1.y) < std::numeric_limits<float>::epsilon() || sign(dv_1.y) != sign(d_1.y) ||
        sign(dv_1.y) != sign(d_2.y)) {
        d_1.y = d_2.y = 0;
    }

    if (std::fabs(dv_1.z) < std::numeric_limits<float>::epsilon() || sign(dv_1.z) != sign(d_1.z) ||
        sign(dv_1.z) != sign(d_2.z)) {
        d_1.z = d_2.z = 0;
    }

    if (std::fabs(dv_1.w) < std::numeric_limits<float>::epsilon() || sign(dv_1.w) != sign(d_1.w) ||
        sign(dv_1.w) != sign(d_2.w)) {
        d_1.w = d_2.w = 0;
    }

    Vector<T, 4> a_3 = d_1 + d_2 - kTwo * dv_1;
    Vector<T, 4> a_2 = kThree * dv_1 - kTwo * d_1 - d_2;
    Vector<T, 4> a_1 = d_1;
    Vector<T, 4> a_0 = v_1;

    return a_3 * cubic(f) + a_2 * square(f) + a_1 * f + a_0;
}

}  // namespace vox
