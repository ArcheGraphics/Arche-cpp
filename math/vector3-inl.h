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
Vector<T, 3>::Vector(const std::initializer_list<U> &lst) {
    set(lst);
}

// Basic setters
template <typename T>
void Vector<T, 3>::set(T s) {
    x = s;
    y = s;
    z = s;
}

template <typename T>
void Vector<T, 3>::set(T new_x, T new_y, T new_z) {
    x = new_x;
    y = new_y;
    z = new_z;
}

template <typename T>
void Vector<T, 3>::set(const Vector2<T> &pt, T new_z) {
    x = pt.x;
    y = pt.y;
    z = new_z;
}

template <typename T>
template <typename U>
void Vector<T, 3>::set(const std::initializer_list<U> &lst) {
    VOX_ASSERT(lst.size() >= 3);

    auto input_elem = lst.begin();
    x = static_cast<T>(*input_elem);
    y = static_cast<T>(*(++input_elem));
    z = static_cast<T>(*(++input_elem));
}

template <typename T>
void Vector<T, 3>::set(const Vector &v) {
    x = v.x;
    y = v.y;
    z = v.z;
}

template <typename T>
void Vector<T, 3>::setZero() {
    x = y = z = 0;
}

template <typename T>
void Vector<T, 3>::normalize() {
    T l = length();
    x /= l;
    y /= l;
    z /= l;
}

// Binary operators: new instance = this (+) v
template <typename T>
Vector<T, 3> Vector<T, 3>::add(T v) const {
    return Vector(x + v, y + v, z + v);
}

template <typename T>
Vector<T, 3> Vector<T, 3>::add(const Vector &v) const {
    return Vector(x + v.x, y + v.y, z + v.z);
}

template <typename T>
Vector<T, 3> Vector<T, 3>::sub(T v) const {
    return Vector(x - v, y - v, z - v);
}

template <typename T>
Vector<T, 3> Vector<T, 3>::sub(const Vector &v) const {
    return Vector(x - v.x, y - v.y, z - v.z);
}

template <typename T>
Vector<T, 3> Vector<T, 3>::mul(T v) const {
    return Vector(x * v, y * v, z * v);
}

template <typename T>
Vector<T, 3> Vector<T, 3>::mul(const Vector &v) const {
    return Vector(x * v.x, y * v.y, z * v.z);
}

template <typename T>
Vector<T, 3> Vector<T, 3>::div(T v) const {
    return Vector(x / v, y / v, z / v);
}

template <typename T>
Vector<T, 3> Vector<T, 3>::div(const Vector &v) const {
    return Vector(x / v.x, y / v.y, z / v.z);
}

template <typename T>
T Vector<T, 3>::dot(const Vector &v) const {
    return x * v.x + y * v.y + z * v.z;
}

template <typename T>
Vector<T, 3> Vector<T, 3>::cross(const Vector &v) const {
    return Vector(y * v.z - v.y * z, z * v.x - v.z * x, x * v.y - v.x * y);
}

// Binary operators: new instance = v (+) this
template <typename T>
Vector<T, 3> Vector<T, 3>::rsub(T v) const {
    return Vector(v - x, v - y, v - z);
}

template <typename T>
Vector<T, 3> Vector<T, 3>::rsub(const Vector &v) const {
    return Vector(v.x - x, v.y - y, v.z - z);
}

template <typename T>
Vector<T, 3> Vector<T, 3>::rdiv(T v) const {
    return Vector(v / x, v / y, v / z);
}

template <typename T>
Vector<T, 3> Vector<T, 3>::rdiv(const Vector &v) const {
    return Vector(v.x / x, v.y / y, v.z / z);
}

template <typename T>
Vector<T, 3> Vector<T, 3>::rcross(const Vector &v) const {
    return Vector(v.y * z - y * v.z, v.z * x - z * v.x, v.x * y - x * v.y);
}

// Augmented operators: this (+)= v
template <typename T>
void Vector<T, 3>::iadd(T v) {
    x += v;
    y += v;
    z += v;
}

template <typename T>
void Vector<T, 3>::iadd(const Vector &v) {
    x += v.x;
    y += v.y;
    z += v.z;
}

template <typename T>
void Vector<T, 3>::isub(T v) {
    x -= v;
    y -= v;
    z -= v;
}

template <typename T>
void Vector<T, 3>::isub(const Vector &v) {
    x -= v.x;
    y -= v.y;
    z -= v.z;
}

template <typename T>
void Vector<T, 3>::imul(T v) {
    x *= v;
    y *= v;
    z *= v;
}

template <typename T>
void Vector<T, 3>::imul(const Vector &v) {
    x *= v.x;
    y *= v.y;
    z *= v.z;
}

