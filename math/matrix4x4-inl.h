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
Matrix<T, 4, 4>::Matrix() {
    set(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
}

template <typename T>
Matrix<T, 4, 4>::Matrix(T s) {
    set(s);
}

template <typename T>
Matrix<T, 4, 4>::Matrix(T m_00, T m_10, T m_20, T m_01, T m_11, T m_21, T m_02, T m_12, T m_22) {
    set(m_00, m_10, m_20, m_01, m_11, m_21, m_02, m_12, m_22);
}

template <typename T>
Matrix<T, 4, 4>::Matrix(T m_00,
                        T m_10,
                        T m_20,
                        T m_30,
                        T m_01,
                        T m_11,
                        T m_21,
                        T m_31,
                        T m_02,
                        T m_12,
                        T m_22,
                        T m_32,
                        T m_03,
                        T m_13,
                        T m_23,
                        T m_33) {
    set(m_00, m_10, m_20, m_30, m_01, m_11, m_21, m_31, m_02, m_12, m_22, m_32, m_03, m_13, m_23, m_33);
}

template <typename T>
template <typename U>
Matrix<T, 4, 4>::Matrix(const std::initializer_list<std::initializer_list<U>> &lst) {
    set(lst);
}

template <typename T>
Matrix<T, 4, 4>::Matrix(const Matrix<T, 3, 3> &m_33) {
    set(m_33);
}

template <typename T>
Matrix<T, 4, 4>::Matrix(const Matrix &m) {
    set(m);
}

template <typename T>
Matrix<T, 4, 4>::Matrix(const T *arr) {
    set(arr);
}

// MARK: - Basic setters
template <typename T>
void Matrix<T, 4, 4>::set(T s) {
    elements_[0] = elements_[4] = elements_[8] = elements_[12] = elements_[1] = elements_[5] = elements_[9] =
            elements_[13] = elements_[2] = elements_[6] = elements_[10] = elements_[14] = elements_[3] = elements_[7] =
                    elements_[11] = elements_[15] = s;
}

template <typename T>
void Matrix<T, 4, 4>::set(T m_00, T m_10, T m_20, T m_01, T m_11, T m_21, T m_02, T m_12, T m_22) {
    elements_[0] = m_00;
    elements_[1] = m_10;
    elements_[2] = m_20;
    elements_[3] = 0;
    elements_[4] = m_01;
    elements_[5] = m_11;
    elements_[6] = m_21;
    elements_[7] = 0;
    elements_[8] = m_02;
    elements_[9] = m_12;
    elements_[10] = m_22;
    elements_[11] = 0;
    elements_[12] = 0;
    elements_[13] = 0;
    elements_[14] = 0;
    elements_[15] = 1;
}

template <typename T>
void Matrix<T, 4, 4>::set(T m_00,
                          T m_10,
                          T m_20,
                          T m_30,
                          T m_01,
                          T m_11,
                          T m_21,
                          T m_31,
                          T m_02,
                          T m_12,
                          T m_22,
                          T m_32,
                          T m_03,
                          T m_13,
                          T m_23,
                          T m_33) {
    elements_[0] = m_00;
    elements_[1] = m_10;
    elements_[2] = m_20;
    elements_[3] = m_30;
    elements_[4] = m_01;
    elements_[5] = m_11;
    elements_[6] = m_21;
    elements_[7] = m_31;
    elements_[8] = m_02;
    elements_[9] = m_12;
    elements_[10] = m_22;
    elements_[11] = m_32;
    elements_[12] = m_03;
    elements_[13] = m_13;
    elements_[14] = m_23;
    elements_[15] = m_33;
}

template <typename T>
template <typename U>
void Matrix<T, 4, 4>::set(const std::initializer_list<std::initializer_list<U>> &lst) {
    size_t width = lst.size();
    size_t height = (width > 0) ? lst.begin()->size() : 0;
    VOX_ASSERT(width == 4);
    VOX_ASSERT(height == 4);

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
void Matrix<T, 4, 4>::set(const Matrix<T, 3, 3> &m_33) {
    set(m_33[0], m_33[1], m_33[2], 0, m_33[3], m_33[4], m_33[5], 0, m_33[6], m_33[7], m_33[8], 0, 0, 0, 0, 1);
}

template <typename T>
void Matrix<T, 4, 4>::set(const Matrix &m) {
    elements_ = m.elements_;
}

template <typename T>
void Matrix<T, 4, 4>::set(const T *arr) {
    memcpy(elements_.data(), arr, sizeof(T) * 16);
}

template <typename T>
void Matrix<T, 4, 4>::setDiagonal(T s) {
    elements_[0] = elements_[5] = elements_[10] = elements_[15] = s;
}

template <typename T>
void Matrix<T, 4, 4>::setOffDiagonal(T s) {
    elements_[1] = elements_[2] = elements_[3] = elements_[4] = elements_[6] = elements_[7] = elements_[8] =
            elements_[9] = elements_[11] = elements_[12] = elements_[13] = elements_[14] = s;
}

template <typename T>
void Matrix<T, 4, 4>::setRow(size_t i, const Vector<T, 4> &row) {
    elements_[i] = row.x;
    elements_[i + 4] = row.y;
    elements_[i + 8] = row.z;
    elements_[i + 12] = row.w;
}

template <typename T>
void Matrix<T, 4, 4>::setColumn(size_t j, const Vector<T, 4> &col) {
    elements_[4 * j] = col.x;
    elements_[4 * j + 1] = col.y;
    elements_[4 * j + 2] = col.z;
    elements_[4 * j + 3] = col.w;
}

// MARK: - Basic getters
template <typename T>
bool Matrix<T, 4, 4>::isSimilar(const Matrix &m, double tol) const {
    return std::fabs(elements_[0] - m.elements_[0]) < tol && std::fabs(elements_[1] - m.elements_[1]) < tol &&
           std::fabs(elements_[2] - m.elements_[2]) < tol && std::fabs(elements_[3] - m.elements_[3]) < tol &&
           std::fabs(elements_[4] - m.elements_[4]) < tol && std::fabs(elements_[5] - m.elements_[5]) < tol &&
           std::fabs(elements_[6] - m.elements_[6]) < tol && std::fabs(elements_[7] - m.elements_[7]) < tol &&
           std::fabs(elements_[8] - m.elements_[8]) < tol && std::fabs(elements_[9] - m.elements_[9]) < tol &&
           std::fabs(elements_[10] - m.elements_[10]) < tol && std::fabs(elements_[11] - m.elements_[11]) < tol &&
           std::fabs(elements_[12] - m.elements_[12]) < tol && std::fabs(elements_[13] - m.elements_[13]) < tol &&
           std::fabs(elements_[14] - m.elements_[14]) < tol && std::fabs(elements_[15] - m.elements_[15]) < tol;
}

template <typename T>
bool Matrix<T, 4, 4>::isSquare() const {
    return true;
}

template <typename T>
size_t Matrix<T, 4, 4>::rows() const {
    return 4;
}

template <typename T>
size_t Matrix<T, 4, 4>::cols() const {
    return 4;
}

template <typename T>
T *Matrix<T, 4, 4>::data() {
    return elements_.data();
}

template <typename T>
const T *Matrix<T, 4, 4>::data() const {
    return elements_.data();
}

template <typename T>
Matrix<T, 3, 3> Matrix<T, 4, 4>::matrix3() const {
    return Matrix<T, 3, 3>(elements_[0], elements_[1], elements_[2], elements_[4], elements_[5], elements_[6],
                           elements_[8], elements_[9], elements_[10]);
}

template <typename T>
Matrix<T, 3, 3> Matrix<T, 4, 4>::normalMatrix() const {
    T a_11 = elements_[0], a_12 = elements_[1], a_13 = elements_[2], a_14 = elements_[3];
    T a_21 = elements_[4], a_22 = elements_[5], a_23 = elements_[6], a_24 = elements_[7];
    T a_31 = elements_[8], a_32 = elements_[9], a_33 = elements_[10], a_34 = elements_[11];
    T a_41 = elements_[12], a_42 = elements_[13], a_43 = elements_[14], a_44 = elements_[15];

    T b_00 = a_11 * a_22 - a_12 * a_21;
    T b_01 = a_11 * a_23 - a_13 * a_21;
    T b_02 = a_11 * a_24 - a_14 * a_21;
    T b_03 = a_12 * a_23 - a_13 * a_22;
    T b_04 = a_12 * a_24 - a_14 * a_22;
    T b_05 = a_13 * a_24 - a_14 * a_23;
    T b_06 = a_31 * a_42 - a_32 * a_41;
    T b_07 = a_31 * a_43 - a_33 * a_41;
    T b_08 = a_31 * a_44 - a_34 * a_41;
    T b_09 = a_32 * a_43 - a_33 * a_42;
    T b_10 = a_32 * a_44 - a_34 * a_42;
    T b_11 = a_33 * a_44 - a_34 * a_43;

    T det = b_00 * b_11 - b_01 * b_10 + b_02 * b_09 + b_03 * b_08 - b_04 * b_07 + b_05 * b_06;
    if (!det) {
        return Matrix<T, 3, 3>();
    }
    det = 1.0 / det;

    return Matrix<T, 3, 3>(
            (a_22 * b_11 - a_23 * b_10 + a_24 * b_09) * det, (a_23 * b_08 - a_21 * b_11 - a_24 * b_07) * det,
            (a_21 * b_10 - a_22 * b_08 + a_24 * b_06) * det,

            (a_13 * b_10 - a_12 * b_11 - a_14 * b_09) * det, (a_11 * b_11 - a_13 * b_08 + a_14 * b_07) * det,
            (a_12 * b_08 - a_11 * b_10 - a_14 * b_06) * det,

            (a_42 * b_05 - a_43 * b_04 + a_44 * b_03) * det, (a_43 * b_02 - a_41 * b_05 - a_44 * b_01) * det,
            (a_41 * b_04 - a_42 * b_02 + a_44 * b_00) * det);
}

// MARK: - Binary operator methods - new instance = this instance (+) input
template <typename T>
Matrix<T, 4, 4> Matrix<T, 4, 4>::add(T s) const {
    return Matrix(elements_[0] + s, elements_[1] + s, elements_[2] + s, elements_[3] + s, elements_[4] + s,
                  elements_[5] + s, elements_[6] + s, elements_[7] + s, elements_[8] + s, elements_[9] + s,
                  elements_[10] + s, elements_[11] + s, elements_[12] + s, elements_[13] + s, elements_[14] + s,
                  elements_[15] + s);
}

template <typename T>
Matrix<T, 4, 4> Matrix<T, 4, 4>::add(const Matrix &m) const {
    return Matrix(elements_[0] + m.elements_[0], elements_[1] + m.elements_[1], elements_[2] + m.elements_[2],
                  elements_[3] + m.elements_[3], elements_[4] + m.elements_[4], elements_[5] + m.elements_[5],
                  elements_[6] + m.elements_[6], elements_[7] + m.elements_[7], elements_[8] + m.elements_[8],
                  elements_[9] + m.elements_[9], elements_[10] + m.elements_[10], elements_[11] + m.elements_[11],
                  elements_[12] + m.elements_[12], elements_[13] + m.elements_[13], elements_[14] + m.elements_[14],
                  elements_[15] + m.elements_[15]);
}

template <typename T>
Matrix<T, 4, 4> Matrix<T, 4, 4>::sub(T s) const {
    return Matrix(elements_[0] - s, elements_[1] - s, elements_[2] - s, elements_[3] - s, elements_[4] - s,
                  elements_[5] - s, elements_[6] - s, elements_[7] - s, elements_[8] - s, elements_[9] - s,
                  elements_[10] - s, elements_[11] - s, elements_[12] - s, elements_[13] - s, elements_[14] - s,
                  elements_[15] - s);
}

template <typename T>
Matrix<T, 4, 4> Matrix<T, 4, 4>::sub(const Matrix &m) const {
    return Matrix(elements_[0] - m.elements_[0], elements_[1] - m.elements_[1], elements_[2] - m.elements_[2],
                  elements_[3] - m.elements_[3], elements_[4] - m.elements_[4], elements_[5] - m.elements_[5],
                  elements_[6] - m.elements_[6], elements_[7] - m.elements_[7], elements_[8] - m.elements_[8],
                  elements_[9] - m.elements_[9], elements_[10] - m.elements_[10], elements_[11] - m.elements_[11],
                  elements_[12] - m.elements_[12], elements_[13] - m.elements_[13], elements_[14] - m.elements_[14],
                  elements_[15] - m.elements_[15]);
}

template <typename T>
Matrix<T, 4, 4> Matrix<T, 4, 4>::mul(T s) const {
    return Matrix(elements_[0] * s, elements_[1] * s, elements_[2] * s, elements_[3] * s, elements_[4] * s,
                  elements_[5] * s, elements_[6] * s, elements_[7] * s, elements_[8] * s, elements_[9] * s,
                  elements_[10] * s, elements_[11] * s, elements_[12] * s, elements_[13] * s, elements_[14] * s,
                  elements_[15] * s);
}

template <typename T>
Vector<T, 4> Matrix<T, 4, 4>::mul(const Vector<T, 4> &v) const {
    return Vector<T, 4>(elements_[0] * v.x + elements_[4] * v.y + elements_[8] * v.z + elements_[12] * v.w,
                        elements_[1] * v.x + elements_[5] * v.y + elements_[9] * v.z + elements_[13] * v.w,
                        elements_[2] * v.x + elements_[6] * v.y + elements_[10] * v.z + elements_[14] * v.w,
                        elements_[3] * v.x + elements_[7] * v.y + elements_[11] * v.z + elements_[15] * v.w);
}

template <typename T>
Vector<T, 3> Matrix<T, 4, 4>::mul(const Vector<T, 3> &v) const {
    return Vector<T, 3>(elements_[0] * v.x + elements_[4] * v.y + elements_[8] * v.z,
                        elements_[1] * v.x + elements_[5] * v.y + elements_[9] * v.z,
                        elements_[2] * v.x + elements_[6] * v.y + elements_[10] * v.z);
}

template <typename T>
Point3<T> Matrix<T, 4, 4>::mul(const Point3<T> &v) const {
    T w = elements_[3] * v.x + elements_[7] * v.y + elements_[11] * v.z + elements_[15];
    w = (T)1 / w;

    return Point3<T>((elements_[0] * v.x + elements_[4] * v.y + elements_[8] * v.z + elements_[12]) * w,
                     (elements_[1] * v.x + elements_[5] * v.y + elements_[9] * v.z + elements_[13]) * w,
                     (elements_[2] * v.x + elements_[6] * v.y + elements_[10] * v.z + elements_[14]) * w);
}

template <typename T>
Matrix<T, 4, 4> Matrix<T, 4, 4>::mul(const Matrix &m) const {
    return Matrix(elements_[0] * m.elements_[0] + elements_[4] * m.elements_[1] + elements_[8] * m.elements_[2] +
                          elements_[12] * m.elements_[3],
                  elements_[1] * m.elements_[0] + elements_[5] * m.elements_[1] + elements_[9] * m.elements_[2] +
                          elements_[13] * m.elements_[3],
                  elements_[2] * m.elements_[0] + elements_[6] * m.elements_[1] + elements_[10] * m.elements_[2] +
                          elements_[14] * m.elements_[3],
                  elements_[3] * m.elements_[0] + elements_[7] * m.elements_[1] + elements_[11] * m.elements_[2] +
                          elements_[15] * m.elements_[3],

                  elements_[0] * m.elements_[4] + elements_[4] * m.elements_[5] + elements_[8] * m.elements_[6] +
                          elements_[12] * m.elements_[7],
                  elements_[1] * m.elements_[4] + elements_[5] * m.elements_[5] + elements_[9] * m.elements_[6] +
                          elements_[13] * m.elements_[7],
                  elements_[2] * m.elements_[4] + elements_[6] * m.elements_[5] + elements_[10] * m.elements_[6] +
                          elements_[14] * m.elements_[7],
                  elements_[3] * m.elements_[4] + elements_[7] * m.elements_[5] + elements_[11] * m.elements_[6] +
                          elements_[15] * m.elements_[7],

                  elements_[0] * m.elements_[8] + elements_[4] * m.elements_[9] + elements_[8] * m.elements_[10] +
                          elements_[12] * m.elements_[11],
                  elements_[1] * m.elements_[8] + elements_[5] * m.elements_[9] + elements_[9] * m.elements_[10] +
                          elements_[13] * m.elements_[11],
                  elements_[2] * m.elements_[8] + elements_[6] * m.elements_[9] + elements_[10] * m.elements_[10] +
                          elements_[14] * m.elements_[11],
                  elements_[3] * m.elements_[8] + elements_[7] * m.elements_[9] + elements_[11] * m.elements_[10] +
                          elements_[15] * m.elements_[11],

                  elements_[0] * m.elements_[12] + elements_[4] * m.elements_[13] + elements_[8] * m.elements_[14] +
                          elements_[12] * m.elements_[15],
                  elements_[1] * m.elements_[12] + elements_[5] * m.elements_[13] + elements_[9] * m.elements_[14] +
                          elements_[13] * m.elements_[15],
                  elements_[2] * m.elements_[12] + elements_[6] * m.elements_[13] + elements_[10] * m.elements_[14] +
                          elements_[14] * m.elements_[15],
                  elements_[3] * m.elements_[12] + elements_[7] * m.elements_[13] + elements_[11] * m.elements_[14] +
                          elements_[15] * m.elements_[15]);
}

template <typename T>
Matrix<T, 4, 4> Matrix<T, 4, 4>::div(T s) const {
    return Matrix(elements_[0] / s, elements_[1] / s, elements_[2] / s, elements_[3] / s, elements_[4] / s,
                  elements_[5] / s, elements_[6] / s, elements_[7] / s, elements_[8] / s, elements_[9] / s,
                  elements_[10] / s, elements_[11] / s, elements_[12] / s, elements_[13] / s, elements_[14] / s,
                  elements_[15] / s);
}

// MARK: - Binary operator methods - new instance = input (+) this instance
template <typename T>
Matrix<T, 4, 4> Matrix<T, 4, 4>::radd(T s) const {
    return Matrix(s + elements_[0], s + elements_[1], s + elements_[2], s + elements_[3], s + elements_[4],
                  s + elements_[5], s + elements_[6], s + elements_[7], s + elements_[8], s + elements_[9],
                  s + elements_[10], s + elements_[11], s + elements_[12], s + elements_[13], s + elements_[14],
                  s + elements_[15]);
}

template <typename T>
Matrix<T, 4, 4> Matrix<T, 4, 4>::radd(const Matrix &m) const {
    return Matrix(m.elements_[0] + elements_[0], m.elements_[1] + elements_[1], m.elements_[2] + elements_[2],
                  m.elements_[3] + elements_[3], m.elements_[4] + elements_[4], m.elements_[5] + elements_[5],
                  m.elements_[6] + elements_[6], m.elements_[7] + elements_[7], m.elements_[8] + elements_[8],
                  m.elements_[9] + elements_[9], m.elements_[10] + elements_[10], m.elements_[11] + elements_[11],
                  m.elements_[12] + elements_[12], m.elements_[13] + elements_[13], m.elements_[14] + elements_[14],
                  m.elements_[15] + elements_[15]);
}

template <typename T>
Matrix<T, 4, 4> Matrix<T, 4, 4>::rsub(T s) const {
    return Matrix(s - elements_[0], s - elements_[1], s - elements_[2], s - elements_[3], s - elements_[4],
                  s - elements_[5], s - elements_[6], s - elements_[7], s - elements_[8], s - elements_[9],
                  s - elements_[10], s - elements_[11], s - elements_[12], s - elements_[13], s - elements_[14],
                  s - elements_[15]);
}

template <typename T>
Matrix<T, 4, 4> Matrix<T, 4, 4>::rsub(const Matrix &m) const {
    return Matrix(m.elements_[0] - elements_[0], m.elements_[1] - elements_[1], m.elements_[2] - elements_[2],
                  m.elements_[3] - elements_[3], m.elements_[4] - elements_[4], m.elements_[5] - elements_[5],
                  m.elements_[6] - elements_[6], m.elements_[7] - elements_[7], m.elements_[8] - elements_[8],
                  m.elements_[9] - elements_[9], m.elements_[10] - elements_[10], m.elements_[11] - elements_[11],
                  m.elements_[12] - elements_[12], m.elements_[13] - elements_[13], m.elements_[14] - elements_[14],
                  m.elements_[15] - elements_[15]);
}

template <typename T>
Matrix<T, 4, 4> Matrix<T, 4, 4>::rmul(T s) const {
    return Matrix(s * elements_[0], s * elements_[1], s * elements_[2], s * elements_[3], s * elements_[4],
                  s * elements_[5], s * elements_[6], s * elements_[7], s * elements_[8], s * elements_[9],
                  s * elements_[10], s * elements_[11], s * elements_[12], s * elements_[13], s * elements_[14],
                  s * elements_[15]);
}

template <typename T>
Matrix<T, 4, 4> Matrix<T, 4, 4>::rmul(const Matrix &m) const {
    return m.mul(*this);
}

template <typename T>
Matrix<T, 4, 4> Matrix<T, 4, 4>::rdiv(T s) const {
    return Matrix(s / elements_[0], s / elements_[1], s / elements_[2], s / elements_[3], s / elements_[4],
                  s / elements_[5], s / elements_[6], s / elements_[7], s / elements_[8], s / elements_[9],
                  s / elements_[10], s / elements_[11], s / elements_[12], s / elements_[13], s / elements_[14],
                  s / elements_[15]);
}

// MARK: - Augmented operator methods - this instance (+)= input
template <typename T>
void Matrix<T, 4, 4>::iadd(T s) {
    elements_[0] += s;
    elements_[1] += s;
    elements_[2] += s;
    elements_[3] += s;
    elements_[4] += s;
    elements_[5] += s;
    elements_[6] += s;
    elements_[7] += s;
    elements_[8] += s;
    elements_[9] += s;
    elements_[10] += s;
    elements_[11] += s;
    elements_[12] += s;
    elements_[13] += s;
    elements_[14] += s;
    elements_[15] += s;
}

template <typename T>
void Matrix<T, 4, 4>::iadd(const Matrix &m) {
    elements_[0] += m.elements_[0];
    elements_[1] += m.elements_[1];
    elements_[2] += m.elements_[2];
    elements_[3] += m.elements_[3];
    elements_[4] += m.elements_[4];
    elements_[5] += m.elements_[5];
    elements_[6] += m.elements_[6];
    elements_[7] += m.elements_[7];
    elements_[8] += m.elements_[8];
    elements_[9] += m.elements_[9];
    elements_[10] += m.elements_[10];
    elements_[11] += m.elements_[11];
    elements_[12] += m.elements_[12];
    elements_[13] += m.elements_[13];
    elements_[14] += m.elements_[14];
    elements_[15] += m.elements_[15];
}

template <typename T>
void Matrix<T, 4, 4>::isub(T s) {
    elements_[0] -= s;
    elements_[1] -= s;
    elements_[2] -= s;
    elements_[3] -= s;
    elements_[4] -= s;
    elements_[5] -= s;
    elements_[6] -= s;
    elements_[7] -= s;
    elements_[8] -= s;
    elements_[9] -= s;
    elements_[10] -= s;
    elements_[11] -= s;
    elements_[12] -= s;
    elements_[13] -= s;
    elements_[14] -= s;
    elements_[15] -= s;
}

template <typename T>
void Matrix<T, 4, 4>::isub(const Matrix &m) {
    elements_[0] -= m.elements_[0];
    elements_[1] -= m.elements_[1];
    elements_[2] -= m.elements_[2];
    elements_[3] -= m.elements_[3];
    elements_[4] -= m.elements_[4];
    elements_[5] -= m.elements_[5];
    elements_[6] -= m.elements_[6];
    elements_[7] -= m.elements_[7];
    elements_[8] -= m.elements_[8];
    elements_[9] -= m.elements_[9];
    elements_[10] -= m.elements_[10];
    elements_[11] -= m.elements_[11];
    elements_[12] -= m.elements_[12];
    elements_[13] -= m.elements_[13];
    elements_[14] -= m.elements_[14];
    elements_[15] -= m.elements_[15];
}

template <typename T>
void Matrix<T, 4, 4>::imul(T s) {
    elements_[0] *= s;
    elements_[1] *= s;
    elements_[2] *= s;
    elements_[3] *= s;
    elements_[4] *= s;
    elements_[5] *= s;
    elements_[6] *= s;
    elements_[7] *= s;
    elements_[8] *= s;
    elements_[9] *= s;
    elements_[10] *= s;
    elements_[11] *= s;
    elements_[12] *= s;
    elements_[13] *= s;
    elements_[14] *= s;
    elements_[15] *= s;
}

template <typename T>
void Matrix<T, 4, 4>::imul(const Matrix &m) {
    set(mul(m));
}

template <typename T>
void Matrix<T, 4, 4>::idiv(T s) {
    elements_[0] /= s;
    elements_[1] /= s;
    elements_[2] /= s;
    elements_[3] /= s;
    elements_[4] /= s;
    elements_[5] /= s;
    elements_[6] /= s;
    elements_[7] /= s;
    elements_[8] /= s;
    elements_[9] /= s;
    elements_[10] /= s;
    elements_[11] /= s;
    elements_[12] /= s;
    elements_[13] /= s;
    elements_[14] /= s;
    elements_[15] /= s;
}

// MARK: - Modifiers
template <typename T>
void Matrix<T, 4, 4>::transpose() {
    std::swap(elements_[1], elements_[4]);
    std::swap(elements_[2], elements_[8]);
    std::swap(elements_[3], elements_[12]);
    std::swap(elements_[6], elements_[9]);
    std::swap(elements_[7], elements_[13]);
    std::swap(elements_[11], elements_[14]);
}

template <typename T>
void Matrix<T, 4, 4>::invert() {
    T d = determinant();
    Matrix m;
    m.elements_[0] = elements_[5] * elements_[10] * elements_[15] + elements_[6] * elements_[11] * elements_[13] +
                     elements_[7] * elements_[9] * elements_[14] - elements_[5] * elements_[11] * elements_[14] -
                     elements_[6] * elements_[9] * elements_[15] - elements_[7] * elements_[10] * elements_[13];
    m.elements_[1] = elements_[1] * elements_[11] * elements_[14] + elements_[2] * elements_[9] * elements_[15] +
                     elements_[3] * elements_[10] * elements_[13] - elements_[1] * elements_[10] * elements_[15] -
                     elements_[2] * elements_[11] * elements_[13] - elements_[3] * elements_[9] * elements_[14];
    m.elements_[2] = elements_[1] * elements_[6] * elements_[15] + elements_[2] * elements_[7] * elements_[13] +
                     elements_[3] * elements_[5] * elements_[14] - elements_[1] * elements_[7] * elements_[14] -
                     elements_[2] * elements_[5] * elements_[15] - elements_[3] * elements_[6] * elements_[13];
    m.elements_[3] = elements_[1] * elements_[7] * elements_[10] + elements_[2] * elements_[5] * elements_[11] +
                     elements_[3] * elements_[6] * elements_[9] - elements_[1] * elements_[6] * elements_[11] -
                     elements_[2] * elements_[7] * elements_[9] - elements_[3] * elements_[5] * elements_[10];
    m.elements_[4] = elements_[4] * elements_[11] * elements_[14] + elements_[6] * elements_[8] * elements_[15] +
                     elements_[7] * elements_[10] * elements_[12] - elements_[4] * elements_[10] * elements_[15] -
                     elements_[6] * elements_[11] * elements_[12] - elements_[7] * elements_[8] * elements_[14];
    m.elements_[5] = elements_[0] * elements_[10] * elements_[15] + elements_[2] * elements_[11] * elements_[12] +
                     elements_[3] * elements_[8] * elements_[14] - elements_[0] * elements_[11] * elements_[14] -
                     elements_[2] * elements_[8] * elements_[15] - elements_[3] * elements_[10] * elements_[12];
    m.elements_[6] = elements_[0] * elements_[7] * elements_[14] + elements_[2] * elements_[4] * elements_[15] +
                     elements_[3] * elements_[6] * elements_[12] - elements_[0] * elements_[6] * elements_[15] -
                     elements_[2] * elements_[7] * elements_[12] - elements_[3] * elements_[4] * elements_[14];
    m.elements_[7] = elements_[0] * elements_[6] * elements_[11] + elements_[2] * elements_[7] * elements_[8] +
                     elements_[3] * elements_[4] * elements_[10] - elements_[0] * elements_[7] * elements_[10] -
                     elements_[2] * elements_[4] * elements_[11] - elements_[3] * elements_[6] * elements_[8];
    m.elements_[8] = elements_[4] * elements_[9] * elements_[15] + elements_[5] * elements_[11] * elements_[12] +
                     elements_[7] * elements_[8] * elements_[13] - elements_[4] * elements_[11] * elements_[13] -
                     elements_[5] * elements_[8] * elements_[15] - elements_[7] * elements_[9] * elements_[12];
    m.elements_[9] = elements_[0] * elements_[11] * elements_[13] + elements_[1] * elements_[8] * elements_[15] +
                     elements_[3] * elements_[9] * elements_[12] - elements_[0] * elements_[9] * elements_[15] -
                     elements_[1] * elements_[11] * elements_[12] - elements_[3] * elements_[8] * elements_[13];
    m.elements_[10] = elements_[0] * elements_[5] * elements_[15] + elements_[1] * elements_[7] * elements_[12] +
                      elements_[3] * elements_[4] * elements_[13] - elements_[0] * elements_[7] * elements_[13] -
                      elements_[1] * elements_[4] * elements_[15] - elements_[3] * elements_[5] * elements_[12];
    m.elements_[11] = elements_[0] * elements_[7] * elements_[9] + elements_[1] * elements_[4] * elements_[11] +
                      elements_[3] * elements_[5] * elements_[8] - elements_[0] * elements_[5] * elements_[11] -
                      elements_[1] * elements_[7] * elements_[8] - elements_[3] * elements_[4] * elements_[9];
    m.elements_[12] = elements_[4] * elements_[10] * elements_[13] + elements_[5] * elements_[8] * elements_[14] +
                      elements_[6] * elements_[9] * elements_[12] - elements_[4] * elements_[9] * elements_[14] -
                      elements_[5] * elements_[10] * elements_[12] - elements_[6] * elements_[8] * elements_[13];
    m.elements_[13] = elements_[0] * elements_[9] * elements_[14] + elements_[1] * elements_[10] * elements_[12] +
                      elements_[2] * elements_[8] * elements_[13] - elements_[0] * elements_[10] * elements_[13] -
                      elements_[1] * elements_[8] * elements_[14] - elements_[2] * elements_[9] * elements_[12];
    m.elements_[14] = elements_[0] * elements_[6] * elements_[13] + elements_[1] * elements_[4] * elements_[14] +
                      elements_[2] * elements_[5] * elements_[12] - elements_[0] * elements_[5] * elements_[14] -
                      elements_[1] * elements_[6] * elements_[12] - elements_[2] * elements_[4] * elements_[13];
    m.elements_[15] = elements_[0] * elements_[5] * elements_[10] + elements_[1] * elements_[6] * elements_[8] +
                      elements_[2] * elements_[4] * elements_[9] - elements_[0] * elements_[6] * elements_[9] -
                      elements_[1] * elements_[4] * elements_[10] - elements_[2] * elements_[5] * elements_[8];
    m.idiv(d);

    set(m);
}

// MARK: - Complex getters
template <typename T>
T Matrix<T, 4, 4>::sum() const {
    T s = 0;
    for (int i = 0; i < 16; ++i) {
        s += elements_[i];
    }
    return s;
}

template <typename T>
T Matrix<T, 4, 4>::avg() const {
    return sum() / 16;
}

template <typename T>
T Matrix<T, 4, 4>::min() const {
    return minn(data(), 16);
}

template <typename T>
T Matrix<T, 4, 4>::max() const {
    return maxn(data(), 16);
}

template <typename T>
T Matrix<T, 4, 4>::absmin() const {
    return absminn(data(), 16);
}

template <typename T>
T Matrix<T, 4, 4>::absmax() const {
    return absmaxn(data(), 16);
}

template <typename T>
T Matrix<T, 4, 4>::trace() const {
    return elements_[0] + elements_[5] + elements_[10] + elements_[15];
}

template <typename T>
T Matrix<T, 4, 4>::determinant() const {
    return elements_[0] * elements_[5] * elements_[10] * elements_[15] +
           elements_[0] * elements_[6] * elements_[11] * elements_[13] +
           elements_[0] * elements_[7] * elements_[9] * elements_[14] +
           elements_[1] * elements_[4] * elements_[11] * elements_[14] +
           elements_[1] * elements_[6] * elements_[8] * elements_[15] +
           elements_[1] * elements_[7] * elements_[10] * elements_[12] +
           elements_[2] * elements_[4] * elements_[9] * elements_[15] +
           elements_[2] * elements_[5] * elements_[11] * elements_[12] +
           elements_[2] * elements_[7] * elements_[8] * elements_[13] +
           elements_[3] * elements_[4] * elements_[10] * elements_[13] +
           elements_[3] * elements_[5] * elements_[8] * elements_[14] +
           elements_[3] * elements_[6] * elements_[9] * elements_[12] -
           elements_[0] * elements_[5] * elements_[11] * elements_[14] -
           elements_[0] * elements_[6] * elements_[9] * elements_[15] -
           elements_[0] * elements_[7] * elements_[10] * elements_[13] -
           elements_[1] * elements_[4] * elements_[10] * elements_[15] -
           elements_[1] * elements_[6] * elements_[11] * elements_[12] -
           elements_[1] * elements_[7] * elements_[8] * elements_[14] -
           elements_[2] * elements_[4] * elements_[11] * elements_[13] -
           elements_[2] * elements_[5] * elements_[8] * elements_[15] -
           elements_[2] * elements_[7] * elements_[9] * elements_[12] -
           elements_[3] * elements_[4] * elements_[9] * elements_[14] -
           elements_[3] * elements_[5] * elements_[10] * elements_[12] -
           elements_[3] * elements_[6] * elements_[8] * elements_[13];
}

template <typename T>
Matrix<T, 4, 4> Matrix<T, 4, 4>::diagonal() const {
    return Matrix(elements_[0], 0, 0, 0, 0, elements_[5], 0, 0, 0, 0, elements_[10], 0, 0, 0, 0, elements_[15]);
}

template <typename T>
Matrix<T, 4, 4> Matrix<T, 4, 4>::offDiagonal() const {
    return Matrix(0, elements_[1], elements_[2], elements_[3], elements_[4], 0, elements_[6], elements_[7],
                  elements_[8], elements_[9], 0, elements_[11], elements_[12], elements_[13], elements_[14], 0);
}

template <typename T>
Matrix<T, 4, 4> Matrix<T, 4, 4>::strictLowerTri() const {
    return Matrix(0, elements_[1], elements_[2], elements_[3], 0, 0, elements_[6], elements_[7], 0, 0, 0, elements_[11],
                  0, 0, 0, 0);
}

template <typename T>
Matrix<T, 4, 4> Matrix<T, 4, 4>::strictUpperTri() const {
    return Matrix(0, 0, 0, 0, elements_[4], 0, 0, 0, elements_[8], elements_[9], 0, 0, elements_[12], elements_[13],
                  elements_[14], 0);
}

template <typename T>
Matrix<T, 4, 4> Matrix<T, 4, 4>::lowerTri() const {
    return Matrix(elements_[0], elements_[1], elements_[2], elements_[3], 0, elements_[5], elements_[6], elements_[7],
                  0, 0, elements_[10], elements_[11], 0, 0, 0, elements_[15]);
}

template <typename T>
Matrix<T, 4, 4> Matrix<T, 4, 4>::upperTri() const {
    return Matrix(elements_[0], 0, 0, 0, elements_[4], elements_[5], 0, 0, elements_[8], elements_[9], elements_[10], 0,
                  elements_[12], elements_[13], elements_[14], elements_[15]);
}

template <typename T>
Matrix<T, 4, 4> Matrix<T, 4, 4>::transposed() const {
    return Matrix(elements_[0], elements_[4], elements_[8], elements_[12], elements_[1], elements_[5], elements_[9],
                  elements_[13], elements_[2], elements_[6], elements_[10], elements_[14], elements_[3], elements_[7],
                  elements_[11], elements_[15]);
}

template <typename T>
Matrix<T, 4, 4> Matrix<T, 4, 4>::inverse() const {
    Matrix m(*this);
    m.invert();
    return m;
}

template <typename T>
Matrix<T, 4, 4> Matrix<T, 4, 4>::rotateAxisAngle(const Vector3<T> &axis, T r) const {
    T x = axis.x;
    T y = axis.y;
    T z = axis.z;
    T len = std::sqrt(x * x + y * y + z * z);

    if (std::abs(len) < std::numeric_limits<T>::epsilon()) {
        return Matrix<T, 4, 4>::makeIdentity();
    }

    len = 1 / len;
    x *= len;
    y *= len;
    z *= len;

    T s = std::sin(r);
    T c = std::cos(r);
    T t = 1 - c;

    T a_11 = elements_[0], a_12 = elements_[1], a_13 = elements_[2], a_14 = elements_[3];
    T a_21 = elements_[4], a_22 = elements_[5], a_23 = elements_[6], a_24 = elements_[7];
    T a_31 = elements_[8], a_32 = elements_[9], a_33 = elements_[10], a_34 = elements_[11];

    // Construct the elements of the rotation matrix
    T b_11 = x * x * t + c;
    T b_12 = y * x * t + z * s;
    T b_13 = z * x * t - y * s;
    T b_21 = x * y * t - z * s;
    T b_22 = y * y * t + c;
    T b_23 = z * y * t + x * s;
    T b_31 = x * z * t + y * s;
    T b_32 = y * z * t - x * s;
    T b_33 = z * z * t + c;

    // Perform rotation-specific matrix multiplication
    return Matrix<T, 4, 4>(a_11 * b_11 + a_21 * b_12 + a_31 * b_13, a_12 * b_11 + a_22 * b_12 + a_32 * b_13,
                           a_13 * b_11 + a_23 * b_12 + a_33 * b_13, a_14 * b_11 + a_24 * b_12 + a_34 * b_13,

                           a_11 * b_21 + a_21 * b_22 + a_31 * b_23, a_12 * b_21 + a_22 * b_22 + a_32 * b_23,
                           a_13 * b_21 + a_23 * b_22 + a_33 * b_23, a_14 * b_21 + a_24 * b_22 + a_34 * b_23,

                           a_11 * b_31 + a_21 * b_32 + a_31 * b_33, a_12 * b_31 + a_22 * b_32 + a_32 * b_33,
                           a_13 * b_31 + a_23 * b_32 + a_33 * b_33, a_14 * b_31 + a_24 * b_32 + a_34 * b_33,

                           elements_[12], elements_[13], elements_[14], elements_[15]);
}

template <typename T>
void Matrix<T, 4, 4>::rotateAxisAngle(const Vector3<T> &axis, T r) {
    T x = axis.x;
    T y = axis.y;
    T z = axis.z;
    T len = std::sqrt(x * x + y * y + z * z);

    if (std::abs(len) < std::numeric_limits<T>::epsilon()) {
        return;
    }

    len = 1 / len;
    x *= len;
    y *= len;
    z *= len;

    T s = std::sin(r);
    T c = std::cos(r);
    T t = 1 - c;

    T a_11 = elements_[0], a_12 = elements_[1], a_13 = elements_[2], a_14 = elements_[3];
    T a_21 = elements_[4], a_22 = elements_[5], a_23 = elements_[6], a_24 = elements_[7];
    T a_31 = elements_[8], a_32 = elements_[9], a_33 = elements_[10], a_34 = elements_[11];

    // Construct the elements of the rotation matrix
    T b_11 = x * x * t + c;
    T b_12 = y * x * t + z * s;
    T b_13 = z * x * t - y * s;
    T b_21 = x * y * t - z * s;
    T b_22 = y * y * t + c;
    T b_23 = z * y * t + x * s;
    T b_31 = x * z * t + y * s;
    T b_32 = y * z * t - x * s;
    T b_33 = z * z * t + c;

    // Perform rotation-specific matrix multiplication
    elements_[0] = a_11 * b_11 + a_21 * b_12 + a_31 * b_13;
    elements_[1] = a_12 * b_11 + a_22 * b_12 + a_32 * b_13;
    elements_[2] = a_13 * b_11 + a_23 * b_12 + a_33 * b_13;
    elements_[3] = a_14 * b_11 + a_24 * b_12 + a_34 * b_13;

    elements_[4] = a_11 * b_21 + a_21 * b_22 + a_31 * b_23;
    elements_[5] = a_12 * b_21 + a_22 * b_22 + a_32 * b_23;
    elements_[6] = a_13 * b_21 + a_23 * b_22 + a_33 * b_23;
    elements_[7] = a_14 * b_21 + a_24 * b_22 + a_34 * b_23;

    elements_[8] = a_11 * b_31 + a_21 * b_32 + a_31 * b_33;
    elements_[9] = a_12 * b_31 + a_22 * b_32 + a_32 * b_33;
    elements_[10] = a_13 * b_31 + a_23 * b_32 + a_33 * b_33;
    elements_[11] = a_14 * b_31 + a_24 * b_32 + a_34 * b_33;
}

template <typename T>
Matrix<T, 4, 4> Matrix<T, 4, 4>::scale(const Vector3<T> &s) const {
    T x = s.x;
    T y = s.y;
    T z = s.z;

    return Matrix<T, 4, 4>(elements_[0] * x, elements_[1] * x, elements_[2] * x, elements_[3] * x,

                           elements_[4] * y, elements_[5] * y, elements_[6] * y, elements_[7] * y,

                           elements_[8] * z, elements_[9] * z, elements_[10] * z, elements_[11] * z,

                           elements_[12], elements_[13], elements_[14], elements_[15]);
}

template <typename T>
void Matrix<T, 4, 4>::scale(const Vector3<T> &s) {
    T x = s.x;
    T y = s.y;
    T z = s.z;

    elements_[0] *= x;
    elements_[1] *= x;
    elements_[2] *= x;
    elements_[3] *= x;

    elements_[4] *= y;
    elements_[5] *= y;
    elements_[6] *= y;
    elements_[7] *= y;

    elements_[8] *= z;
    elements_[9] *= z;
    elements_[10] *= z;
    elements_[11] *= z;
}

template <typename T>
Matrix<T, 4, 4> Matrix<T, 4, 4>::translate(const Vector3<T> &v) const {
    T x = v.x;
    T y = v.y;
    T z = v.z;

    T a_11 = elements_[0], a_12 = elements_[1], a_13 = elements_[2], a_14 = elements_[3];
    T a_21 = elements_[4], a_22 = elements_[5], a_23 = elements_[6], a_24 = elements_[7];
    T a_31 = elements_[8], a_32 = elements_[9], a_33 = elements_[10], a_34 = elements_[11];

    return Matrix<T, 4, 4>(
            a_11, a_12, a_13, a_14,

            a_21, a_22, a_23, a_24,

            a_31, a_32, a_33, a_34,

            a_11 * x + a_21 * y + a_31 * z + elements_[12], a_12 * x + a_22 * y + a_32 * z + elements_[13],
            a_13 * x + a_23 * y + a_33 * z + elements_[14], a_14 * x + a_24 * y + a_34 * z + elements_[15]);
}

template <typename T>
void Matrix<T, 4, 4>::translate(const Vector3<T> &v) {
    T x = v.x;
    T y = v.y;
    T z = v.z;

    elements_[12] = elements_[0] * x + elements_[4] * y + elements_[8] * z + elements_[12];
    elements_[13] = elements_[1] * x + elements_[5] * y + elements_[9] * z + elements_[13];
    elements_[14] = elements_[2] * x + elements_[6] * y + elements_[10] * z + elements_[14];
    elements_[15] = elements_[3] * x + elements_[7] * y + elements_[11] * z + elements_[15];
}

template <typename T>
template <typename U>
Matrix<U, 4, 4> Matrix<T, 4, 4>::castTo() const {
    return Matrix<U, 4, 4>(static_cast<U>(elements_[0]), static_cast<U>(elements_[1]), static_cast<U>(elements_[2]),
                           static_cast<U>(elements_[3]), static_cast<U>(elements_[4]), static_cast<U>(elements_[5]),
                           static_cast<U>(elements_[6]), static_cast<U>(elements_[7]), static_cast<U>(elements_[8]),
                           static_cast<U>(elements_[9]), static_cast<U>(elements_[10]), static_cast<U>(elements_[11]),
                           static_cast<U>(elements_[12]), static_cast<U>(elements_[13]), static_cast<U>(elements_[14]),
                           static_cast<U>(elements_[15]));
}

// MARK: - Setter operators
template <typename T>
Matrix<T, 4, 4> &Matrix<T, 4, 4>::operator=(const Matrix &m) {
    set(m);
    return *this;
}

template <typename T>
Matrix<T, 4, 4> &Matrix<T, 4, 4>::operator+=(T s) {
    iadd(s);
    return *this;
}

template <typename T>
Matrix<T, 4, 4> &Matrix<T, 4, 4>::operator+=(const Matrix &m) {
    iadd(m);
    return *this;
}

template <typename T>
Matrix<T, 4, 4> &Matrix<T, 4, 4>::operator-=(T s) {
    isub(s);
    return *this;
}

template <typename T>
Matrix<T, 4, 4> &Matrix<T, 4, 4>::operator-=(const Matrix &m) {
    isub(m);
    return *this;
}

template <typename T>
Matrix<T, 4, 4> &Matrix<T, 4, 4>::operator*=(T s) {
    imul(s);
    return *this;
}

template <typename T>
Matrix<T, 4, 4> &Matrix<T, 4, 4>::operator*=(const Matrix &m) {
    imul(m);
    return *this;
}

template <typename T>
Matrix<T, 4, 4> &Matrix<T, 4, 4>::operator/=(T s) {
    idiv(s);
    return *this;
}

template <typename T>
bool Matrix<T, 4, 4>::operator==(const Matrix &m) const {
    return elements_[0] == m.elements_[0] && elements_[1] == m.elements_[1] && elements_[2] == m.elements_[2] &&
           elements_[3] == m.elements_[3] && elements_[4] == m.elements_[4] && elements_[5] == m.elements_[5] &&
           elements_[6] == m.elements_[6] && elements_[7] == m.elements_[7] && elements_[8] == m.elements_[8] &&
           elements_[9] == m.elements_[9] && elements_[10] == m.elements_[10] && elements_[11] == m.elements_[11] &&
           elements_[12] == m.elements_[12] && elements_[13] == m.elements_[13] && elements_[14] == m.elements_[14] &&
           elements_[15] == m.elements_[15];
}

template <typename T>
bool Matrix<T, 4, 4>::operator!=(const Matrix &m) const {
    return elements_[0] != m.elements_[0] || elements_[1] != m.elements_[1] || elements_[2] != m.elements_[2] ||
           elements_[3] != m.elements_[3] || elements_[4] != m.elements_[4] || elements_[5] != m.elements_[5] ||
           elements_[6] != m.elements_[6] || elements_[7] != m.elements_[7] || elements_[8] != m.elements_[8] ||
           elements_[9] != m.elements_[9] || elements_[10] != m.elements_[10] || elements_[11] != m.elements_[11] ||
           elements_[12] != m.elements_[12] || elements_[13] != m.elements_[13] || elements_[14] != m.elements_[14] ||
           elements_[15] != m.elements_[15];
}

// MARK: - Getter operators
template <typename T>
T &Matrix<T, 4, 4>::operator[](size_t i) {
    return elements_[i];
}

template <typename T>
const T &Matrix<T, 4, 4>::operator[](size_t i) const {
    return elements_[i];
}

template <typename T>
T &Matrix<T, 4, 4>::operator()(size_t i, size_t j) {
    return elements_[i + 4 * j];
}

template <typename T>
const T &Matrix<T, 4, 4>::operator()(size_t i, size_t j) const {
    return elements_[i + 4 * j];
}

// MARK: - Helpers
template <typename T>
Matrix<T, 4, 4> Matrix<T, 4, 4>::makeZero() {
    return Matrix(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
}

template <typename T>
Matrix<T, 4, 4> Matrix<T, 4, 4>::makeIdentity() {
    return Matrix(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
}

// MARK: - Operator overloadings
template <typename T>
Matrix<T, 4, 4> operator-(const Matrix<T, 4, 4> &a) {
    return a.mul(-1);
}

template <typename T>
Matrix<T, 4, 4> operator+(const Matrix<T, 4, 4> &a, const Matrix<T, 4, 4> &b) {
    return a.add(b);
}

template <typename T>
Matrix<T, 4, 4> operator+(const Matrix<T, 4, 4> &a, T b) {
    return a.add(b);
}

template <typename T>
Matrix<T, 4, 4> operator+(T a, const Matrix<T, 4, 4> &b) {
    return b.radd(a);
}

template <typename T>
Matrix<T, 4, 4> operator-(const Matrix<T, 4, 4> &a, const Matrix<T, 4, 4> &b) {
    return a.sub(b);
}

template <typename T>
Matrix<T, 4, 4> operator-(const Matrix<T, 4, 4> &a, T b) {
    return a.sub(b);
}

template <typename T>
Matrix<T, 4, 4> operator-(T a, const Matrix<T, 4, 4> &b) {
    return b.rsub(a);
}

template <typename T>
Matrix<T, 4, 4> operator*(const Matrix<T, 4, 4> &a, T b) {
    return a.mul(b);
}

template <typename T>
Matrix<T, 4, 4> operator*(T a, const Matrix<T, 4, 4> &b) {
    return b.rmul(a);
}

template <typename T>
Point3<T> operator*(const Matrix<T, 4, 4> &a, const Point3<T> &b) {
    return a.mul(b);
}

template <typename T>
Vector<T, 3> operator*(const Matrix<T, 4, 4> &a, const Vector<T, 3> &b) {
    return a.mul(b);
}

template <typename T>
Vector<T, 4> operator*(const Matrix<T, 4, 4> &a, const Vector<T, 4> &b) {
    return a.mul(b);
}

template <typename T>
Matrix<T, 4, 4> operator*(const Matrix<T, 4, 4> &a, const Matrix<T, 4, 4> &b) {
    return a.mul(b);
}

template <typename T>
Matrix<T, 4, 4> operator/(const Matrix<T, 4, 4> &a, T b) {
    return a.div(b);
}

template <typename T>
Matrix<T, 4, 4> operator/(T a, const Matrix<T, 4, 4> &b) {
    return b.rdiv(a);
}

}  // namespace vox
