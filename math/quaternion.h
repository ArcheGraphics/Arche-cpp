//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "math/matrix4x4.h"

namespace vox {

//!
//! \brief Quaternion class defined as q = w + xi + yj + zk.
//!
template <typename T>
struct Quaternion {
public:
    static_assert(std::is_floating_point<T>::value, "Quaternion only can be instantiated with floating point types");

    //! Real part.
    T w;

    //!< Imaginary part (i).
    T x;

    //!< Imaginary part (j).
    T y;

    //!< Imaginary part (k).
    T z;

    // MARK: - Constructors

    //! Make an identity quaternion.
    Quaternion();

    //! Constructs a quaternion with given elements.
    Quaternion(T new_x, T new_y, T new_z, T new_w);

    //! Constructs a quaternion with given elements.
    Quaternion(const std::initializer_list<T> &lst);

    //! Constructs a quaternion with given rotation axis and angle.
    Quaternion(const Vector3<T> &axis, T angle);

    //! Constructs a quaternion with from and to vectors.
    Quaternion(const Vector3<T> &from, const Vector3<T> &to);

    //! Constructs a quaternion with three basis vectors.
    Quaternion(const Vector3<T> &rotation_basis_0,
               const Vector3<T> &rotation_basis_1,
               const Vector3<T> &rotation_basis_2);

    //! Constructs a quaternion with 3x3 rotational matrix.
    explicit Quaternion(const Matrix3x3<T> &matrix);

    //! Copy constructor.
    Quaternion(const Quaternion &other);

    // MARK: - Basic setters

    //! Sets the quaternion with other quaternion.
    void set(const Quaternion &other);

    //! Sets the quaternion with given elements.
    void set(T new_x, T new_y, T new_z, T new_w);

    //! Sets the quaternion with given elements.
    void set(const std::initializer_list<T> &lst);

    //! Sets the quaternion with given rotation axis and angle.
    void set(const Vector3<T> &axis, T angle);

    //! Sets the quaternion with from and to vectors.
    void set(const Vector3<T> &from, const Vector3<T> &to);

    //! Sets quaternion with three basis vectors.
    void set(const Vector3<T> &rotation_basis_0,
             const Vector3<T> &rotation_basis_1,
             const Vector3<T> &rotation_basis_2);

    //! Sets the quaternion with 3x3 rotational matrix.
    void set(const Matrix3x3<T> &matrix);

    // MARK: - Basic getters

    //! Returns quaternion with other base type.
    template <typename U>
    Quaternion<U> castTo() const;

    //! Returns normalized quaternion.
    Quaternion normalized() const;

    // MARK: - Binary operator methods - new instance = this instance (+) input
    //! Computes this + (v.x, v.y, v.z, v.w).
    Quaternion add(const Quaternion &v) const;

    //! Computes this * (v, v, v, v).
    Quaternion mul(T v) const;

    //! Returns this quaternion * vector.
    Point3<T> mul(const Point3<T> &v) const;

    //! Returns this quaternion * vector.
    Vector3<T> mul(const Vector3<T> &v) const;

    //! Returns this quaternion * vector.
    Vector4<T> mul(const Vector4<T> &v) const;

    //! Returns this quaternion * other quaternion.
    Quaternion mul(const Quaternion &other) const;

    //! Computes the dot product with other quaternion.
    T dot(const Quaternion<T> &other) const;

    // MARK: - Binary operator methods - new instance = input (+) this instance

    //! Returns other quaternion * this quaternion.
    Quaternion rmul(const Quaternion &other) const;

    // MARK: - Augmented operator methods - this instance (+)= input
    //! Computes this += (v.x, v.y, v.z, v.w).
    void iadd(const Quaternion &v);

    //! Computes this *= (v, v, v, v).
    void imul(T v);

    //! Returns this quaternion *= other quaternion.
    void imul(const Quaternion &other);

    // MARK: - Modifiers

    //! Makes this quaternion identity.
    void setIdentity();

    //! Rotate this quaternion with given angle in radians.
    void rotate(T angle_in_radians);

    //! Normalizes the quaternion.
    void normalize();

    /**
     * Calculate a quaternion that the specified quaternion rotate around X axis.
     * @param rad - The rotation angle in radians
     * @return The calculated quaternion
     */
    Quaternion rotateX(T rad) const;

    /**
     * Calculate this quaternion rotate around X axis.
     * @param rad - The rotation angle in radians
     */
    void rotateX(T rad);

    /**
     * Calculate a quaternion that the specified quaternion rotate around Y axis.
     * @param rad - The rotation angle in radians
     * @return The calculated quaternion
     */
    Quaternion rotateY(T rad) const;

    /**
     * Calculate this quaternion rotate around Y axis.
     * @param rad - The rotation angle in radians
     */
    void rotateY(T rad);

    /**
     * Calculate a quaternion that the specified quaternion rotate around Z axis.
     * @param rad - The rotation angle in radians
     * @return The calculated quaternion
     */
    Quaternion rotateZ(T rad) const;

    /**
     * Calculate this quaternion rotate around Z axis.
     * @param rad - The rotation angle in radians
     */
    void rotateZ(T rad);

