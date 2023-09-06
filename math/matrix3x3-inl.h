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
Matrix<T, 3, 3>::Matrix() {
    set(1, 0, 0, 0, 1, 0, 0, 0, 1);
}

template <typename T>
Matrix<T, 3, 3>::Matrix(T s) {
    set(s);
}

template <typename T>
Matrix<T, 3, 3>::Matrix(T m_00, T m_10, T m_20, T m_01, T m_11, T m_21, T m_02, T m_12, T m_22) {
    set(m_00, m_10, m_20, m_01, m_11, m_21, m_02, m_12, m_22);
}

template <typename T>
template <typename U>
Matrix<T, 3, 3>::Matrix(const std::initializer_list<std::initializer_list<U>> &lst) {
    set(lst);
}

template <typename T>
Matrix<T, 3, 3>::Matrix(const Matrix &m) {
    set(m);
}

template <typename T>
Matrix<T, 3, 3>::Matrix(const T *arr) {
    set(arr);
}

// MARK: - Basic setters
template <typename T>
void Matrix<T, 3, 3>::set(T s) {
    elements_[0] = elements_[3] = elements_[6] = elements_[1] = elements_[4] = elements_[7] = elements_[2] =
            elements_[5] = elements_[8] = s;
}

template <typename T>
void Matrix<T, 3, 3>::set(T m_00, T m_10, T m_20, T m_01, T m_11, T m_21, T m_02, T m_12, T m_22) {
    elements_[0] = m_00;
    elements_[1] = m_10;
    elements_[2] = m_20;
    elements_[3] = m_01;
    elements_[4] = m_11;
    elements_[5] = m_21;
    elements_[6] = m_02;
    elements_[7] = m_12;
    elements_[8] = m_22;
}