template <typename T>
void Vector<T, 3>::idiv(T v) {
    x /= v;
    y /= v;
    z /= v;
}

template <typename T>
void Vector<T, 3>::idiv(const Vector &v) {
    x /= v.x;
    y /= v.y;
    z /= v.z;
}

// Basic getters
template <typename T>
const T &Vector<T, 3>::at(size_t i) const {
    VOX_ASSERT(i < 3);
    return (&x)[i];
}

template <typename T>
T &Vector<T, 3>::at(size_t i) {
    VOX_ASSERT(i < 3);
    return (&x)[i];
}

template <typename T>
T Vector<T, 3>::sum() const {
    return x + y + z;
}

template <typename T>
T Vector<T, 3>::avg() const {
    return (x + y + z) / 3;
}

template <typename T>
T Vector<T, 3>::min() const {
    return std::min(std::min(x, y), z);
}

template <typename T>
T Vector<T, 3>::max() const {
    return std::max(std::max(x, y), z);
}

template <typename T>
T Vector<T, 3>::absmin() const {
    return vox::absmin(vox::absmin(x, y), z);
}

template <typename T>
T Vector<T, 3>::absmax() const {
    return vox::absmax(vox::absmax(x, y), z);
}

template <typename T>
size_t Vector<T, 3>::dominantAxis() const {
    return (std::fabs(x) > std::fabs(y)) ? ((std::fabs(x) > std::fabs(z)) ? 0 : 2)
                                         : ((std::fabs(y) > std::fabs(z)) ? 1 : 2);
}

template <typename T>
size_t Vector<T, 3>::subminantAxis() const {
    return (std::fabs(x) < std::fabs(y)) ? ((std::fabs(x) < std::fabs(z)) ? 0 : 2)
                                         : ((std::fabs(y) < std::fabs(z)) ? 1 : 2);
}

template <typename T>
Vector<T, 3> Vector<T, 3>::normalized() const {
    T l = length();
    return Vector(x / l, y / l, z / l);
}

template <typename T>
T Vector<T, 3>::length() const {
    return std::sqrt(x * x + y * y + z * z);
}

template <typename T>
T Vector<T, 3>::lengthSquared() const {
    return x * x + y * y + z * z;
}

template <typename T>
T Vector<T, 3>::distanceTo(const Vector<T, 3> &other) const {
    return sub(other).length();
}

template <typename T>
T Vector<T, 3>::distanceSquaredTo(const Vector<T, 3> &other) const {
    return sub(other).lengthSquared();
}

template <typename T>
std::tuple<Vector<T, 3>, Vector<T, 3>>

Vector<T, 3>::tangential() const {
    Vector<T, 3> a = ((std::fabs(y) > 0 || std::fabs(z) > 0) ? Vector<T, 3>(1, 0, 0) : Vector<T, 3>(0, 1, 0))
                             .cross(*this)
                             .normalized();
    Vector<T, 3> b = cross(a);
    return std::make_tuple(a, b);
}

template <typename T>
template <typename U>
Vector<U, 3> Vector<T, 3>::castTo() const {
    return Vector<U, 3>(static_cast<U>(x), static_cast<U>(y), static_cast<U>(z));
}

template <typename T>
bool Vector<T, 3>::isEqual(const Vector &other) const {
    return x == other.x && y == other.y && z == other.z;
}

template <typename T>
bool Vector<T, 3>::isSimilar(const Vector &other, T epsilon) const {
    return (std::fabs(x - other.x) < epsilon) && (std::fabs(y - other.y) < epsilon) &&
           (std::fabs(z - other.z) < epsilon);
}

// Operators
template <typename T>
T &Vector<T, 3>::operator[](size_t i) {
    VOX_ASSERT(i < 3);
    return (&x)[i];
}

template <typename T>
const T &Vector<T, 3>::operator[](size_t i) const {
    VOX_ASSERT(i < 3);
    return (&x)[i];
}

template <typename T>
template <typename U>
Vector<T, 3> &Vector<T, 3>::operator=(const std::initializer_list<U> &lst) {
    set(lst);
    return (*this);
}

template <typename T>
Vector<T, 3> &Vector<T, 3>::operator=(const Vector &v) {
    set(v);
    return (*this);
}

template <typename T>
Vector<T, 3> &Vector<T, 3>::operator+=(T v) {
    iadd(v);
    return (*this);
}

template <typename T>
Vector<T, 3> &Vector<T, 3>::operator+=(const Vector &v) {
    iadd(v);
    return (*this);
}

template <typename T>
Vector<T, 3> &Vector<T, 3>::operator-=(T v) {
    isub(v);
    return (*this);
}

template <typename T>
Vector<T, 3> &Vector<T, 3>::operator-=(const Vector &v) {
    isub(v);
    return (*this);
}

