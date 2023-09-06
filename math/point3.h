//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <algorithm>

#include "math/point2.h"
#include "math/vector3.h"

namespace vox {
//!
//! \brief 3-D point class.
//!
//! This class defines simple 3-D point data.
//!
//! \tparam T - Type of the element
//!
template <typename T>
struct Point<T, 3> {
public:
    static_assert(std::is_arithmetic<T>::value, "Point only can be instantiated with arithmetic types");

    //! X (or the first) component of the point.
    T x;

    //! Y (or the second) component of the point.
    T y;

    //! Z (or the third) component of the point.
    T z;

    // MARK: - Constructors

    //! Constructs default point (0, 0, 0).
    constexpr Point() : x(0), y(0), z(0) {}

    //! Constructs point with given parameters \p x_, \p y_, and \p z_.
    constexpr Point(T x, T y, T z) : x(x), y(y), z(z) {}

    //! Constructs point with a 2-D point and a scalar.
    constexpr Point(const Point2<T> &v, T z) : x(v.x), y(v.y), z(z) {}

    //! Constructs point with initializer list.
    template <typename U>
    Point(const std::initializer_list<U> &lst);

    //! Copy constructor.
    constexpr Point(const Point &v) : x(v.x), y(v.y), z(v.z) {}

    // MARK: - Basic setters

    //! Set all x, y, and z components to \p s.
    void set(T s);

    //! Set x, y, and z components with given parameters.
    void set(T new_x, T new_y, T new_z);

    //! Set x, y, and z components with given \p pt.x, \p pt.y, and \p z.
    void set(const Point2<T> &pt, T new_z);

    //! Set x, y, and z components with given initializer list.
    template <typename U>
    void set(const std::initializer_list<U> &lst);

    //! Set x, y, and z with other point \p pt.
    void set(const Point &v);

    //! Set both x, y, and z to zero.
    void setZero();

    // MARK: - Binary operations: new instance = this (+) v

    //! Computes this + (v, v, v).
    Point add(T v) const;

    //! Computes this + (v.x, v.y, v.z).
    Vector3<T> add(const Point &v) const;

    //! Computes this + (v.x, v.y, v.z).
    Point add(const Vector3<T> &v) const;

    //! Computes this - (v, v, v).
    Point sub(T v) const;

    //! Computes this - (v.x, v.y, v.z).
    Vector3<T> sub(const Point &v) const;

    //! Computes this - (v.x, v.y, v.z).
    Point sub(const Vector3<T> &v) const;

    //! Computes this * (v, v, v).
    Point mul(T v) const;

    //! Computes this * (v.x, v.y, v.z).
    Point mul(const Point &v) const;

    //! Computes this / (v, v, v).
    Point div(T v) const;

    //! Computes this / (v.x, v.y, v.z).
    Point div(const Point &v) const;

    //! Computes dot product.
    T dot(const Vector3<T> &v) const;

    //! Computes cross product.
    Vector3<T> cross(const Point &v) const;

    // MARK: - Binary operations: new instance = v (+) this

    //! Computes (v, v, v) - this.
    Point rsub(T v) const;

    //! Computes (v.x, v.y, v.z) - this.
    Vector3<T> rsub(const Point &v) const;

    //! Computes (v, v, v) / this.
    Point rdiv(T v) const;

    //! Computes (v.x, v.y, v.z) / this.
    Point rdiv(const Point &v) const;

    // MARK: - Augmented operations: this (+)= v

    //! Computes this += (v, v, v).
    void iadd(T v);

    //! Computes this += (v.x, v.y, v.z).
    void iadd(const Vector3<T> &v);

    //! Computes this -= (v, v, v).
    void isub(T v);

    //! Computes this -= (v.x, v.y, v.z).
    void isub(const Vector3<T> &v);

    //! Computes this *= (v, v, v).
    void imul(T v);

    //! Computes this *= (v.x, v.y, v.z).
    void imul(const Point &v);

    //! Computes this /= (v, v, v).
    void idiv(T v);

    //! Computes this /= (v.x, v.y, v.z).
    void idiv(const Point &v);

    // MARK: - Basic getters

    //! Returns const reference to the \p i -th element of the point.
    const T &at(size_t i) const;

    //! Returns reference to the \p i -th element of the point.
    T &at(size_t i);

    //! Returns the sum of all the components (i.e. x + y).
    T sum() const;

    //! Returns the minimum value among x, y, and z.
    T min() const;

    //! Returns the maximum value among x, y, and z.
    T max() const;

    //! Returns the absolute minimum value among x, y, and z.
    T absmin() const;

    //! Returns the absolute maximum value among x, y, and z.
    T absmax() const;

    //! Returns the index of the dominant axis.
    [[nodiscard]] size_t dominantAxis() const;

    //! Returns the index of the subminant axis.
    [[nodiscard]] size_t subminantAxis() const;

    //! Returns a point with different value type.
    template <typename U>
    Point<U, 3> castTo() const;

    //! Returns true if \p other is the same as this point.
    bool isEqual(const Point &other) const;

    //! Returns true if \p other is similar to this vector.
    bool isSimilar(const Point &other, T epsilon = std::numeric_limits<T>::epsilon()) const;