template <typename T>
template <typename U>
void Matrix<T, 3, 3>::set(const std::initializer_list<std::initializer_list<U>> &lst) {
    size_t width = lst.size();
    size_t height = (width > 0) ? lst.begin()->size() : 0;
    VOX_ASSERT(width == 3);
    VOX_ASSERT(height == 3);

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
void Matrix<T, 3, 3>::set(const Matrix &m) {
    memcpy(elements_.data(), m.elements_.data(), sizeof(T) * 9);
}

template <typename T>
void Matrix<T, 3, 3>::set(const T *arr) {
    memcpy(elements_.data(), arr, sizeof(T) * 9);
}

template <typename T>
void Matrix<T, 3, 3>::setDiagonal(T s) {
    elements_[0] = elements_[4] = elements_[8] = s;
}

template <typename T>
void Matrix<T, 3, 3>::setOffDiagonal(T s) {
    elements_[1] = elements_[2] = elements_[3] = elements_[5] = elements_[6] = elements_[7] = s;
}

template <typename T>
void Matrix<T, 3, 3>::setRow(size_t i, const Vector<T, 3> &row) {
    elements_[i] = row.x;
    elements_[i + 3] = row.y;
    elements_[i + 6] = row.z;
}

template <typename T>
void Matrix<T, 3, 3>::setColumn(size_t j, const Vector<T, 3> &col) {
    elements_[3 * j] = col.x;
    elements_[3 * j + 1] = col.y;
    elements_[3 * j + 2] = col.z;
}

// MARK: - Basic getters
template <typename T>
bool Matrix<T, 3, 3>::isSimilar(const Matrix &m, double tol) const {
    return std::fabs(elements_[0] - m.elements_[0]) < tol && std::fabs(elements_[1] - m.elements_[1]) < tol &&
           std::fabs(elements_[2] - m.elements_[2]) < tol && std::fabs(elements_[3] - m.elements_[3]) < tol &&
           std::fabs(elements_[4] - m.elements_[4]) < tol && std::fabs(elements_[5] - m.elements_[5]) < tol &&
           std::fabs(elements_[6] - m.elements_[6]) < tol && std::fabs(elements_[7] - m.elements_[7]) < tol &&
           std::fabs(elements_[8] - m.elements_[8]) < tol;
}

template <typename T>
bool Matrix<T, 3, 3>::isSquare() const {
    return true;
}

template <typename T>
size_t Matrix<T, 3, 3>::rows() const {
    return 3;
}

template <typename T>
size_t Matrix<T, 3, 3>::cols() const {
    return 3;
}

template <typename T>
T *Matrix<T, 3, 3>::data() {
    return elements_.data();
}

template <typename T>
const T *Matrix<T, 3, 3>::data() const {
    return elements_.data();
}

// MARK: - Binary operator methods - new instance = this instance (+) input
template <typename T>
Matrix<T, 3, 3> Matrix<T, 3, 3>::add(T s) const {
    return Matrix(elements_[0] + s, elements_[1] + s, elements_[2] + s, elements_[3] + s, elements_[4] + s,
                  elements_[5] + s, elements_[6] + s, elements_[7] + s, elements_[8] + s);
}

template <typename T>
Matrix<T, 3, 3> Matrix<T, 3, 3>::add(const Matrix &m) const {
    return Matrix(elements_[0] + m.elements_[0], elements_[1] + m.elements_[1], elements_[2] + m.elements_[2],
                  elements_[3] + m.elements_[3], elements_[4] + m.elements_[4], elements_[5] + m.elements_[5],
                  elements_[6] + m.elements_[6], elements_[7] + m.elements_[7], elements_[8] + m.elements_[8]);
}

template <typename T>
Matrix<T, 3, 3> Matrix<T, 3, 3>::sub(T s) const {
    return Matrix(elements_[0] - s, elements_[1] - s, elements_[2] - s, elements_[3] - s, elements_[4] - s,
                  elements_[5] - s, elements_[6] - s, elements_[7] - s, elements_[8] - s);
}

template <typename T>
Matrix<T, 3, 3> Matrix<T, 3, 3>::sub(const Matrix &m) const {
    return Matrix(elements_[0] - m.elements_[0], elements_[1] - m.elements_[1], elements_[2] - m.elements_[2],
                  elements_[3] - m.elements_[3], elements_[4] - m.elements_[4], elements_[5] - m.elements_[5],
                  elements_[6] - m.elements_[6], elements_[7] - m.elements_[7], elements_[8] - m.elements_[8]);
}

template <typename T>
Matrix<T, 3, 3> Matrix<T, 3, 3>::mul(T s) const {
    return Matrix(elements_[0] * s, elements_[1] * s, elements_[2] * s, elements_[3] * s, elements_[4] * s,
                  elements_[5] * s, elements_[6] * s, elements_[7] * s, elements_[8] * s);
}

template <typename T>
Point<T, 3> Matrix<T, 3, 3>::mul(const Point<T, 3> &v) const {
    return Point<T, 3>(elements_[0] * v.x + elements_[3] * v.y + elements_[6] * v.z,
                       elements_[1] * v.x + elements_[4] * v.y + elements_[7] * v.z,
                       elements_[2] * v.x + elements_[5] * v.y + elements_[8] * v.z);
}

template <typename T>
Vector<T, 3> Matrix<T, 3, 3>::mul(const Vector<T, 3> &v) const {
    return Vector<T, 3>(elements_[0] * v.x + elements_[3] * v.y + elements_[6] * v.z,
                        elements_[1] * v.x + elements_[4] * v.y + elements_[7] * v.z,
                        elements_[2] * v.x + elements_[5] * v.y + elements_[8] * v.z);
}

template <typename T>
Matrix<T, 3, 3> Matrix<T, 3, 3>::mul(const Matrix &m) const {
    return Matrix(elements_[0] * m.elements_[0] + elements_[3] * m.elements_[1] + elements_[6] * m.elements_[2],
                  elements_[1] * m.elements_[0] + elements_[4] * m.elements_[1] + elements_[7] * m.elements_[2],
                  elements_[2] * m.elements_[0] + elements_[5] * m.elements_[1] + elements_[8] * m.elements_[2],

                  elements_[0] * m.elements_[3] + elements_[3] * m.elements_[4] + elements_[6] * m.elements_[5],
                  elements_[1] * m.elements_[3] + elements_[4] * m.elements_[4] + elements_[7] * m.elements_[5],
                  elements_[2] * m.elements_[3] + elements_[5] * m.elements_[4] + elements_[8] * m.elements_[5],

                  elements_[0] * m.elements_[6] + elements_[3] * m.elements_[7] + elements_[6] * m.elements_[8],
                  elements_[1] * m.elements_[6] + elements_[4] * m.elements_[7] + elements_[7] * m.elements_[8],
                  elements_[2] * m.elements_[6] + elements_[5] * m.elements_[7] + elements_[8] * m.elements_[8]);
}

template <typename T>
Matrix<T, 3, 3> Matrix<T, 3, 3>::div(T s) const {
    return Matrix(elements_[0] / s, elements_[1] / s, elements_[2] / s, elements_[3] / s, elements_[4] / s,
                  elements_[5] / s, elements_[6] / s, elements_[7] / s, elements_[8] / s);
}

// MARK: - Binary operator methods - new instance = input (+) this instance
template <typename T>
Matrix<T, 3, 3> Matrix<T, 3, 3>::radd(T s) const {
    return Matrix(s + elements_[0], s + elements_[1], s + elements_[2], s + elements_[3], s + elements_[4],
                  s + elements_[5], s + elements_[6], s + elements_[7], s + elements_[8]);
}

template <typename T>
Matrix<T, 3, 3> Matrix<T, 3, 3>::radd(const Matrix &m) const {
    return Matrix(m.elements_[0] + elements_[0], m.elements_[1] + elements_[1], m.elements_[2] + elements_[2],
                  m.elements_[3] + elements_[3], m.elements_[4] + elements_[4], m.elements_[5] + elements_[5],
                  m.elements_[6] + elements_[6], m.elements_[7] + elements_[7], m.elements_[8] + elements_[8]);
}

template <typename T>
Matrix<T, 3, 3> Matrix<T, 3, 3>::rsub(T s) const {
    return Matrix(s - elements_[0], s - elements_[1], s - elements_[2], s - elements_[3], s - elements_[4],
                  s - elements_[5], s - elements_[6], s - elements_[7], s - elements_[8]);
}

template <typename T>
Matrix<T, 3, 3> Matrix<T, 3, 3>::rsub(const Matrix &m) const {
    return Matrix(m.elements_[0] - elements_[0], m.elements_[1] - elements_[1], m.elements_[2] - elements_[2],
                  m.elements_[3] - elements_[3], m.elements_[4] - elements_[4], m.elements_[5] - elements_[5],
                  m.elements_[6] - elements_[6], m.elements_[7] - elements_[7], m.elements_[8] - elements_[8]);
}

template <typename T>
Matrix<T, 3, 3> Matrix<T, 3, 3>::rmul(T s) const {
    return Matrix(s * elements_[0], s * elements_[1], s * elements_[2], s * elements_[3], s * elements_[4],
                  s * elements_[5], s * elements_[6], s * elements_[7], s * elements_[8]);
}

template <typename T>
Matrix<T, 3, 3> Matrix<T, 3, 3>::rmul(const Matrix &m) const {
    return m.mul(*this);
}

template <typename T>
Matrix<T, 3, 3> Matrix<T, 3, 3>::rdiv(T s) const {
    return Matrix(s / elements_[0], s / elements_[1], s / elements_[2], s / elements_[3], s / elements_[4],
                  s / elements_[5], s / elements_[6], s / elements_[7], s / elements_[8]);
}

// MARK: - Augmented operator methods - this instance (+)= input
template <typename T>
void Matrix<T, 3, 3>::iadd(T s) {
    elements_[0] += s;
    elements_[1] += s;
    elements_[2] += s;
    elements_[3] += s;
    elements_[4] += s;
    elements_[5] += s;
    elements_[6] += s;
    elements_[7] += s;
    elements_[8] += s;
}

template <typename T>
void Matrix<T, 3, 3>::iadd(const Matrix &m) {
    elements_[0] += m.elements_[0];
    elements_[1] += m.elements_[1];
    elements_[2] += m.elements_[2];
    elements_[3] += m.elements_[3];
    elements_[4] += m.elements_[4];
    elements_[5] += m.elements_[5];
    elements_[6] += m.elements_[6];
    elements_[7] += m.elements_[7];
    elements_[8] += m.elements_[8];
}

template <typename T>
void Matrix<T, 3, 3>::isub(T s) {
    elements_[0] -= s;
    elements_[1] -= s;
    elements_[2] -= s;
    elements_[3] -= s;
    elements_[4] -= s;
    elements_[5] -= s;
    elements_[6] -= s;
    elements_[7] -= s;
    elements_[8] -= s;
}

template <typename T>
void Matrix<T, 3, 3>::isub(const Matrix &m) {
    elements_[0] -= m.elements_[0];
    elements_[1] -= m.elements_[1];
    elements_[2] -= m.elements_[2];
    elements_[3] -= m.elements_[3];
    elements_[4] -= m.elements_[4];
    elements_[5] -= m.elements_[5];
    elements_[6] -= m.elements_[6];
    elements_[7] -= m.elements_[7];
    elements_[8] -= m.elements_[8];
}

template <typename T>
void Matrix<T, 3, 3>::imul(T s) {
    elements_[0] *= s;
    elements_[1] *= s;
    elements_[2] *= s;
    elements_[3] *= s;
    elements_[4] *= s;
    elements_[5] *= s;
    elements_[6] *= s;
    elements_[7] *= s;
    elements_[8] *= s;
}

template <typename T>
void Matrix<T, 3, 3>::imul(const Matrix &m) {
    set(mul(m));
}

template <typename T>
void Matrix<T, 3, 3>::idiv(T s) {
    elements_[0] /= s;
    elements_[1] /= s;
    elements_[2] /= s;
    elements_[3] /= s;
    elements_[4] /= s;
    elements_[5] /= s;
    elements_[6] /= s;
    elements_[7] /= s;
    elements_[8] /= s;
}

// MARK: - Modifiers
template <typename T>
void Matrix<T, 3, 3>::transpose() {
    std::swap(elements_[1], elements_[3]);
    std::swap(elements_[2], elements_[6]);
    std::swap(elements_[5], elements_[7]);
}

template <typename T>
void Matrix<T, 3, 3>::invert() {
    T d = determinant();

    Matrix m;
    m.elements_[0] = elements_[4] * elements_[8] - elements_[5] * elements_[7];
    m.elements_[1] = elements_[2] * elements_[7] - elements_[1] * elements_[8];
    m.elements_[2] = elements_[1] * elements_[5] - elements_[2] * elements_[4];
    m.elements_[3] = elements_[5] * elements_[6] - elements_[3] * elements_[8];
    m.elements_[4] = elements_[0] * elements_[8] - elements_[2] * elements_[6];
    m.elements_[5] = elements_[2] * elements_[3] - elements_[0] * elements_[5];
    m.elements_[6] = elements_[3] * elements_[7] - elements_[4] * elements_[6];
    m.elements_[7] = elements_[1] * elements_[6] - elements_[0] * elements_[7];
    m.elements_[8] = elements_[0] * elements_[4] - elements_[1] * elements_[3];
    m.idiv(d);

    set(m);
}

// MARK: - Complex getters
template <typename T>
T Matrix<T, 3, 3>::sum() const {
    T s = 0;
    for (int i = 0; i < 9; ++i) {
        s += elements_[i];
    }
    return s;
}

template <typename T>
T Matrix<T, 3, 3>::avg() const {
    return sum() / 9;
}

template <typename T>
T Matrix<T, 3, 3>::min() const {
    return minn(data(), 9);
}

template <typename T>
T Matrix<T, 3, 3>::max() const {
    return maxn(data(), 9);
}

template <typename T>
T Matrix<T, 3, 3>::absmin() const {
    return absminn(data(), 9);
}

template <typename T>
T Matrix<T, 3, 3>::absmax() const {
    return absmaxn(data(), 9);
}

template <typename T>
T Matrix<T, 3, 3>::trace() const {
    return elements_[0] + elements_[4] + elements_[8];
}

template <typename T>
T Matrix<T, 3, 3>::determinant() const {
    return elements_[0] * elements_[4] * elements_[8] - elements_[0] * elements_[5] * elements_[7] +
           elements_[1] * elements_[5] * elements_[6] - elements_[1] * elements_[3] * elements_[8] +
           elements_[2] * elements_[3] * elements_[7] - elements_[2] * elements_[4] * elements_[6];
}

template <typename T>
Matrix<T, 3, 3> Matrix<T, 3, 3>::diagonal() const {
    return Matrix(elements_[0], 0, 0, 0, elements_[4], 0, 0, 0, elements_[8]);
}

template <typename T>
Matrix<T, 3, 3> Matrix<T, 3, 3>::offDiagonal() const {
    return Matrix(0, elements_[1], elements_[2], elements_[3], 0, elements_[5], elements_[6], elements_[7], 0);
}

template <typename T>
Matrix<T, 3, 3> Matrix<T, 3, 3>::strictLowerTri() const {
    return Matrix(0, elements_[1], elements_[2], 0, 0, elements_[5], 0, 0, 0);
}

template <typename T>
Matrix<T, 3, 3> Matrix<T, 3, 3>::strictUpperTri() const {
    return Matrix(0, 0, 0, elements_[3], 0, 0, elements_[6], elements_[7], 0);
}

template <typename T>
Matrix<T, 3, 3> Matrix<T, 3, 3>::lowerTri() const {
    return Matrix(elements_[0], elements_[1], elements_[2], 0, elements_[4], elements_[5], 0, 0, elements_[8]);
}

template <typename T>
Matrix<T, 3, 3> Matrix<T, 3, 3>::upperTri() const {
    return Matrix(elements_[0], 0, 0, elements_[3], elements_[4], 0, elements_[6], elements_[7], elements_[8]);
}

template <typename T>
Matrix<T, 3, 3> Matrix<T, 3, 3>::transposed() const {
    return Matrix(elements_[0], elements_[3], elements_[6], elements_[1], elements_[4], elements_[7], elements_[2],
                  elements_[5], elements_[8]);
}

template <typename T>
Matrix<T, 3, 3> Matrix<T, 3, 3>::inverse() const {
    Matrix m(*this);
    m.invert();
    return m;
}

template <typename T>
Matrix<T, 3, 3> Matrix<T, 3, 3>::rotate(T r) const {
    T s = std::sin(r);
    T c = std::cos(r);

    const T &a_11 = elements_[0], kA12 = elements_[1], kA13 = elements_[2];
    const T &a_21 = elements_[3], kA22 = elements_[4], kA23 = elements_[5];
    const T &a_31 = elements_[6], kA32 = elements_[7], kA33 = elements_[8];

    Matrix<T, 3, 3> result;
    result[0] = c * a_11 + s * a_21;
    result[1] = c * kA12 + s * kA22;
    result[2] = c * kA13 + s * kA23;

    result[3] = c * a_21 - s * a_11;
    result[4] = c * kA22 - s * kA12;
    result[5] = c * kA23 - s * kA13;

    result[6] = a_31;
    result[7] = kA32;
    result[8] = kA33;
    return result;
}

template <typename T>
void Matrix<T, 3, 3>::rotate(T r) {
    T s = std::sin(r);
    T c = std::cos(r);

    const T &a_11 = elements_[0], kA12 = elements_[1], kA13 = elements_[2];
    const T &a_21 = elements_[3], kA22 = elements_[4], kA23 = elements_[5];

    elements_[0] = c * a_11 + s * a_21;
    elements_[1] = c * kA12 + s * kA22;
    elements_[2] = c * kA13 + s * kA23;

    elements_[3] = c * a_21 - s * a_11;
    elements_[4] = c * kA22 - s * kA12;
    elements_[5] = c * kA23 - s * kA13;
}

template <typename T>
T Matrix<T, 3, 3>::frobeniusNorm() const {
    return std::sqrt(elements_[0] * elements_[0] + elements_[1] * elements_[1] + elements_[2] * elements_[2] +
                     elements_[3] * elements_[3] + elements_[4] * elements_[4] + elements_[5] * elements_[5] +
                     elements_[6] * elements_[6] + elements_[7] * elements_[7] + elements_[8] * elements_[8]);
}

template <typename T>
template <typename U>
Matrix<U, 3, 3> Matrix<T, 3, 3>::castTo() const {
    return Matrix<U, 3, 3>(static_cast<U>(elements_[0]), static_cast<U>(elements_[1]), static_cast<U>(elements_[2]),
                           static_cast<U>(elements_[3]), static_cast<U>(elements_[4]), static_cast<U>(elements_[5]),
                           static_cast<U>(elements_[6]), static_cast<U>(elements_[7]), static_cast<U>(elements_[8]));
}

// MARK: - Setter operators
template <typename T>
Matrix<T, 3, 3> &Matrix<T, 3, 3>::operator=(const Matrix &m) {
    set(m);
    return *this;
}

template <typename T>
Matrix<T, 3, 3> &Matrix<T, 3, 3>::operator+=(T s) {
    iadd(s);
    return *this;
}

template <typename T>
Matrix<T, 3, 3> &Matrix<T, 3, 3>::operator+=(const Matrix &m) {
    iadd(m);
    return *this;
}

template <typename T>
Matrix<T, 3, 3> &Matrix<T, 3, 3>::operator-=(T s) {
    isub(s);
    return *this;
}

template <typename T>
Matrix<T, 3, 3> &Matrix<T, 3, 3>::operator-=(const Matrix &m) {
    isub(m);
    return *this;
}

template <typename T>
Matrix<T, 3, 3> &Matrix<T, 3, 3>::operator*=(T s) {
    imul(s);
    return *this;
}

template <typename T>
Matrix<T, 3, 3> &Matrix<T, 3, 3>::operator*=(const Matrix &m) {
    imul(m);
    return *this;
}

template <typename T>
Matrix<T, 3, 3> &Matrix<T, 3, 3>::operator/=(T s) {
    idiv(s);
    return *this;
}

template <typename T>
bool Matrix<T, 3, 3>::operator==(const Matrix &m) const {
    return elements_[0] == m.elements_[0] && elements_[1] == m.elements_[1] && elements_[2] == m.elements_[2] &&
           elements_[3] == m.elements_[3] && elements_[4] == m.elements_[4] && elements_[5] == m.elements_[5] &&
           elements_[6] == m.elements_[6] && elements_[7] == m.elements_[7] && elements_[8] == m.elements_[8];
}

template <typename T>
bool Matrix<T, 3, 3>::operator!=(const Matrix &m) const {
    return elements_[0] != m.elements_[0] || elements_[1] != m.elements_[1] || elements_[2] != m.elements_[2] ||
           elements_[3] != m.elements_[3] || elements_[4] != m.elements_[4] || elements_[5] != m.elements_[5] ||
           elements_[6] != m.elements_[6] || elements_[7] != m.elements_[7] || elements_[8] != m.elements_[8];
}

// MARK: - Getter operators
template <typename T>
T &Matrix<T, 3, 3>::operator[](size_t i) {
    return elements_[i];
}

template <typename T>
const T &Matrix<T, 3, 3>::operator[](size_t i) const {
    return elements_[i];
}

template <typename T>
T &Matrix<T, 3, 3>::operator()(size_t i, size_t j) {
    return elements_[i + 3 * j];
}

template <typename T>
const T &Matrix<T, 3, 3>::operator()(size_t i, size_t j) const {
    return elements_[i + 3 * j];
}

// MARK: - Helpers
template <typename T>
Matrix<T, 3, 3> Matrix<T, 3, 3>::makeZero() {
    return Matrix(0, 0, 0, 0, 0, 0, 0, 0, 0);
}

template <typename T>
Matrix<T, 3, 3> Matrix<T, 3, 3>::makeIdentity() {
    return Matrix(1, 0, 0, 0, 1, 0, 0, 0, 1);
}

template <typename T>
Matrix<T, 3, 3> Matrix<T, 3, 3>::makeScaleMatrix(T sx, T sy, T sz) {
    return Matrix(sx, 0, 0, 0, sy, 0, 0, 0, sz);
}

template <typename T>
Matrix<T, 3, 3> Matrix<T, 3, 3>::makeScaleMatrix(const Vector<T, 3> &s) {
    return makeScaleMatrix(s.x, s.y, s.z);
}

template <typename T>
Matrix<T, 3, 3> Matrix<T, 3, 3>::makeRotationMatrix(const Vector<T, 3> &axis, T rad) {
    return Matrix(1 + (1 - std::cos(rad)) * (axis.x * axis.x - 1),
                  axis.z * std::sin(rad) + (1 - std::cos(rad)) * axis.x * axis.y,
                  -axis.y * std::sin(rad) + (1 - std::cos(rad)) * axis.x * axis.z,

                  -axis.z * std::sin(rad) + (1 - std::cos(rad)) * axis.x * axis.y,
                  1 + (1 - std::cos(rad)) * (axis.y * axis.y - 1),
                  axis.x * std::sin(rad) + (1 - std::cos(rad)) * axis.y * axis.z,

                  axis.y * std::sin(rad) + (1 - std::cos(rad)) * axis.x * axis.z,
                  -axis.x * std::sin(rad) + (1 - std::cos(rad)) * axis.y * axis.z,
                  1 + (1 - std::cos(rad)) * (axis.z * axis.z - 1));
}

// MARK: - Operator overloadings
template <typename T>
Matrix<T, 3, 3> operator-(const Matrix<T, 3, 3> &a) {
    return a.mul(-1);
}

template <typename T>
Matrix<T, 3, 3> operator+(const Matrix<T, 3, 3> &a, const Matrix<T, 3, 3> &b) {
    return a.add(b);
}

template <typename T>
Matrix<T, 3, 3> operator+(const Matrix<T, 3, 3> &a, T b) {
    return a.add(b);
}

template <typename T>
Matrix<T, 3, 3> operator+(T a, const Matrix<T, 3, 3> &b) {
    return b.radd(a);
}

template <typename T>
Matrix<T, 3, 3> operator-(const Matrix<T, 3, 3> &a, const Matrix<T, 3, 3> &b) {
    return a.sub(b);
}

template <typename T>
Matrix<T, 3, 3> operator-(const Matrix<T, 3, 3> &a, T b) {
    return a.sub(b);
}

template <typename T>
Matrix<T, 3, 3> operator-(T a, const Matrix<T, 3, 3> &b) {
    return b.rsub(a);
}

template <typename T>
Matrix<T, 3, 3> operator*(const Matrix<T, 3, 3> &a, T b) {
    return a.mul(b);
}

template <typename T>
Matrix<T, 3, 3> operator*(T a, const Matrix<T, 3, 3> &b) {
    return b.rmul(a);
}

template <typename T>
Point<T, 3> operator*(const Matrix<T, 3, 3> &a, const Point<T, 3> &b) {
    return a.mul(b);
}

template <typename T>
Vector<T, 3> operator*(const Matrix<T, 3, 3> &a, const Vector<T, 3> &b) {
    return a.mul(b);
}

template <typename T>
Matrix<T, 3, 3> operator*(const Matrix<T, 3, 3> &a, const Matrix<T, 3, 3> &b) {
    return a.mul(b);
}

template <typename T>
Matrix<T, 3, 3> operator/(const Matrix<T, 3, 3> &a, T b) {
    return a.div(b);
}

template <typename T>
Matrix<T, 3, 3> operator/(T a, const Matrix<T, 3, 3> &b) {
    return b.rdiv(a);
}

}  // namespace vox
