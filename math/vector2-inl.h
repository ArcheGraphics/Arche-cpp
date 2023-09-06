//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <algorithm>
#include <limits>

#include "math/macros.h"
#include "math/math_utils.h"

namespace vox {

// Constructors
template <typename T>
template <typename U>
Vector<T, 2>::Vector(const std::initializer_list<U> &lst) {
    set(lst);
}

// Basic setters
template <typename T>
void Vector<T, 2>::set(T s) {
    x = s;
    y = s;
}

template <typename T>
void Vector<T, 2>::set(T new_x, T new_y) {
    x = new_x;
    y = new_y;
}

template <typename T>
template <typename U>
void Vector<T, 2>::set(const std::initializer_list<U> &lst) {
    VOX_ASSERT(lst.size() >= 2);

    auto input_elem = lst.begin();
    x = static_cast<T>(*input_elem);
    y = static_cast<T>(*(++input_elem));
}

template <typename T>
void Vector<T, 2>::set(const Vector &v) {
    x = v.x;
    y = v.y;
}

template <typename T>
void Vector<T, 2>::setZero() {
    x = y = 0;
}

template <typename T>
void Vector<T, 2>::normalize() {
    T l = length();
    x /= l;
    y /= l;
}

// Binary operators: new instance = this (+) v
template <typename T>
Vector<T, 2> Vector<T, 2>::add(T v) const {
    return Vector(x + v, y + v);
}

template <typename T>
Vector<T, 2> Vector<T, 2>::add(const Vector &v) const {
    return Vector(x + v.x, y + v.y);
}

template <typename T>
Vector<T, 2> Vector<T, 2>::sub(T v) const {
    return Vector(x - v, y - v);
}

template <typename T>
Vector<T, 2> Vector<T, 2>::sub(const Vector &v) const {
    return Vector(x - v.x, y - v.y);
}

template <typename T>
Vector<T, 2> Vector<T, 2>::mul(T v) const {
    return Vector(x * v, y * v);
}

template <typename T>
Vector<T, 2> Vector<T, 2>::mul(const Vector &v) const {
    return Vector(x * v.x, y * v.y);
}

template <typename T>
Vector<T, 2> Vector<T, 2>::div(T v) const {
    return Vector(x / v, y / v);
}

template <typename T>
Vector<T, 2> Vector<T, 2>::div(const Vector &v) const {
    return Vector(x / v.x, y / v.y);
}

template <typename T>
T Vector<T, 2>::dot(const Vector &v) const {
    return x * v.x + y * v.y;
}

template <typename T>
T Vector<T, 2>::cross(const Vector &v) const {
    return x * v.y - v.x * y;
}

// Binary operators: new instance = v (+) this
template <typename T>
Vector<T, 2> Vector<T, 2>::rsub(T v) const {
    return Vector(v - x, v - y);
}

template <typename T>
Vector<T, 2> Vector<T, 2>::rsub(const Vector &v) const {
    return Vector(v.x - x, v.y - y);
}

template <typename T>
Vector<T, 2> Vector<T, 2>::rdiv(T v) const {
    return Vector(v / x, v / y);
}

template <typename T>
Vector<T, 2> Vector<T, 2>::rdiv(const Vector &v) const {
    return Vector(v.x / x, v.y / y);
}

template <typename T>
T Vector<T, 2>::rcross(const Vector &v) const {
    return v.x * y - x * v.y;
}

// Augmented operators: this (+)= v
template <typename T>
void Vector<T, 2>::iadd(T v) {
    x += v;
    y += v;
}

template <typename T>
void Vector<T, 2>::iadd(const Vector &v) {
    x += v.x;
    y += v.y;
}

template <typename T>
void Vector<T, 2>::isub(T v) {
    x -= v;
    y -= v;
}

template <typename T>
void Vector<T, 2>::isub(const Vector &v) {
    x -= v.x;
    y -= v.y;
}

template <typename T>
void Vector<T, 2>::imul(T v) {
    x *= v;
    y *= v;
}

template <typename T>
void Vector<T, 2>::imul(const Vector &v) {
    x *= v.x;
    y *= v.y;
}

template <typename T>
void Vector<T, 2>::idiv(T v) {
    x /= v;
    y /= v;
}

template <typename T>
void Vector<T, 2>::idiv(const Vector &v) {
    x /= v.x;
    y /= v.y;
}

// Basic getters
template <typename T>
const T &Vector<T, 2>::at(size_t i) const {
    VOX_ASSERT(i < 2);
    return (&x)[i];
}

template <typename T>
T &Vector<T, 2>::at(size_t i) {
    VOX_ASSERT(i < 2);
    return (&x)[i];
}

template <typename T>
T Vector<T, 2>::sum() const {
    return x + y;
}

template <typename T>
T Vector<T, 2>::avg() const {
    return (x + y) / 2;
}

template <typename T>
T Vector<T, 2>::min() const {
    return std::min(x, y);
}

template <typename T>
T Vector<T, 2>::max() const {
    return std::max(x, y);
}

template <typename T>
T Vector<T, 2>::absmin() const {
    return vox::absmin(x, y);
}

template <typename T>
T Vector<T, 2>::absmax() const {
    return vox::absmax(x, y);
}

template <typename T>
size_t Vector<T, 2>::dominantAxis() const {
    return (std::fabs(x) > std::fabs(y)) ? 0 : 1;
}

template <typename T>
size_t Vector<T, 2>::subminantAxis() const {
    return (std::fabs(x) < std::fabs(y)) ? 0 : 1;
}

template <typename T>
Vector<T, 2> Vector<T, 2>::normalized() const {
    T l = length();
    return Vector(x / l, y / l);
}

template <typename T>
T Vector<T, 2>::length() const {
    return std::sqrt(x * x + y * y);
}

template <typename T>
T Vector<T, 2>::lengthSquared() const {
    return x * x + y * y;
}

template <typename T>
T Vector<T, 2>::distanceTo(const Vector<T, 2> &other) const {
    return sub(other).length();
}

template <typename T>
T Vector<T, 2>::distanceSquaredTo(const Vector<T, 2> &other) const {
    return sub(other).lengthSquared();
}

template <typename T>
Vector<T, 2> Vector<T, 2>::tangential() const {
    // Rotate 90 degrees
    return Vector<T, 2>(-y, x);
}

template <typename T>
template <typename U>
Vector<U, 2> Vector<T, 2>::castTo() const {
    return Vector<U, 2>(static_cast<U>(x), static_cast<U>(y));
}

template <typename T>
bool Vector<T, 2>::isEqual(const Vector &other) const {
    return x == other.x && y == other.y;
}

template <typename T>
bool Vector<T, 2>::isSimilar(const Vector &other, T epsilon) const {
    return (std::fabs(x - other.x) < epsilon) && (std::fabs(y - other.y) < epsilon);
}

// Operators
template <typename T>
T &Vector<T, 2>::operator[](size_t i) {
    VOX_ASSERT(i < 2);
    return (&x)[i];
}

template <typename T>
const T &Vector<T, 2>::operator[](size_t i) const {
    VOX_ASSERT(i < 2);
    return (&x)[i];
}

template <typename T>
template <typename U>
Vector<T, 2> &Vector<T, 2>::operator=(const std::initializer_list<U> &lst) {
    set(lst);
    return (*this);
}

template <typename T>
Vector<T, 2> &Vector<T, 2>::operator=(const Vector &v) {
    set(v);
    return (*this);
}

template <typename T>
Vector<T, 2> &Vector<T, 2>::operator+=(T v) {
    iadd(v);
    return (*this);
}

template <typename T>
Vector<T, 2> &Vector<T, 2>::operator+=(const Vector &v) {
    iadd(v);
    return (*this);
}

template <typename T>
Vector<T, 2> &Vector<T, 2>::operator-=(T v) {
    isub(v);
    return (*this);
}

template <typename T>
Vector<T, 2> &Vector<T, 2>::operator-=(const Vector &v) {
    isub(v);
    return (*this);
}

template <typename T>
Vector<T, 2> &Vector<T, 2>::operator*=(T v) {
    imul(v);
    return (*this);
}

template <typename T>
Vector<T, 2> &Vector<T, 2>::operator*=(const Vector &v) {
    imul(v);
    return (*this);
}

template <typename T>
Vector<T, 2> &Vector<T, 2>::operator/=(T v) {
    idiv(v);
    return (*this);
}

template <typename T>
Vector<T, 2> &Vector<T, 2>::operator/=(const Vector &v) {
    idiv(v);
    return (*this);
}

template <typename T>
bool Vector<T, 2>::operator==(const Vector &v) const {
    return isEqual(v);
}

template <typename T>
bool Vector<T, 2>::operator!=(const Vector &v) const {
    return !isEqual(v);
}

// Math functions
template <typename T>
Vector<T, 2> operator+(const Vector<T, 2> &a) {
    return a;
}

template <typename T>
Vector<T, 2> operator-(const Vector<T, 2> &a) {
    return Vector<T, 2>(-a.x, -a.y);
}

template <typename T>
Vector<T, 2> operator+(const Vector<T, 2> &a, T b) {
    return a.add(b);
}

template <typename T>
Vector<T, 2> operator+(T a, const Vector<T, 2> &b) {
    return b.add(a);
}

template <typename T>
Vector<T, 2> operator+(const Vector<T, 2> &a, const Vector<T, 2> &b) {
    return a.add(b);
}

template <typename T>
Vector<T, 2> operator-(const Vector<T, 2> &a, T b) {
    return a.sub(b);
}

template <typename T>
Vector<T, 2> operator-(T a, const Vector<T, 2> &b) {
    return b.rsub(a);
}

template <typename T>
Vector<T, 2> operator-(const Vector<T, 2> &a, const Vector<T, 2> &b) {
    return a.sub(b);
}

template <typename T>
Vector<T, 2> operator*(const Vector<T, 2> &a, T b) {
    return a.mul(b);
}

template <typename T>
Vector<T, 2> operator*(T a, const Vector<T, 2> &b) {
    return b.mul(a);
}

template <typename T>
Vector<T, 2> operator*(const Vector<T, 2> &a, const Vector<T, 2> &b) {
    return a.mul(b);
}

template <typename T>
Vector<T, 2> operator/(const Vector<T, 2> &a, T b) {
    return a.div(b);
}

template <typename T>
Vector<T, 2> operator/(T a, const Vector<T, 2> &b) {
    return b.rdiv(a);
}

template <typename T>
Vector<T, 2> operator/(const Vector<T, 2> &a, const Vector<T, 2> &b) {
    return a.div(b);
}

template <typename T>
Vector<T, 2> min(const Vector<T, 2> &a, const Vector<T, 2> &b) {
    return Vector<T, 2>(std::min(a.x, b.x), std::min(a.y, b.y));
}

template <typename T>
Vector<T, 2> max(const Vector<T, 2> &a, const Vector<T, 2> &b) {
    return Vector<T, 2>(std::max(a.x, b.x), std::max(a.y, b.y));
}

template <typename T>
Vector<T, 2> clamp(const Vector<T, 2> &v, const Vector<T, 2> &low, const Vector<T, 2> &high) {
    return Vector<T, 2>(clamp(v.x, low.x, high.x), clamp(v.y, low.y, high.y));
}

template <typename T>
Vector<T, 2> ceil(const Vector<T, 2> &a) {
    return Vector<T, 2>(std::ceil(a.x), std::ceil(a.y));
}

template <typename T>
Vector<T, 2> floor(const Vector<T, 2> &a) {
    return Vector<T, 2>(std::floor(a.x), std::floor(a.y));
}

// Extensions
template <typename T>
Vector<T, 2> monotonicCatmullRom(
        const Vector<T, 2> &v_0, const Vector<T, 2> &v_1, const Vector<T, 2> &v_2, const Vector<T, 2> &v_3, T f) {
    static const T kTwo = static_cast<T>(2);
    static const T kThree = static_cast<T>(3);

    Vector<T, 2> d_1 = (v_2 - v_0) / kTwo;
    Vector<T, 2> d_2 = (v_3 - v_1) / kTwo;
    Vector<T, 2> vd_1 = v_2 - v_1;

    if (std::fabs(vd_1.x) < std::numeric_limits<T>::epsilon() || sign(vd_1.x) != sign(d_1.x) ||
        sign(vd_1.x) != sign(d_2.x)) {
        d_1.x = d_2.x = 0;
    }

    if (std::fabs(vd_1.y) < std::numeric_limits<T>::epsilon() || sign(vd_1.y) != sign(d_1.y) ||
        sign(vd_1.y) != sign(d_2.y)) {
        d_1.y = d_2.y = 0;
    }

    Vector<T, 2> a_3 = d_1 + d_2 - kTwo * vd_1;
    Vector<T, 2> a_2 = kThree * vd_1 - kTwo * d_1 - d_2;
    Vector<T, 2> a_1 = d_1;
    Vector<T, 2> a_0 = v_1;

    return a_3 * cubic(f) + a_2 * square(f) + a_1 * f + a_0;
}

}  // namespace vox