    /**
     * Calculate this quaternion rotation around an arbitrary axis.
     * @param axis - The axis
     * @param rad - The rotation angle in radians
     */
    void rotateAxisAngle(const Vector3<T> &axis, T rad);

    // MARK: - Complex getters

    //! Returns the rotational axis.
    Vector3<T> axis() const;

    //! Returns the rotational angle.
    T angle() const;

    //! Returns the axis and angle.
    void getAxisAngle(Vector3<T> *axis, T *angle) const;

    /**
     * Get the euler of this quaternion.
     * @returns Euler x->pitch y->yaw z->roll
     */
    Vector3<T> toEuler() const;

    /**
     * Get the euler of this quaternion.
     * @returns Euler x->yaw y->pitch z->roll
     */
    Vector3<T> toYawPitchRoll() const;

    /**
     * Calculate quaternion that contains conjugated version of the specified quaternion.
     * @return The conjugate version of the specified quaternion
     */
    Quaternion conjugate() const;

    /**
     * Calculate the length of this quaternion.
     * @returns The length of this quaternion
     */
    T length() const;

    /**
     * Calculates the squared length of this quaternion.
     * @returns The squared length of this quaternion
     */
    T lengthSquared() const;

    //! Returns the inverse quaternion.
    Quaternion inverse() const;

    //! Converts to the 3x3 rotation matrix.
    Matrix3x3<T> matrix3() const;

    //! Converts to the 4x4 rotation matrix.
    Matrix4x4<T> matrix4() const;

    //! Returns L2 norm of this quaternion.
    T l2Norm() const;

    // MARK: - Setter operators

    //! Assigns other quaternion.
    Quaternion &operator=(const Quaternion &other);

    //! Computes this += (v.x, v.y, v.z, v.w)
    Quaternion &operator+=(const Quaternion &v);

    //! Computes this *= (v, v, v, v)
    Quaternion &operator*=(T v);

    //! Returns this quaternion *= other quaternion.
    Quaternion &operator*=(const Quaternion &other);

    // MARK: - Getter operators

    //! Returns the reference to the i-th element.
    T &operator[](size_t i);

    //! Returns the const reference to the i-th element.
    const T &operator[](size_t i) const;

    //! Returns true if equal.
    bool operator==(const Quaternion &other) const;

    //! Returns true if not equal.
    bool operator!=(const Quaternion &other) const;

    // MARK: - Builders

    //! Returns identity matrix.
    static Quaternion makeIdentity();

    /**
     * Calculate a quaternion rotates around x, y, z axis (pitch/yaw/roll).
     * @param pitch - The radian of rotation around X (pitch)
     * @param yaw - The radian of rotation around Y (yaw)
     * @param roll - The radian of rotation around Z (roll)
     * @return The calculated quaternion
     */
    static Quaternion makeRotationEuler(T pitch, T yaw, T roll);

    /**
     * Calculate a quaternion from the specified yaw, pitch and roll angles.
     * @param yaw - Yaw around the y axis in radians
     * @param pitch - Pitch around the x axis in radians
     * @param roll - Roll around the z axis in radians
     * @return The calculated quaternion
     */
    static Quaternion makeRotationYawPitchRoll(T yaw, T pitch, T roll);

    /**
     * Calculate a quaternion rotate around X axis.
     * @param rad - The rotation angle in radians
     * @return The calculated quaternion
     */
    static Quaternion makeRotationX(T rad);

    /**
     * Calculate a quaternion rotate around Y axis.
     * @param rad - The rotation angle in radians
     * @return The calculated quaternion
     */
    static Quaternion makeRotationY(T rad);

    /**
     * Calculate a quaternion rotate around Z axis.
     * @param rad - The rotation angle in radians
     * @return The calculated quaternion
     */
    static Quaternion makeRotationZ(T rad);
};

//! Computes spherical linear interpolation.
template <typename T>
Quaternion<T> slerp(const Quaternion<T> &a, const Quaternion<T> &b, T t);

//! Computes (a.x, a.y, a.z, a.w) + (b.x, b.y, b.z, b.w).
template <typename T>
Quaternion<T> operator+(const Quaternion<T> &a, const Quaternion<T> &b);

//! Computes (a.x, a.y, a.z, a.w) * (b, b, b, b).
template <typename T>
Quaternion<T> operator*(const Quaternion<T> &a, T b);

//! Computes (a, a, a, a) * (b.x, b.y, b.z, b.w).
template <typename T>
Quaternion<T> operator*(T a, const Quaternion<T> &b);

//! Returns quaternion q * vector v.
template <typename T>
Point<T, 3> operator*(const Quaternion<T> &q, const Point<T, 3> &v);

//! Returns quaternion q * vector v.
template <typename T>
Vector<T, 3> operator*(const Quaternion<T> &q, const Vector<T, 3> &v);

//! Returns quaternion q * vector v.
template <typename T>
Vector<T, 4> operator*(const Quaternion<T> &q, const Vector<T, 4> &v);

//! Returns quaternion a times quaternion b.
template <typename T>
Quaternion<T> operator*(const Quaternion<T> &a, const Quaternion<T> &b);

//! Float-type quaternion.
using QuaternionF = Quaternion<float>;

//! Double-type quaternion.
using QuaternionD = Quaternion<double>;

}  // namespace vox

#include "math/quaternion-inl.h"