    //! Returns the distance to the other vector.
    T distanceTo(const Point &other) const;

    //! Returns the squared distance to the other vector.
    T distanceSquaredTo(const Point &other) const;

    //! Returns the reflection vector to the surface with given surface normal.
    Point reflected(const Vector3<T> &normal) const;

    //! Returns the projected vector to the surface with given surface normal.
    Point projected(const Vector3<T> &normal) const;

    // MARK: - Operators

    //! Returns reference to the \p i -th element of the point.
    T &operator[](size_t i);

    //! Returns const reference to the \p i -th element of the point.
    const T &operator[](size_t i) const;

    //! Set x, y, and z components with given initializer list.
    Point &operator=(const std::initializer_list<T> &lst);

    //! Set x, y, and z with other point \p pt.
    Point &operator=(const Point &v);

    //! Computes this += (v, v, v)
    Point &operator+=(T v);

    //! Computes this += (v.x, v.y, v.z)
    Point &operator+=(const Vector3<T> &v);

    //! Computes this -= (v, v, v)
    Point &operator-=(T v);

    //! Computes this -= (v.x, v.y, v.z)
    Point &operator-=(const Vector3<T> &v);

    //! Computes this *= (v, v, v)
    Point &operator*=(T v);

    //! Computes this *= (v.x, v.y, v.z)
    Point &operator*=(const Point &v);

    //! Computes this /= (v, v, v)
    Point &operator/=(T v);

    //! Computes this /= (v.x, v.y, v.z)
    Point &operator/=(const Point &v);

    //! Returns true if \p other is the same as this point.
    bool operator==(const Point &v) const;

    //! Returns true if \p other is the not same as this point.
    bool operator!=(const Point &v) const;
};

//! Type alias for three dimensional point.
template <typename T>
using Point3 = Point<T, 3>;

//! Positive sign operator.
template <typename T>
Point<T, 3> operator+(const Point<T, 3> &a);

//! Negative sign operator.
template <typename T>
Point3<T> operator-(const Point3<T> &a);

//! Computes (a, a, a) + (b.x, b.y, b.z).
template <typename T>
Point3<T> operator+(T a, const Point3<T> &b);

//! Computes (a.x, a.y, a.z) + (b.x, b.y, b.z).
template <typename T>
Vector3<T> operator+(const Point3<T> &a, const Point3<T> &b);

//! Computes (a.x, a.y, a.z) + (b.x, b.y, b.z).
template <typename T>
Point3<T> operator+(const Point3<T> &a, const Vector3<T> &b);

//! Computes (a.x, a.y, a.z) - (b, b, b).
template <typename T>
Point3<T> operator-(const Point3<T> &a, T b);

//! Computes (a, a, a) - (b.x, b.y, b.z).
template <typename T>
Point3<T> operator-(T a, const Point3<T> &b);

//! Computes (a.x, a.y, a.z) - (b.x, b.y, b.z).
template <typename T>
Vector3<T> operator-(const Point3<T> &a, const Point3<T> &b);

//! Computes (a.x, a.y, a.z) - (b.x, b.y, b.z).
template <typename T>
Point3<T> operator-(const Point3<T> &a, const Vector3<T> &b);

//! Computes (a.x, a.y, a.z) * (b, b, b).
template <typename T>
Point3<T> operator*(const Point3<T> &a, T b);

//! Computes (a, a, a) * (b.x, b.y, b.z).
template <typename T>
Point3<T> operator*(T a, const Point3<T> &b);

//! Computes (a.x, a.y, a.z) * (b.x, b.y, b.z).
template <typename T>
Point3<T> operator*(const Point3<T> &a, const Point3<T> &b);

//! Computes (a.x, a.y, a.z) / (b, b, b).
template <typename T>
Point3<T> operator/(const Point3<T> &a, T b);

//! Computes (a, a, a) / (b.x, b.y, b.z).
template <typename T>
Point3<T> operator/(T a, const Point3<T> &b);

//! Computes (a.x, a.y, a.z) / (b.x, b.y, b.z).
template <typename T>
Point3<T> operator/(const Point3<T> &a, const Point3<T> &b);

//! Returns element-wise min point.
template <typename T>
Point3<T> min(const Point3<T> &a, const Point3<T> &b);

//! Returns element-wise max point.
template <typename T>
Point3<T> max(const Point3<T> &a, const Point3<T> &b);

//! Returns element-wise clamped point.
template <typename T>
Point3<T> clamp(const Point3<T> &v, const Point3<T> &low, const Point3<T> &high);

//! Returns element-wise ceiled point.
template <typename T>
Point3<T> ceil(const Point3<T> &a);

//! Returns element-wise floored point.
template <typename T>
Point3<T> floor(const Point3<T> &a);

//! Float-type 3D point.
using Point3F = Point3<float>;

//! Double-type 3D point.
using Point3D = Point3<double>;

//! Integer-type 3D point.
using Point3I = Point3<ssize_t>;

//! Unsigned integer-type 3D point.
using Point3UI = Point3<size_t>;

}  // namespace vox

#include "math/point3-inl.h"
