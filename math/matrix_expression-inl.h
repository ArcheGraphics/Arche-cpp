//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "math/matrix_expression.h"

namespace vox {

// MARK: - MatrixExpression

template <typename T, typename E>
Size2 MatrixExpression<T, E>::size() const {
    return static_cast<const E &>(*this).size();
}

template <typename T, typename E>
size_t MatrixExpression<T, E>::rows() const {
    return static_cast<const E &>(*this).rows();
}

template <typename T, typename E>
size_t MatrixExpression<T, E>::cols() const {
    return static_cast<const E &>(*this).cols();
}

template <typename T, typename E>
const E &MatrixExpression<T, E>::operator()() const {
    return static_cast<const E &>(*this);
}

//

template <typename T>
MatrixConstant<T>::MatrixConstant(size_t m, size_t n, const T &c) : m_(m), n_(n), c_(c) {}

template <typename T>
Size2 MatrixConstant<T>::size() const {
    return Size2(rows(), cols());
}

template <typename T>
size_t MatrixConstant<T>::rows() const {
    return m_;
}

template <typename T>
size_t MatrixConstant<T>::cols() const {
    return n_;
}

template <typename T>
T MatrixConstant<T>::operator()(size_t, size_t) const {
    return c_;
}

//

template <typename T>
MatrixIdentity<T>::MatrixIdentity(size_t m) : m_(m) {}

template <typename T>
Size2 MatrixIdentity<T>::size() const {
    return Size2(m_, m_);
}

template <typename T>
size_t MatrixIdentity<T>::rows() const {
    return m_;
}

template <typename T>
size_t MatrixIdentity<T>::cols() const {
    return m_;
}

template <typename T>
T MatrixIdentity<T>::operator()(size_t i, size_t j) const {
    return (i == j) ? 1 : 0;
}

// MARK: - MatrixUnaryOp

template <typename T, typename E, typename Op>
MatrixUnaryOp<T, E, Op>::MatrixUnaryOp(const E &u) : u_(u) {}

template <typename T, typename E, typename Op>
Size2 MatrixUnaryOp<T, E, Op>::size() const {
    return u_.size();
}

template <typename T, typename E, typename Op>
size_t MatrixUnaryOp<T, E, Op>::rows() const {
    return u_.rows();
}

template <typename T, typename E, typename Op>
size_t MatrixUnaryOp<T, E, Op>::cols() const {
    return u_.cols();
}

template <typename T, typename E, typename Op>
T MatrixUnaryOp<T, E, Op>::operator()(size_t i, size_t j) const {
    return op_(u_(i, j));
}

//

template <typename T, typename E>
MatrixDiagonal<T, E>::MatrixDiagonal(const E &u, bool is_diag) : u_(u), is_diag_(is_diag) {}

template <typename T, typename E>
Size2 MatrixDiagonal<T, E>::size() const {
    return u_.size();
}

template <typename T, typename E>
size_t MatrixDiagonal<T, E>::rows() const {
    return u_.rows();
}

template <typename T, typename E>
size_t MatrixDiagonal<T, E>::cols() const {
    return u_.cols();
}

template <typename T, typename E>
T MatrixDiagonal<T, E>::operator()(size_t i, size_t j) const {
    if (is_diag_) {
        return (i == j) ? u_(i, j) : 0;
    } else {
        return (i != j) ? u_(i, j) : 0;
    }
}

//

template <typename T, typename E>
MatrixTriangular<T, E>::MatrixTriangular(const E &u, bool is_upper, bool is_strict)
    : u_(u), is_upper_(is_upper), is_strict_(is_strict) {}

template <typename T, typename E>
Size2 MatrixTriangular<T, E>::size() const {
    return u_.size();
}

template <typename T, typename E>
size_t MatrixTriangular<T, E>::rows() const {
    return u_.rows();
}

template <typename T, typename E>
size_t MatrixTriangular<T, E>::cols() const {
    return u_.cols();
}

template <typename T, typename E>
T MatrixTriangular<T, E>::operator()(size_t i, size_t j) const {
    if (i < j) {
        return (is_upper_) ? u_(i, j) : 0;
    } else if (i > j) {
        return (!is_upper_) ? u_(i, j) : 0;
    } else {
        return (!is_strict_) ? u_(i, j) : 0;
    }
}

// MARK: - MatrixBinaryOp

template <typename T, typename E1, typename E2, typename Op>
MatrixBinaryOp<T, E1, E2, Op>::MatrixBinaryOp(const E1 &u, const E2 &v) : u_(u), v_(v) {
    VOX_ASSERT(u.size() == v.size());
}

template <typename T, typename E1, typename E2, typename Op>
Size2 MatrixBinaryOp<T, E1, E2, Op>::size() const {
    return v_.size();
}

template <typename T, typename E1, typename E2, typename Op>
size_t MatrixBinaryOp<T, E1, E2, Op>::rows() const {
    return v_.rows();
}

template <typename T, typename E1, typename E2, typename Op>
size_t MatrixBinaryOp<T, E1, E2, Op>::cols() const {
    return v_.cols();
}

template <typename T, typename E1, typename E2, typename Op>
T MatrixBinaryOp<T, E1, E2, Op>::operator()(size_t i, size_t j) const {
    return op_(u_(i, j), v_(i, j));
}

// MARK: - MatrixScalarBinaryOp

template <typename T, typename E, typename Op>
MatrixScalarBinaryOp<T, E, Op>::MatrixScalarBinaryOp(const E &u, const T &v) : u_(u), v_(v) {}

template <typename T, typename E, typename Op>
Size2 MatrixScalarBinaryOp<T, E, Op>::size() const {
    return u_.size();
}

template <typename T, typename E, typename Op>
size_t MatrixScalarBinaryOp<T, E, Op>::rows() const {
    return u_.rows();
}

template <typename T, typename E, typename Op>
size_t MatrixScalarBinaryOp<T, E, Op>::cols() const {
    return u_.cols();
}

template <typename T, typename E, typename Op>
T MatrixScalarBinaryOp<T, E, Op>::operator()(size_t i, size_t j) const {
    return op_(u_(i, j), v_);
}

//

template <typename T, typename ME, typename VE>
MatrixVectorMul<T, ME, VE>::MatrixVectorMul(const ME &m, const VE &v) : m_(m), v_(v) {
    VOX_ASSERT(m_.cols() == v_.size());
}

template <typename T, typename ME, typename VE>
size_t MatrixVectorMul<T, ME, VE>::size() const {
    return v_.size();
}

template <typename T, typename ME, typename VE>
T MatrixVectorMul<T, ME, VE>::operator[](size_t i) const {
    T sum = 0;
    const size_t kN = m_.cols();
    for (size_t j = 0; j < kN; ++j) {
        sum += m_(i, j) * v_[j];
    }
    return sum;
}

// MARK: - MatrixMul

template <typename T, typename E1, typename E2>
MatrixMul<T, E1, E2>::MatrixMul(const E1 &u, const E2 &v) : u_(u), v_(v) {
    VOX_ASSERT(u_.cols() == v_.rows());
}

template <typename T, typename E1, typename E2>
Size2 MatrixMul<T, E1, E2>::size() const {
    return Size2(u_.rows(), v_.cols());
}

template <typename T, typename E1, typename E2>
size_t MatrixMul<T, E1, E2>::rows() const {
    return u_.rows();
}

template <typename T, typename E1, typename E2>
size_t MatrixMul<T, E1, E2>::cols() const {
    return v_.cols();
}

template <typename T, typename E1, typename E2>
T MatrixMul<T, E1, E2>::operator()(size_t i, size_t j) const {
    // Unoptimized mat-mat-mul
    T sum = 0;
    const size_t kN = u_.cols();
    for (size_t k = 0; k < kN; ++k) {
        sum += u_(i, k) * v_(k, j);
    }
    return sum;
}

// MARK: - Operator overloadings

template <typename T, typename E>
MatrixScalarMul<T, E> operator-(const MatrixExpression<T, E> &a) {
    return MatrixScalarMul<T, E>(a(), T(-1));
}

template <typename T, typename E1, typename E2>
MatrixAdd<T, E1, E2> operator+(const MatrixExpression<T, E1> &a, const MatrixExpression<T, E2> &b) {
    return MatrixAdd<T, E1, E2>(a(), b());
}

template <typename T, typename E>
MatrixScalarAdd<T, E> operator+(const MatrixExpression<T, E> &a, T b) {
    return MatrixScalarAdd<T, E>(a(), b);
}

template <typename T, typename E>
MatrixScalarAdd<T, E> operator+(T a, const MatrixExpression<T, E> &b) {
    return MatrixScalarAdd<T, E>(b(), a);
}

template <typename T, typename E1, typename E2>
MatrixSub<T, E1, E2> operator-(const MatrixExpression<T, E1> &a, const MatrixExpression<T, E2> &b) {
    return MatrixSub<T, E1, E2>(a(), b());
}

template <typename T, typename E>
MatrixScalarSub<T, E> operator-(const MatrixExpression<T, E> &a, T b) {
    return MatrixScalarSub<T, E>(a(), b);
}

template <typename T, typename E>
MatrixScalarRSub<T, E> operator-(T a, const MatrixExpression<T, E> &b) {
    return MatrixScalarRSub<T, E>(b(), a);
}

template <typename T, typename E>
MatrixScalarMul<T, E> operator*(const MatrixExpression<T, E> &a, T b) {
    return MatrixScalarMul<T, E>(a(), b);
}

template <typename T, typename E>
MatrixScalarMul<T, E> operator*(T a, const MatrixExpression<T, E> &b) {
    return MatrixScalarMul<T, E>(b(), a);
}

template <typename T, typename ME, typename VE>
MatrixVectorMul<T, ME, VE> operator*(const MatrixExpression<T, ME> &a, const VectorExpression<T, VE> &b) {
    return MatrixVectorMul<T, ME, VE>(a(), b());
}

template <typename T, typename E1, typename E2>
MatrixMul<T, E1, E2> operator*(const MatrixExpression<T, E1> &a, const MatrixExpression<T, E2> &b) {
    return MatrixMul<T, E1, E2>(a(), b());
}

template <typename T, typename E>
MatrixScalarDiv<T, E> operator/(const MatrixExpression<T, E> &a, T b) {
    return MatrixScalarDiv<T, E>(a(), b);
}

template <typename T, typename E>
MatrixScalarRDiv<T, E> operator/(T a, const MatrixExpression<T, E> &b) {
    return MatrixScalarRDiv<T, E>(a(), b);
}

}  // namespace vox
