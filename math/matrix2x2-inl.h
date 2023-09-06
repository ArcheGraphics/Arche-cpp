//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <algorithm>
#include <cstring>
#include <utility>

#include "math/math_utils.h"

namespace vox {

// MARK: - CTOR/DTOR
template <typename T>
Matrix<T, 2, 2>::Matrix() {
    set(1, 0, 0, 1);
}

template <typename T>
Matrix<T, 2, 2>::Matrix(T s) {
    set(s);
}

template <typename T>
Matrix<T, 2, 2>::Matrix(T m_00, T m_10, T m_01, T m_11) {
    set(m_00, m_10, m_01, m_11);
}

template <typename T>
template <typename U>
Matrix<T, 2, 2>::Matrix(const std::initializer_list<std::initializer_list<U>> &lst) {
    set(lst);
}

template <typename T>
Matrix<T, 2, 2>::Matrix(const Matrix &m) {
    set(m);
}

template <typename T>
Matrix<T, 2, 2>::Matrix(const T *arr) {
    set(arr);
}

// MARK: - Basic setters
template <typename T>
void Matrix<T, 2, 2>::set(T s) {
    elements_[0] = elements_[1] = elements_[2] = elements_[3] = s;
}

template <typename T>
void Matrix<T, 2, 2>::set(T m_00, T m_10, T m_01, T m_11) {
    elements_[0] = m_00;
    elements_[1] = m_10;
    elements_[2] = m_01;
    elements_[3] = m_11;
}

template <typename T>
template <typename U>
void Matrix<T, 2, 2>::set(const std::initializer_list<std::initializer_list<U>> &lst) {
    size_t width = lst.size();
    size_t height = (width > 0) ? lst.begin()->size() : 0;
    VOX_ASSERT(width == 2);
    VOX_ASSERT(height == 2);

    auto col_iter = lst.begin();
    for (size_t i = 0; i < width; ++i) {
        VOX_ASSERT(height == col_iter->size());
        auto row_iter = col_iter->begin();
        for (size_t j = 0; j < height; ++j) {
            (*this)(j, i) = static_cast<T>(*row_iter);
            ++row_iter;
        }
        ++col_iter;
    }
}

template <typename T>
void Matrix<T, 2, 2>::set(const Matrix &m) {
    memcpy(elements_.data(), m.elements_.data(), sizeof(T) * 4);
}

template <typename T>
void Matrix<T, 2, 2>::set(const T *arr) {
    memcpy(elements_.data(), arr, sizeof(T) * 4);
}

template <typename T>
void Matrix<T, 2, 2>::setDiagonal(T s) {
    elements_[0] = elements_[3] = s;
}

template <typename T>
void Matrix<T, 2, 2>::setOffDiagonal(T s) {
    elements_[1] = elements_[2] = s;
}

template <typename T>
void Matrix<T, 2, 2>::setRow(size_t i, const Vector<T, 2> &row) {
    elements_[i] = row.x;
    elements_[i + 2] = row.y;
}

template <typename T>
void Matrix<T, 2, 2>::setColumn(size_t j, const Vector<T, 2> &col) {
    elements_[2 * j] = col.x;
    elements_[2 * j + 1] = col.y;
}

// MARK: - Basic getters
template <typename T>
bool Matrix<T, 2, 2>::isSimilar(const Matrix &m, double tol) const {
    return (std::fabs(elements_[0] - m.elements_[0]) < tol) && (std::fabs(elements_[1] - m.elements_[1]) < tol) &&
           (std::fabs(elements_[2] - m.elements_[2]) < tol) && (std::fabs(elements_[3] - m.elements_[3]) < tol);
}

template <typename T>
bool Matrix<T, 2, 2>::isSquare() const {
    return true;
}

template <typename T>
size_t Matrix<T, 2, 2>::rows() const {
    return 2;
}

template <typename T>
size_t Matrix<T, 2, 2>::cols() const {
    return 2;
}

template <typename T>
T *Matrix<T, 2, 2>::data() {
    return elements_.data();
}

template <typename T>
const T *Matrix<T, 2, 2>::data() const {
    return elements_.data();
}

// MARK: - Binary operator methods - new instance = this instance (+) input
template <typename T>
Matrix<T, 2, 2> Matrix<T, 2, 2>::add(T s) const {
    return Matrix(elements_[0] + s, elements_[1] + s, elements_[2] + s, elements_[3] + s);
}

template <typename T>
Matrix<T, 2, 2> Matrix<T, 2, 2>::add(const Matrix &m) const {
    return Matrix(elements_[0] + m.elements_[0], elements_[1] + m.elements_[1], elements_[2] + m.elements_[2],
                  elements_[3] + m.elements_[3]);
}

template <typename T>
Matrix<T, 2, 2> Matrix<T, 2, 2>::sub(T s) const {
    return Matrix(elements_[0] - s, elements_[1] - s, elements_[2] - s, elements_[3] - s);
}

template <typename T>
Matrix<T, 2, 2> Matrix<T, 2, 2>::sub(const Matrix &m) const {
    return Matrix(elements_[0] - m.elements_[0], elements_[1] - m.elements_[1], elements_[2] - m.elements_[2],
                  elements_[3] - m.elements_[3]);
}

template <typename T>
Matrix<T, 2, 2> Matrix<T, 2, 2>::mul(T s) const {
    return Matrix(elements_[0] * s, elements_[1] * s, elements_[2] * s, elements_[3] * s);
}

template <typename T>
Vector<T, 2> Matrix<T, 2, 2>::mul(const Vector<T, 2> &v) const {
    return Vector<T, 2>(elements_[0] * v.x + elements_[2] * v.y, elements_[1] * v.x + elements_[3] * v.y);
}

template <typename T>
Matrix<T, 2, 2> Matrix<T, 2, 2>::mul(const Matrix &m) const {
    return Matrix(elements_[0] * m.elements_[0] + elements_[2] * m.elements_[1],
                  elements_[1] * m.elements_[0] + elements_[3] * m.elements_[1],
                  elements_[0] * m.elements_[2] + elements_[2] * m.elements_[3],
                  elements_[1] * m.elements_[2] + elements_[3] * m.elements_[3]);
}

template <typename T>
Matrix<T, 2, 2> Matrix<T, 2, 2>::div(T s) const {
    return Matrix(elements_[0] / s, elements_[1] / s, elements_[2] / s, elements_[3] / s);
}

// MARK: - Binary operator methods - new instance = input (+) this instance
template <typename T>
Matrix<T, 2, 2> Matrix<T, 2, 2>::radd(T s) const {
    return Matrix(s + elements_[0], s + elements_[1], s + elements_[2], s + elements_[3]);
}

template <typename T>
Matrix<T, 2, 2> Matrix<T, 2, 2>::radd(const Matrix &m) const {
    return Matrix(m.elements_[0] + elements_[0], m.elements_[1] + elements_[1], m.elements_[2] + elements_[2],
                  m.elements_[3] + elements_[3]);
}

template <typename T>
Matrix<T, 2, 2> Matrix<T, 2, 2>::rsub(T s) const {
    return Matrix(s - elements_[0], s - elements_[1], s - elements_[2], s - elements_[3]);
}

template <typename T>
Matrix<T, 2, 2> Matrix<T, 2, 2>::rsub(const Matrix &m) const {
    return Matrix(m.elements_[0] - elements_[0], m.elements_[1] - elements_[1], m.elements_[2] - elements_[2],
                  m.elements_[3] - elements_[3]);
}

template <typename T>
Matrix<T, 2, 2> Matrix<T, 2, 2>::rmul(T s) const {
    return Matrix(s * elements_[0], s * elements_[1], s * elements_[2], s * elements_[3]);
}

template <typename T>
Matrix<T, 2, 2> Matrix<T, 2, 2>::rmul(const Matrix &m) const {
    return m.mul(*this);
}

template <typename T>
Matrix<T, 2, 2> Matrix<T, 2, 2>::rdiv(T s) const {
    return Matrix(s / elements_[0], s / elements_[1], s / elements_[2], s / elements_[3]);
}

// MARK: - Augmented operator methods - this instance (+)= input
template <typename T>
void Matrix<T, 2, 2>::iadd(T s) {
    elements_[0] += s;
    elements_[1] += s;
    elements_[2] += s;
    elements_[3] += s;
}

template <typename T>
void Matrix<T, 2, 2>::iadd(const Matrix &m) {
    elements_[0] += m.elements_[0];
    elements_[1] += m.elements_[1];
    elements_[2] += m.elements_[2];
    elements_[3] += m.elements_[3];
}

template <typename T>
void Matrix<T, 2, 2>::isub(T s) {
    elements_[0] -= s;
    elements_[1] -= s;
    elements_[2] -= s;
    elements_[3] -= s;
}

template <typename T>
void Matrix<T, 2, 2>::isub(const Matrix &m) {
    elements_[0] -= m.elements_[0];
    elements_[1] -= m.elements_[1];
    elements_[2] -= m.elements_[2];
    elements_[3] -= m.elements_[3];
}

template <typename T>
void Matrix<T, 2, 2>::imul(T s) {
    elements_[0] *= s;
    elements_[1] *= s;
    elements_[2] *= s;
    elements_[3] *= s;
}

template <typename T>
void Matrix<T, 2, 2>::imul(const Matrix &m) {
    set(mul(m));
}

template <typename T>
void Matrix<T, 2, 2>::idiv(T s) {
    elements_[0] /= s;
    elements_[1] /= s;
    elements_[2] /= s;
    elements_[3] /= s;
}

// MARK: - Modifiers
template <typename T>
void Matrix<T, 2, 2>::transpose() {
    std::swap(elements_[1], elements_[2]);
}

template <typename T>
void Matrix<T, 2, 2>::invert() {
    T d = determinant();
    Matrix m;
    m.elements_[0] = elements_[3];
    m.elements_[1] = -elements_[1];
    m.elements_[2] = -elements_[2];
    m.elements_[3] = elements_[0];
    m.idiv(d);

    set(m);
}

// MARK: - Complex getters
template <typename T>
T Matrix<T, 2, 2>::sum() const {
    T s = 0;
    for (int i = 0; i < 4; ++i) {
        s += elements_[i];
    }
    return s;
}

template <typename T>
T Matrix<T, 2, 2>::avg() const {
    return sum() / 4;
}

template <typename T>
T Matrix<T, 2, 2>::min() const {
    return std::min(std::min(elements_[0], elements_[1]), std::min(elements_[2], elements_[3]));
}

template <typename T>
T Matrix<T, 2, 2>::max() const {
    return std::max(std::max(elements_[0], elements_[1]), std::max(elements_[2], elements_[3]));
}

template <typename T>
T Matrix<T, 2, 2>::absmin() const {
    return ::vox::absmin(::vox::absmin(elements_[0], elements_[1]), ::vox::absmin(elements_[2], elements_[3]));
}

template <typename T>
T Matrix<T, 2, 2>::absmax() const {
    return ::vox::absmax(::vox::absmax(elements_[0], elements_[1]), ::vox::absmax(elements_[2], elements_[3]));
}

template <typename T>
T Matrix<T, 2, 2>::trace() const {
    return elements_[0] + elements_[3];
}

template <typename T>
T Matrix<T, 2, 2>::determinant() const {
    return elements_[0] * elements_[3] - elements_[1] * elements_[2];
}

template <typename T>
Matrix<T, 2, 2> Matrix<T, 2, 2>::diagonal() const {
    return Matrix(elements_[0], 0, 0, elements_[3]);
}

template <typename T>
Matrix<T, 2, 2> Matrix<T, 2, 2>::offDiagonal() const {
    return Matrix(0, elements_[1], elements_[2], 0);
}

template <typename T>
Matrix<T, 2, 2> Matrix<T, 2, 2>::strictLowerTri() const {
    return Matrix(0, elements_[1], 0, 0);
}

template <typename T>
Matrix<T, 2, 2> Matrix<T, 2, 2>::strictUpperTri() const {
    return Matrix(0, 0, elements_[2], 0);
}

template <typename T>
Matrix<T, 2, 2> Matrix<T, 2, 2>::lowerTri() const {
    return Matrix(elements_[0], elements_[1], 0, elements_[3]);
}

template <typename T>
Matrix<T, 2, 2> Matrix<T, 2, 2>::upperTri() const {
    return Matrix(elements_[0], 0, elements_[2], elements_[3]);
}

template <typename T>
Matrix<T, 2, 2> Matrix<T, 2, 2>::transposed() const {
    return Matrix(elements_[0], elements_[2], elements_[1], elements_[3]);
}

template <typename T>
Matrix<T, 2, 2> Matrix<T, 2, 2>::inverse() const {
    Matrix m(*this);
    m.invert();
    return m;
}

template <typename T>
T Matrix<T, 2, 2>::frobeniusNorm() const {
    return std::sqrt(elements_[0] * elements_[0] + elements_[1] * elements_[1] + elements_[2] * elements_[2] +
                     elements_[3] * elements_[3]);
}

template <typename T>
template <typename U>
Matrix<U, 2, 2> Matrix<T, 2, 2>::castTo() const {
    return Matrix<U, 2, 2>(static_cast<U>(elements_[0]), static_cast<U>(elements_[1]), static_cast<U>(elements_[2]),
                           static_cast<U>(elements_[3]));
}

// MARK: - Setter operators
template <typename T>
Matrix<T, 2, 2> &Matrix<T, 2, 2>::operator=(const Matrix &m) {
    set(m);
    return *this;
}

template <typename T>
Matrix<T, 2, 2> &Matrix<T, 2, 2>::operator+=(T s) {
    iadd(s);
    return *this;
}

template <typename T>
Matrix<T, 2, 2> &Matrix<T, 2, 2>::operator+=(const Matrix &m) {
    iadd(m);
    return *this;
}

template <typename T>
Matrix<T, 2, 2> &Matrix<T, 2, 2>::operator-=(T s) {
    isub(s);
    return *this;
}

template <typename T>
Matrix<T, 2, 2> &Matrix<T, 2, 2>::operator-=(const Matrix &m) {
    isub(m);
    return *this;
}

template <typename T>
Matrix<T, 2, 2> &Matrix<T, 2, 2>::operator*=(T s) {
    imul(s);
    return *this;
}

template <typename T>
Matrix<T, 2, 2> &Matrix<T, 2, 2>::operator*=(const Matrix &m) {
    imul(m);
    return *this;
}

template <typename T>
Matrix<T, 2, 2> &Matrix<T, 2, 2>::operator/=(T s) {
    idiv(s);
    return *this;
}

template <typename T>
bool Matrix<T, 2, 2>::operator==(const Matrix &m) const {
    return elements_[0] == m.elements_[0] && elements_[1] == m.elements_[1] && elements_[2] == m.elements_[2] &&
           elements_[3] == m.elements_[3];
}

template <typename T>
bool Matrix<T, 2, 2>::operator!=(const Matrix &m) const {
    return elements_[0] != m.elements_[0] || elements_[1] != m.elements_[1] || elements_[2] != m.elements_[2] ||
           elements_[3] != m.elements_[3];
}

// MARK: - Getter operators
template <typename T>
T &Matrix<T, 2, 2>::operator[](size_t i) {
    return elements_[i];
}

template <typename T>
const T &Matrix<T, 2, 2>::operator[](size_t i) const {
    return elements_[i];
}

template <typename T>
T &Matrix<T, 2, 2>::operator()(size_t i, size_t j) {
    return elements_[i + j * 2];
}

template <typename T>
const T &Matrix<T, 2, 2>::operator()(size_t i, size_t j) const {
    return elements_[i + j * 2];
}

// MARK: - Helpers
template <typename T>
Matrix<T, 2, 2> Matrix<T, 2, 2>::makeZero() {
    return Matrix(0, 0, 0, 0);
}

template <typename T>
Matrix<T, 2, 2> Matrix<T, 2, 2>::makeIdentity() {
    return Matrix(1, 0, 0, 1);
}

template <typename T>
Matrix<T, 2, 2> Matrix<T, 2, 2>::makeScaleMatrix(T sx, T sy) {
    return Matrix(sx, 0, 0, sy);
}

template <typename T>
Matrix<T, 2, 2> Matrix<T, 2, 2>::makeScaleMatrix(const Vector<T, 2> &s) {
    return makeScaleMatrix(s.x, s.y);
}

template <typename T>
Matrix<T, 2, 2> Matrix<T, 2, 2>::makeRotationMatrix(const T &rad) {
    return Matrix(std::cos(rad), std::sin(rad), -std::sin(rad), std::cos(rad));
}

// MARK: - Operator overloadings
template <typename T>
Matrix<T, 2, 2> operator-(const Matrix<T, 2, 2> &a) {
    return a.mul(-1);
}

template <typename T>
Matrix<T, 2, 2> operator+(const Matrix<T, 2, 2> &a, const Matrix<T, 2, 2> &b) {
    return a.add(b);
}

template <typename T>
Matrix<T, 2, 2> operator+(const Matrix<T, 2, 2> &a, T b) {
    return a.add(b);
}

template <typename T>
Matrix<T, 2, 2> operator+(T a, const Matrix<T, 2, 2> &b) {
    return b.radd(a);
}

template <typename T>
Matrix<T, 2, 2> operator-(const Matrix<T, 2, 2> &a, const Matrix<T, 2, 2> &b) {
    return a.sub(b);
}

template <typename T>
Matrix<T, 2, 2> operator-(const Matrix<T, 2, 2> &a, T b) {
    return a.sub(b);
}

template <typename T>
Matrix<T, 2, 2> operator-(T a, const Matrix<T, 2, 2> &b) {
    return b.rsub(a);
}

template <typename T>
Matrix<T, 2, 2> operator*(const Matrix<T, 2, 2> &a, T b) {
    return a.mul(b);
}

template <typename T>
Matrix<T, 2, 2> operator*(T a, const Matrix<T, 2, 2> &b) {
    return b.rmul(a);
}

template <typename T>
Vector<T, 3> operator*(const Matrix<T, 2, 2> &a, const Vector<T, 3> &b) {
    return a.mul(b);
}

template <typename T>
Vector<T, 2> operator*(const Matrix<T, 2, 2> &a, const Vector<T, 2> &b) {
    return a.mul(b);
}

template <typename T>
Matrix<T, 2, 2> operator*(const Matrix<T, 2, 2> &a, const Matrix<T, 2, 2> &b) {
    return a.mul(b);
}

template <typename T>
Matrix<T, 2, 2> operator/(const Matrix<T, 2, 2> &a, T b) {
    return a.div(b);
}

template <typename T>
Matrix<T, 2, 2> operator/(T a, const Matrix<T, 2, 2> &b) {
    return b.rdiv(a);
}

}  // namespace vox
