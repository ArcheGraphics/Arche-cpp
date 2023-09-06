//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <algorithm>
#include <limits>

#include "math/vector3.h"

namespace vox {
//!
//! \brief 4-D vector class.
//!
//! This class defines simple 4-D vector data.
//!
//! \tparam T - Type of the element
//!
template <typename T>
struct Vector<T, 4> final {
public:
    static_assert(std::is_floating_point<T>::value, "Vector only can be instantiated with floating point types");

    //! X (or the first) component of the vector.
    T x;

    //! Y (or the second) component of the vector.
    T y;

    //! Z (or the third) component of the vector.
    T z;

    //! W (or the fourth) component of the vector.
    T w;

    // MARK: - Constructors

    //! Constructs default vector (0, 0, 0, 0).
    constexpr Vector() : x(0), y(0), z(0), w(0) {}

    //! Constructs vector with given parameters \p x_, \p y_, \p z_, and \p w_.
    constexpr Vector(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {}

    //! Constructs vector with a 3-D vector (x, y, and z) and a scalar (w).
    constexpr Vector(const Vector3<T> &v, T w) : x(v.x), y(v.y), z(v.z), w(w) {}

    //! Constructs vector with initializer list.
    template <typename U>
    Vector(const std::initializer_list<U> &lst);

    //! Copy constructor.
    constexpr Vector(const Vector &v) : x(v.x), y(v.y), z(v.z), w(v.w) {}

    // MARK: - Basic setters

    //! Set both x, y, z, and w components to \p s.
    void set(T s);

    //! Set x, y, z, and w components with given parameters.
    void set(T new_x, T new_y, T new_z, T new_w);

    //! Set x, y, z, and w components with \p pt.x, \p pt.y, \p z, and \p w.
    void set(const Vector<T, 3> &pt, T new_w);

    //! Set x, y, z, and w components with given initializer list.
    template <typename U>
    void set(const std::initializer_list<U> &lst);

    //! Set x, y, z, and w with other vector \p v.
    void set(const Vector &v);

    //! Set all x, y, z, and w to zero.
    void setZero();

    //! Normalizes this vector.
    void normalize();

    // MARK: - Binary operations: new instance = this (+) v

    //! Computes this + (v, v, v, v).
    Vector add(T v) const;

    //! Computes this + (v.x, v.y, v.z, v.w).
    Vector add(const Vector &v) const;

    //! Computes this - (v, v, v, v).
    Vector sub(T v) const;

    //! Computes this - (v.x, v.y, v.z, v.w).
    Vector sub(const Vector &v) const;

    //! Computes this * (v, v, v, v).
    Vector mul(T v) const;

    //! Computes this * (v.x, v.y, v.z, v.w).
    Vector mul(const Vector &v) const;

    //! Computes this / (v, v, v, v).
    Vector div(T v) const;

    //! Computes this / (v.x, v.y, v.z, v.w).
    Vector div(const Vector &v) const;

    //! Computes dot product.
    T dot(const Vector &v) const;

    // MARK: - Binary operations: new instance = v (+) this

    //! Computes (v, v, v, v) - this.
    Vector rsub(T v) const;

    //! Computes (v.x, v.y, v.z, v.w) - this.
    Vector rsub(const Vector &v) const;

    //! Computes (v, v, v, v) / this.
    Vector rdiv(T v) const;

    //! Computes (v.x, v.y, v.z, v.w) / this.
    Vector rdiv(const Vector &v) const;

    // MARK: - Augmented operations: this (+)= v

    //! Computes this += (v, v, v, v).
    void iadd(T v);

    //! Computes this += (v.x, v.y, v.z, v.w).
    void iadd(const Vector &v);

    //! Computes this -= (v, v, v, v).
    void isub(T v);

    //! Computes this -= (v.x, v.y, v.z, v.w).
    void isub(const Vector &v);

    //! Computes this *= (v, v, v, v).
    void imul(T v);

    //! Computes this *= (v.x, v.y, v.z, v.w).
    void imul(const Vector &v);

    //! Computes this /= (v, v, v, v).
    void idiv(T v);

    //! Computes this /= (v.x, v.y, v.z, v.w).
    void idiv(const Vector &v);

    // MARK: - Basic getters

    //! Returns const reference to the \p i -th element of the vector.
    const T &at(size_t i) const;

    //! Returns reference to the \p i -th element of the vector.
    T &at(size_t i);

    //! Returns the sum of all the components (i.e. x + y + z + w).
    T sum() const;

    //! Returns the average of all the components.
    T avg() const;

    //! Returns the minimum value among x, y, z, and w.
    T min() const;

    //! Returns the maximum value among x, y, z, and w.
    T max() const;

    //! Returns the absolute minimum value among x, y, z, and w.
    T absmin() const;

    //! Returns the absolute maximum value among x, y, z, and w.
    T absmax() const;

    //! Returns the index of the dominant axis.
    [[nodiscard]] size_t dominantAxis() const;

    //! Returns the index of the subminant axis.
    [[nodiscard]] size_t subminantAxis() const;

    //! Returns normalized vector.
    Vector normalized() const;

    //! Returns the length of the vector.
    T length() const;

    //! Returns the squared length of the vector.
    T lengthSquared() const;

    //! Returns the distance to the other vector.
    T distanceTo(const Vector &other) const;

    //! Returns the squared distance to the other vector.
    T distanceSquaredTo(const Vector &other) const;

    //! Returns a vector with different value type.
    template <typename U>
    Vector<U, 4> castTo() const;

    //! Returns true if \p other is the same as this vector.
    bool isEqual(const Vector &other) const;

    //! Returns true if \p other is similar to this vector.
    bool isSimilar(const Vector &other, T epsilon = std::numeric_limits<T>::epsilon()) const;

    // MARK: - Operators

    //! Returns reference to the \p i -th element of the vector.
    T &operator[](size_t i);

    //! Returns const reference to the \p i -th element of the vector.
    const T &operator[](size_t i) const;

    //! Set x and y components with given initializer list.
    template <typename U>
    Vector &operator=(const std::initializer_list<U> &lst);

    //! Set x and y with other vector \p v.
    Vector &operator=(const Vector &v);

    //! Computes this += (v, v, v, v)
    Vector &operator+=(T v);

    //! Computes this += (v.x, v.y, v.z, v.w)
    Vector &operator+=(const Vector &v);

    //! Computes this -= (v, v, v, v)
    Vector &operator-=(T v);

    //! Computes this -= (v.x, v.y, v.z, v.w)
    Vector &operator-=(const Vector &v);

    //! Computes this *= (v, v, v, v)
    Vector &operator*=(T v);

    //! Computes this *= (v.x, v.y, v.z, v.w)
    Vector &operator*=(const Vector &v);

    //! Computes this /= (v, v, v, v)
    Vector &operator/=(T v);

    //! Computes this /= (v.x, v.y, v.z, v.w)
    Vector &operator/=(const Vector &v);

    //! Returns true if \p other is the same as this vector.
    bool operator==(const Vector &v) const;

    //! Returns true if \p other is the not same as this vector.
    bool operator!=(const Vector &v) const;
};

//! Type alias for four dimensional vector.
template <typename T>
using Vector4 = Vector<T, 4>;

//! Positive sign operator.
template <typename T>
Vector4<T> operator+(const Vector4<T> &a);

//! Negative sign operator.
template <typename T>
Vector4<T> operator-(const Vector4<T> &a);

//! Computes (a, a, a, a) + (b.x, b.y, b.z, b.w).
template <typename T>
Vector4<T> operator+(T a, const Vector4<T> &b);

//! Computes (a.x, a.y, a.z, a.w) + (b, b, b, b).
template <typename T>
Vector4<T> operator+(const Vector4<T> &a, T b);

//! Computes (a.x, a.y, a.z, a.w) + (b.x, b.y, b.z, b.w).
template <typename T>
Vector4<T> operator+(const Vector4<T> &a, const Vector4<T> &b);

//! Computes (a.x, a.y, a.z, a.w) - (b, b, b, b).
template <typename T>
Vector4<T> operator-(const Vector4<T> &a, T b);

//! Computes (a, a, a, a) - (b.x, b.y, b.z, b.w).
template <typename T>
Vector4<T> operator-(T a, const Vector4<T> &b);

//! Computes (a.x, a.y, a.z, a.w) - (b.x, b.y, b.z, b.w).
template <typename T>
Vector4<T> operator-(const Vector4<T> &a, const Vector4<T> &b);

//! Computes (a.x, a.y, a.z, a.w) * (b, b, b, b).
template <typename T>
Vector4<T> operator*(const Vector4<T> &a, T b);

//! Computes (a, a, a, a) * (b.x, b.y, b.z, b.w).
template <typename T>
Vector4<T> operator*(T a, const Vector4<T> &b);

//! Computes (a.x, a.y, a.z, a.w) * (b.x, b.y, b.z, b.w).
template <typename T>
Vector4<T> operator*(const Vector4<T> &a, const Vector4<T> &b);

//! Computes (a.x, a.y, a.z, a.w) / (b, b, b, b).
template <typename T>
Vector4<T> operator/(const Vector4<T> &a, T b);

//! Computes (a, a, a, a) / (b.x, b.y, b.z, b.w).
template <typename T>
Vector4<T> operator/(T a, const Vector4<T> &b);

//! Computes (a.x, a.y, a.z, a.w) / (b.x, b.y, b.z, b.w).
template <typename T>
Vector4<T> operator/(const Vector4<T> &a, const Vector4<T> &b);

//! Returns element-wise min vector.
template <typename T>
Vector4<T> min(const Vector4<T> &a, const Vector4<T> &b);

//! Returns element-wise max vector.
template <typename T>
Vector4<T> max(const Vector4<T> &a, const Vector4<T> &b);

//! Returns element-wise clamped vector.
template <typename T>
Vector4<T> clamp(const Vector4<T> &v, const Vector4<T> &low, const Vector4<T> &high);

//! Returns element-wise ceiled vector.
template <typename T>
Vector4<T> ceil(const Vector4<T> &a);

//! Returns element-wise floored vector.
template <typename T>
Vector4<T> floor(const Vector4<T> &a);

//! Float-type 4D vector.
using Vector4F = Vector4<float>;

//! Double-type 4D vector.
using Vector4D = Vector4<double>;

// MARK: - Extensions

//! Returns float-type zero vector.
template <>
constexpr Vector4F zero<Vector4F>() {
    return Vector4F(0.f, 0.f, 0.f, 0.f);
}

//! Returns double-type zero vector.
template <>
constexpr Vector4D zero<Vector4D>() {
    return Vector4D(0.0, 0.0, 0.0, 0.0);
}

//! Returns the type of the value itself.
template <typename T>
struct ScalarType<Vector4<T>> {
    using value = T;
};

//! Computes monotonic Catmull-Rom interpolation.
template <typename T>
Vector4<T> monotonicCatmullRom(
        const Vector4<T> &v_0, const Vector4<T> &v_1, const Vector4<T> &v_2, const Vector4<T> &v_3, T f);

}  // namespace vox

#include "math/vector4-inl.h"
