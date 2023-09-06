//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <algorithm>

#include "math/point.h"
#include "math/vector2.h"

namespace vox {
//!
//! \brief 2-D point class.
//!
//! This class defines simple 2-D point data.
//!
//! \tparam T - Type of the element
//!
template <typename T>
struct Point<T, 2> {
public:
    static_assert(std::is_arithmetic<T>::value, "Point only can be instantiated with arithmetic types");

    //! X (or the first) component of the point.
    T x;

    //! Y (or the second) component of the point.
    T y;

    // MARK: - Constructors

    //! Constructs default point (0, 0).
    constexpr Point() : x(0), y(0) {}

    //! Constructs point with given parameters \p x_ and \p y_.
    constexpr Point(T x, T y) : x(x), y(y) {}

    //! Constructs point with initializer list.
    template <typename U>
    Point(const std::initializer_list<U> &lst);

    //! Copy constructor.
    constexpr Point(const Point &v) : x(v.x), y(v.y) {}

    // MARK: - Basic setters

    //! Set both x and y components to \p s.
    void set(T s);

    //! Set x and y components with given parameters.
    void set(T new_x, T new_y);

    //! Set x and y components with given initializer list.
    template <typename U>
    void set(const std::initializer_list<U> &lst);

    //! Set x and y with other point \p pt.
    void set(const Point &v);

    //! Set both x and y to zero.
    void setZero();

    // MARK: - Binary operations: new instance = this (+) v

    //! Computes this + (v, v).
    Point add(T v) const;

    //! Computes this + (v.x, v.y).
    Vector2<T> add(const Point &v) const;

    //! Computes this + (v.x, v.y).
    Point add(const Vector2<T> &v) const;

    //! Computes this - (v, v).
    Point sub(T v) const;

    //! Computes this - (v.x, v.y).
    Vector2<T> sub(const Point &v) const;

    //! Computes this - (v.x, v.y).
    Point sub(const Vector2<T> &v) const;

    //! Computes this * (v, v).
    Point mul(T v) const;

    //! Computes this * (v.x, v.y).
    Point mul(const Point &v) const;

    //! Computes this / (v, v).
    Point div(T v) const;

    //! Computes this / (v.x, v.y).
    Point div(const Point &v) const;

    //! Computes dot product.
    T dot(const Vector2<T> &v) const;

    // MARK: - Binary operations: new instance = v (+) this

    //! Computes (v, v) - this.
    Point rsub(T v) const;

    //! Computes (v.x, v.y) - this.
    Vector2<T> rsub(const Point &v) const;

    //! Computes (v, v) / this.
    Point rdiv(T v) const;

    //! Computes (v.x, v.y) / this.
    Point rdiv(const Point &v) const;

    // MARK: - Augmented operations: this (+)= v

    //! Computes this += (v, v).
    void iadd(T v);

    //! Computes this += (v.x, v.y).
    void iadd(const Vector2<T> &v);

    //! Computes this -= (v, v).
    void isub(T v);

    //! Computes this -= (v.x, v.y).
    void isub(const Vector2<T> &v);

    //! Computes this *= (v, v).
    void imul(T v);

    //! Computes this *= (v.x, v.y).
    void imul(const Point &v);

    //! Computes this /= (v, v).
    void idiv(T v);

    //! Computes this /= (v.x, v.y).
    void idiv(const Point &v);

    // MARK: - Basic getters

    //! Returns const reference to the \p i -th element of the point.
    const T &at(size_t i) const;

    //! Returns reference to the \p i -th element of the point.
    T &at(size_t i);

    //! Returns the sum of all the components (i.e. x + y).
    T sum() const;

    //! Returns the minimum value among x and y.
    T min() const;

    //! Returns the maximum value among x and y.
    T max() const;

    //! Returns the absolute minimum value among x and y.
    T absmin() const;

    //! Returns the absolute maximum value among x and y.
    T absmax() const;

    //! Returns the index of the dominant axis.
    [[nodiscard]] size_t dominantAxis() const;

    //! Returns the index of the subminant axis.
    [[nodiscard]] size_t subminantAxis() const;

    //! Returns a point with different value type.
    template <typename U>
    Point<U, 2> castTo() const;

    //! Returns true if \p other is the same as this point.
    bool isEqual(const Point &other) const;

    //! Returns true if \p other is similar to this vector.
    bool isSimilar(const Point &other, T epsilon = std::numeric_limits<T>::epsilon()) const;

    //! Returns the distance to the other vector.
    T distanceTo(const Point &other) const;

    //! Returns the squared distance to the other vector.
    T distanceSquaredTo(const Point &other) const;