template <typename T>
Vector<T, 3> &Vector<T, 3>::operator*=(T v) {
    imul(v);
    return (*this);
}

template <typename T>
Vector<T, 3> &Vector<T, 3>::operator*=(const Vector &v) {
    imul(v);
    return (*this);
}

template <typename T>
Vector<T, 3> &Vector<T, 3>::operator/=(T v) {
    idiv(v);
    return (*this);
}

template <typename T>
Vector<T, 3> &Vector<T, 3>::operator/=(const Vector &v) {
    idiv(v);
    return (*this);
}

template <typename T>
bool Vector<T, 3>::operator==(const Vector &v) const {
    return isEqual(v);
}

template <typename T>
bool Vector<T, 3>::operator!=(const Vector &v) const {
    return !isEqual(v);
}

template <typename T>
Vector<T, 3> operator+(const Vector<T, 3> &a) {
    return a;
}

template <typename T>
Vector<T, 3> operator-(const Vector<T, 3> &a) {
    return Vector<T, 3>(-a.x, -a.y, -a.z);
}

template <typename T>
Vector<T, 3> operator+(const Vector<T, 3> &a, T b) {
    return a.add(b);
}

template <typename T>
Vector<T, 3> operator+(T a, const Vector<T, 3> &b) {
    return b.add(a);
}

template <typename T>
Vector<T, 3> operator+(const Vector<T, 3> &a, const Vector<T, 3> &b) {
    return a.add(b);
}

template <typename T>
Vector<T, 3> operator-(const Vector<T, 3> &a, T b) {
    return a.sub(b);
}

template <typename T>
Vector<T, 3> operator-(T a, const Vector<T, 3> &b) {
    return b.rsub(a);
}

template <typename T>
Vector<T, 3> operator-(const Vector<T, 3> &a, const Vector<T, 3> &b) {
    return a.sub(b);
}

template <typename T>
Vector<T, 3> operator*(const Vector<T, 3> &a, T b) {
    return a.mul(b);
}

template <typename T>
Vector<T, 3> operator*(T a, const Vector<T, 3> &b) {
    return b.mul(a);
}

template <typename T>
Vector<T, 3> operator*(const Vector<T, 3> &a, const Vector<T, 3> &b) {
    return a.mul(b);
}

template <typename T>
Vector<T, 3> operator/(const Vector<T, 3> &a, T b) {
    return a.div(b);
}

template <typename T>
Vector<T, 3> operator/(T a, const Vector<T, 3> &b) {
    return b.rdiv(a);
}

template <typename T>
Vector<T, 3> operator/(const Vector<T, 3> &a, const Vector<T, 3> &b) {
    return a.div(b);
}

template <typename T>
Vector<T, 3> min(const Vector<T, 3> &a, const Vector<T, 3> &b) {
    return Vector<T, 3>(std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z));
}

template <typename T>
Vector<T, 3> max(const Vector<T, 3> &a, const Vector<T, 3> &b) {
    return Vector<T, 3>(std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z));
}

template <typename T>
Vector<T, 3> clamp(const Vector<T, 3> &v, const Vector<T, 3> &low, const Vector<T, 3> &high) {
    return Vector<T, 3>(clamp(v.x, low.x, high.x), clamp(v.y, low.y, high.y), clamp(v.z, low.z, high.z));
}

template <typename T>
Vector<T, 3> ceil(const Vector<T, 3> &a) {
    return Vector<T, 3>(std::ceil(a.x), std::ceil(a.y), std::ceil(a.z));
}

template <typename T>
Vector<T, 3> floor(const Vector<T, 3> &a) {
    return Vector<T, 3>(std::floor(a.x), std::floor(a.y), std::floor(a.z));
}

// Extensions
template <typename T>
Vector<T, 3> monotonicCatmullRom(
        const Vector<T, 3> &v_0, const Vector<T, 3> &v_1, const Vector<T, 3> &v_2, const Vector<T, 3> &v_3, T f) {
    static const T kTwo = static_cast<T>(2);
    static const T kThree = static_cast<T>(3);

    Vector<T, 3> d_1 = (v_2 - v_0) / kTwo;
    Vector<T, 3> d_2 = (v_3 - v_1) / kTwo;
    Vector<T, 3> dv_1 = v_2 - v_1;

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

    Vector<T, 3> a_3 = d_1 + d_2 - kTwo * dv_1;
    Vector<T, 3> a_2 = kThree * dv_1 - kTwo * d_1 - d_2;
    Vector<T, 3> a_1 = d_1;
    Vector<T, 3> a_0 = v_1;

    return a_3 * cubic(f) + a_2 * square(f) + a_1 * f + a_0;
}

}  // namespace vox