    //! Returns the reflection vector to the surface with given surface normal.
    Point reflected(const Vector2<T> &normal) const;

    //! Returns the projected vector to the surface with given surface normal.
    Point projected(const Vector2<T> &normal) const;

    // MARK: - Operators

    //! Returns reference to the \p i -th element of the point.
    T &operator[](size_t i);

    //! Returns const reference to the \p i -th element of the point.
    const T &operator[](size_t i) const;

    //! Set x and y components with given initializer list.
    Point &operator=(const std::initializer_list<T> &lst);

    //! Set x and y with other point \p pt.
    Point &operator=(const Point &v);

    //! Computes this += (v, v)
    Point &operator+=(T v);

    //! Computes this += (v.x, v.y)
    Point &operator+=(const Vector2<T> &v);

    //! Computes this -= (v, v)
    Point &operator-=(T v);

    //! Computes this -= (v.x, v.y)
    Point &operator-=(const Vector2<T> &v);

    //! Computes this *= (v, v)
    Point &operator*=(T v);

    //! Computes this *= (v.x, v.y)
    Point &operator*=(const Point &v);

    //! Computes this /= (v, v)
    Point &operator/=(T v);

    //! Computes this /= (v.x, v.y)
    Point &operator/=(const Point &v);

    //! Returns true if \p other is the same as this point.
    bool operator==(const Point &v) const;

    //! Returns true if \p other is the not same as this point.
    bool operator!=(const Point &v) const;
};

//! Type alias for two dimensional point.
template <typename T>
using Point2 = Point<T, 2>;

//! Positive sign operator.
template <typename T>
Point<T, 2> operator+(const Point<T, 2> &a);

//! Negative sign operator.
template <typename T>
Point2<T> operator-(const Point2<T> &a);

//! Computes (a, a) + (b.x, b.y).
template <typename T>
Point2<T> operator+(T a, const Point2<T> &b);

//! Computes (a.x, a.y) + (b.x, b.y).
template <typename T>
Vector2<T> operator+(const Point2<T> &a, const Point2<T> &b);

//! Computes (a.x, a.y) + (b.x, b.y).
template <typename T>
Point2<T> operator+(const Point2<T> &a, const Vector2<T> &b);

//! Computes (a.x, a.y) - (b, b).
template <typename T>
Point2<T> operator-(const Point2<T> &a, T b);

//! Computes (a, a) - (b.x, b.y).
template <typename T>
Point2<T> operator-(T a, const Point2<T> &b);

//! Computes (a.x, a.y) - (b.x, b.y).
template <typename T>
Vector2<T> operator-(const Point2<T> &a, const Point2<T> &b);

//! Computes (a.x, a.y) - (b.x, b.y).
template <typename T>
Point2<T> operator-(const Point2<T> &a, const Vector2<T> &b);

//! Computes (a.x, a.y) * (b, b).
template <typename T>
Point2<T> operator*(const Point2<T> &a, T b);

//! Computes (a, a) * (b.x, b.y).
template <typename T>
Point2<T> operator*(T a, const Point2<T> &b);

//! Computes (a.x, a.y) * (b.x, b.y).
template <typename T>
Point2<T> operator*(const Point2<T> &a, const Point2<T> &b);

//! Computes (a.x, a.y) / (b, b).
template <typename T>
Point2<T> operator/(const Point2<T> &a, T b);

//! Computes (a, a) / (b.x, b.y).
template <typename T>
Point2<T> operator/(T a, const Point2<T> &b);

//! Computes (a.x, a.y) / (b.x, b.y).
template <typename T>
Point2<T> operator/(const Point2<T> &a, const Point2<T> &b);

//! Returns element-wise min point: (min(a.x, b.x), min(a.y, b.y)).
template <typename T>
Point2<T> min(const Point2<T> &a, const Point2<T> &b);

//! Returns element-wise max point: (max(a.x, b.x), max(a.y, b.y)).
template <typename T>
Point2<T> max(const Point2<T> &a, const Point2<T> &b);

//! Returns element-wise clamped point.
template <typename T>
Point2<T> clamp(const Point2<T> &v, const Point2<T> &low, const Point2<T> &high);

//! Returns element-wise ceiled point.
template <typename T>
Point2<T> ceil(const Point2<T> &a);

//! Returns element-wise floored point.
template <typename T>
Point2<T> floor(const Point2<T> &a);

//! Float-type 2D point.
using Point2F = Point2<float>;

//! Double-type 2D point.
using Point2D = Point2<double>;

//! Integer-type 2D point.
using Point2I = Point2<ssize_t>;

//! Unsigned integer-type 2D point.
using Point2UI = Point2<size_t>;

}  // namespace vox

#include "math/point2-inl.h"
