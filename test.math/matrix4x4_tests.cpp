//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "unit_tests_utils.h"
#include "math/matrix4x4.h"
#include "math/matrix_utils.h"

using namespace vox;

TEST(Matrix4x4, Constructors) {
    Matrix4x4D mat;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (i == j) {
                EXPECT_DOUBLE_EQ(1.0, mat(i, j));
            } else {
                EXPECT_DOUBLE_EQ(0.0, mat(i, j));
            }
        }
    }

    Matrix4x4D mat2(3.1);
    for (int i = 0; i < 16; ++i) {
        EXPECT_DOUBLE_EQ(3.1, mat2[i]);
    }

    Matrix4x4D mat3(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);
    for (int i = 0; i < 16; ++i) {
        EXPECT_DOUBLE_EQ(static_cast<double>(i + 1), mat3[i]);
    }

    Matrix4x4D mat4({{1, 2, 3, 4}, {5, 6, 7, 8}, {9, 10, 11, 12}, {13, 14, 15, 16}});
    for (int i = 0; i < 16; ++i) {
        EXPECT_DOUBLE_EQ(static_cast<double>(i + 1), mat4[i]);
    }

    Matrix4x4D mat5(mat4);
    for (int i = 0; i < 16; ++i) {
        EXPECT_DOUBLE_EQ(static_cast<double>(i + 1), mat5[i]);
    }

    double arr[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    Matrix4x4D mat6(arr);
    for (int i = 0; i < 16; ++i) {
        EXPECT_DOUBLE_EQ(static_cast<double>(i + 1), mat6[i]);
    }
}

TEST(Matrix4x4, SetMethods) {
    Matrix4x4D mat;

    mat.set(3.1);
    for (int i = 0; i < 16; ++i) {
        EXPECT_DOUBLE_EQ(3.1, mat[i]);
    }

    mat.set(0.0);
    mat.set(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);
    for (int i = 0; i < 16; ++i) {
        EXPECT_DOUBLE_EQ(static_cast<double>(i + 1), mat[i]);
    }

    mat.set(0.0);
    mat.set({{1, 2, 3, 4}, {5, 6, 7, 8}, {9, 10, 11, 12}, {13, 14, 15, 16}});
    for (int i = 0; i < 16; ++i) {
        EXPECT_DOUBLE_EQ(static_cast<double>(i + 1), mat[i]);
    }

    mat.set(0.0);
    mat.set(Matrix4x4D(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16));
    for (int i = 0; i < 16; ++i) {
        EXPECT_DOUBLE_EQ(static_cast<double>(i + 1), mat[i]);
    }

    mat.set(0.0);
    double arr[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    mat.set(arr);
    for (int i = 0; i < 16; ++i) {
        EXPECT_DOUBLE_EQ(static_cast<double>(i + 1), mat[i]);
    }

    mat.set(0.0);
    mat.setDiagonal(3.1);
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (i == j) {
                EXPECT_DOUBLE_EQ(3.1, mat(i, j));
            } else {
                EXPECT_DOUBLE_EQ(0.0, mat(i, j));
            }
        }
    }

    mat.set(0.0);
    mat.setOffDiagonal(4.2);
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (i != j) {
                EXPECT_DOUBLE_EQ(4.2, mat(i, j));
            } else {
                EXPECT_DOUBLE_EQ(0.0, mat(i, j));
            }
        }
    }

    mat.set(0.0);
    mat.setRow(0, Vector4D(1, 5, 9, 13));
    mat.setRow(1, Vector4D(2, 6, 10, 14));
    mat.setRow(2, Vector4D(3, 7, 11, 15));
    mat.setRow(3, Vector4D(4, 8, 12, 16));

    for (int i = 0; i < 16; ++i) {
        EXPECT_DOUBLE_EQ(static_cast<double>(i + 1), mat[i]);
    }

    mat.set(0.0);
    mat.setColumn(0, Vector4D(1, 2, 3, 4));
    mat.setColumn(1, Vector4D(5, 6, 7, 8));
    mat.setColumn(2, Vector4D(9, 10, 11, 12));
    mat.setColumn(3, Vector4D(13, 14, 15, 16));
    for (int i = 0; i < 16; ++i) {
        EXPECT_DOUBLE_EQ(static_cast<double>(i + 1), mat[i]);
    }
}

TEST(Matrix4x4, BasicGetters) {
    Matrix4x4D mat(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16),
            mat2(1.01, 2.01, 2.99, 4.0, 4.99, 6.001, 7.0003, 8.0, 8.99, 10.01, 11, 11.99, 13.01, 14.001, 14.999, 16),
            mat3;

    EXPECT_TRUE(mat.isSimilar(mat2, 0.02));
    EXPECT_FALSE(mat.isSimilar(mat2, 0.001));

    EXPECT_TRUE(mat.isSquare());

    EXPECT_EQ(4u, mat.rows());
    EXPECT_EQ(4u, mat.cols());
}

TEST(Matrix4x4, BinaryOperators) {
    Matrix4x4D mat(-16, 15, -14, 13, -12, 11, -10, 9, -8, 7, -6, 5, -6, 3, -2, 1), mat2;
    Vector4D vec;

    mat2 = mat.add(2.0);
    EXPECT_TRUE(mat2.isSimilar({{-14, 17, -12, 15}, {-10, 13, -8, 11}, {-6, 9, -4, 7}, {-4, 5, 0, 3}}));

    mat2 = mat.add(Matrix4x4D(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16));
    EXPECT_TRUE(mat2.isSimilar({{-15, 17, -11, 17}, {-7, 17, -3, 17}, {1, 17, 5, 17}, {7, 17, 13, 17}}));

    mat2 = mat.sub(2.0);
    EXPECT_TRUE(mat2.isSimilar({{-18, 13, -16, 11}, {-14, 9, -12, 7}, {-10, 5, -8, 3}, {-8, 1, -4, -1}}));

    mat2 = mat.sub(Matrix4x4D(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16));
    EXPECT_TRUE(mat2.isSimilar({{-17, 13, -17, 9}, {-17, 5, -17, 1}, {-17, -3, -17, -7}, {-19, -11, -17, -15}}));

    mat2 = mat.mul(2.0);
    EXPECT_TRUE(mat2.isSimilar({{-32, 30, -28, 26}, {-24, 22, -20, 18}, {-16, 14, -12, 10}, {-12, 6, -4, 2}}));

    vec = mat.mul(Vector4D(1, 2, 3, 4));
    EXPECT_TRUE(vec.isSimilar(Vector4D(-88, 70, -60, 50)));

    mat2 = mat.mul(Matrix4x4D(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16));
    EXPECT_TRUE(mat2.isSimilar(
            {{-88, 70, -60, 50}, {-256, 214, -188, 162}, {-424, 358, -316, 274}, {-592, 502, -444, 386}}));

    mat2 = mat.div(2.0);
    EXPECT_TRUE(mat2.isSimilar({{-8.0, 15.0 / 2.0, -7.0, 13.0 / 2.0},
                                {-6.0, 11.0 / 2.0, -5.0, 9.0 / 2.0},
                                {-4.0, 7.0 / 2.0, -3.0, 5.0 / 2.0},
                                {-3.0, 3.0 / 2.0, -1.0, 1.0 / 2.0}}));

    mat2 = mat.radd(2.0);
    EXPECT_TRUE(mat2.isSimilar({{-14, 17, -12, 15}, {-10, 13, -8, 11}, {-6, 9, -4, 7}, {-4, 5, 0, 3}}));

    mat2 = mat.radd(Matrix4x4D(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16));
    EXPECT_TRUE(mat2.isSimilar({{-15, 17, -11, 17}, {-7, 17, -3, 17}, {1, 17, 5, 17}, {7, 17, 13, 17}}));

    mat2 = mat.rsub(2.0);
    EXPECT_TRUE(mat2.isSimilar({{18, -13, 16, -11}, {14, -9, 12, -7}, {10, -5, 8, -3}, {8, -1, 4, 1}}));

    mat2 = mat.rsub(Matrix4x4D(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16));
    EXPECT_TRUE(mat2.isSimilar({{17, -13, 17, -9}, {17, -5, 17, -1}, {17, 3, 17, 7}, {19, 11, 17, 15}}));

    mat2 = mat.rmul(2.0);
    EXPECT_TRUE(mat2.isSimilar({{-32, 30, -28, 26}, {-24, 22, -20, 18}, {-16, 14, -12, 10}, {-12, 6, -4, 2}}));

    mat2 = mat.rmul(Matrix4x4D(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16));
    EXPECT_TRUE(mat2.isSimilar({{102, 100, 98, 96}, {70, 68, 66, 64}, {38, 36, 34, 32}, {4, 0, -4, -8}}));

    mat2 = mat.rdiv(2.0);
    EXPECT_TRUE(mat2.isSimilar({{-1.0 / 8.0, 2.0 / 15.0, -1.0 / 7.0, 2.0 / 13.0},
                                {-1.0 / 6.0, 2.0 / 11.0, -1.0 / 5.0, 2.0 / 9.0},
                                {-1.0 / 4.0, 2.0 / 7.0, -1.0 / 3.0, 2.0 / 5.0},
                                {-1.0 / 3.0, 2.0 / 3.0, -1.0, 2.0}}));
}

TEST(Matrix4x4, AugmentedOperators) {
    Matrix4x4D mat(-16, 15, -14, 13, -12, 11, -10, 9, -8, 7, -6, 5, -6, 3, -2, 1);

    mat.iadd(2.0);
    EXPECT_TRUE(mat.isSimilar({{-14, 17, -12, 15}, {-10, 13, -8, 11}, {-6, 9, -4, 7}, {-4, 5, 0, 3}}));

    mat.set(-16, 15, -14, 13, -12, 11, -10, 9, -8, 7, -6, 5, -6, 3, -2, 1);
    mat.iadd(Matrix4x4D(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16));
    EXPECT_TRUE(mat.isSimilar({{-15, 17, -11, 17}, {-7, 17, -3, 17}, {1, 17, 5, 17}, {7, 17, 13, 17}}));

    mat.set(-16, 15, -14, 13, -12, 11, -10, 9, -8, 7, -6, 5, -6, 3, -2, 1);
    mat.isub(2.0);
    EXPECT_TRUE(mat.isSimilar({{-18, 13, -16, 11}, {-14, 9, -12, 7}, {-10, 5, -8, 3}, {-8, 1, -4, -1}}));

    mat.set(-16, 15, -14, 13, -12, 11, -10, 9, -8, 7, -6, 5, -6, 3, -2, 1);
    mat.isub(Matrix4x4D(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16));
    EXPECT_TRUE(mat.isSimilar({{-17, 13, -17, 9}, {-17, 5, -17, 1}, {-17, -3, -17, -7}, {-19, -11, -17, -15}}));

    mat.set(-16, 15, -14, 13, -12, 11, -10, 9, -8, 7, -6, 5, -6, 3, -2, 1);
    mat.imul(2.0);
    EXPECT_TRUE(mat.isSimilar({{-32, 30, -28, 26}, {-24, 22, -20, 18}, {-16, 14, -12, 10}, {-12, 6, -4, 2}}));

    mat.set(-16, 15, -14, 13, -12, 11, -10, 9, -8, 7, -6, 5, -6, 3, -2, 1);
    mat.imul(Matrix4x4D(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16));
    EXPECT_TRUE(mat.isSimilar(
            {{-88, 70, -60, 50}, {-256, 214, -188, 162}, {-424, 358, -316, 274}, {-592, 502, -444, 386}}));

    mat.set(-16, 15, -14, 13, -12, 11, -10, 9, -8, 7, -6, 5, -6, 3, -2, 1);
    mat.idiv(2.0);
    EXPECT_TRUE(mat.isSimilar({{-8.0, 15.0 / 2.0, -7.0, 13.0 / 2.0},
                               {-6.0, 11.0 / 2.0, -5.0, 9.0 / 2.0},
                               {-4.0, 7.0 / 2.0, -3.0, 5.0 / 2.0},
                               {-3.0, 3.0 / 2.0, -1.0, 1.0 / 2.0}}));
}

TEST(Matrix4x4, Modifiers) {
    Matrix4x4D mat(-16, 15, -14, 13, -12, 11, -10, 9, -8, 7, -6, 5, -6, 3, -2, 1);

    mat.transpose();
    EXPECT_TRUE(mat.isSimilar({{-16, -12, -8, -6}, {15, 11, 7, 3}, {-14, -10, -6, -2}, {13, 9, 5, 1}}));

    mat.set(-16, 15, -14, 13, -12, 11, -10, 9, -8, 7, -6, 6, -6, 3, -2, 2);
    mat.invert();
    EXPECT_TRUE(mat.isSimilar({{-1.0 / 2.0, 1.0 / 2.0, 1.0 / 2.0, -1.0 / 2.0},
                               {-5.0 / 2.0, 5.0 / 2.0, 2.0, -1.0},
                               {-5.0 / 4.0, 1.0 / 4.0, 5.0 / 2.0, -1.0 / 2.0},
                               {1.0, -2.0, 1.0, 0.0}}));
}

TEST(Matrix4x4, ComplexGetters) {
    Matrix4x4D mat(-16, 15, -14, 13, -12, 11, -10, 9, -8, 7, -6, 5, -4, 3, -2, 1), mat2;

    EXPECT_DOUBLE_EQ(-8.0, mat.sum());

    EXPECT_DOUBLE_EQ(-0.5, mat.avg());

    EXPECT_DOUBLE_EQ(-16.0, mat.min());

    EXPECT_DOUBLE_EQ(15.0, mat.max());

    EXPECT_DOUBLE_EQ(1.0, mat.absmin());

    EXPECT_DOUBLE_EQ(-16.0, mat.absmax());

    EXPECT_DOUBLE_EQ(-10.0, mat.trace());

    EXPECT_DOUBLE_EQ(0.0, mat.determinant());

    mat2 = mat.diagonal();
    EXPECT_TRUE(mat2.isSimilar(Matrix4x4D(-16, 0, 0, 0, 0, 11, 0, 0, 0, 0, -6, 0, 0, 0, 0, 1)));

    mat2 = mat.offDiagonal();
    EXPECT_TRUE(mat2.isSimilar(Matrix4x4D(0, 15, -14, 13, -12, 0, -10, 9, -8, 7, 0, 5, -4, 3, -2, 0)));

    mat2 = mat.strictLowerTri();
    EXPECT_TRUE(mat2.isSimilar(Matrix4x4D(0, 15, -14, 13, 0, 0, -10, 9, 0, 0, 0, 5, 0, 0, 0, 0)));

    mat2 = mat.strictUpperTri();
    EXPECT_TRUE(mat2.isSimilar(Matrix4x4D(0, 0, 0, 0, -12, 0, 0, 0, -8, 7, 0, 0, -4, 3, -2, 0)));

    mat2 = mat.lowerTri();
    EXPECT_TRUE(mat2.isSimilar(Matrix4x4D(-16, 15, -14, 13, 0, 11, -10, 9, 0, 0, -6, 5, 0, 0, 0, 1)));

    mat2 = mat.upperTri();
    EXPECT_TRUE(mat2.isSimilar(Matrix4x4D(-16, 0, 0, 0, -12, 11, 0, 0, -8, 7, -6, 0, -4, 3, -2, 1)));

    mat2 = mat.transposed();
    EXPECT_TRUE(mat2.isSimilar({{-16, -12, -8, -4}, {15, 11, 7, 3}, {-14, -10, -6, -2}, {13, 9, 5, 1}}));

    mat.set(-16, 15, -14, 13, -12, 11, -10, 9, -8, 7, -6, 6, -6, 3, -2, 2);
    mat2 = mat.inverse();
    EXPECT_TRUE(mat2.isSimilar({{-1.0 / 2.0, 1.0 / 2.0, 1.0 / 2.0, -1.0 / 2.0},
                                {-5.0 / 2.0, 5.0 / 2.0, 2.0, -1.0},
                                {-5.0 / 4.0, 1.0 / 4.0, 5.0 / 2.0, -1.0 / 2.0},
                                {1.0, -2.0, 1.0, 0.0}}));

    mat.set(-16, 15, -14, 13, -12, 11, -10, 9, -8, 7, -6, 5, -6, 3, -2, 1);
    Matrix4x4F mat3 = mat.castTo<float>();
    EXPECT_TRUE(mat3.isSimilar(Matrix4x4F(-16, 15, -14, 13, -12, 11, -10, 9, -8, 7, -6, 5, -6, 3, -2, 1)));
}

TEST(Matrix4x4, SetterOperatorOverloadings) {
    Matrix4x4D mat(-16, 15, -14, 13, -12, 11, -10, 9, -8, 7, -6, 5, -6, 3, -2, 1), mat2;

    mat2 = mat;
    EXPECT_TRUE(mat2.isSimilar(Matrix4x4D(-16, 15, -14, 13, -12, 11, -10, 9, -8, 7, -6, 5, -6, 3, -2, 1)));

    mat += 2.0;
    EXPECT_TRUE(mat.isSimilar({{-14, 17, -12, 15}, {-10, 13, -8, 11}, {-6, 9, -4, 7}, {-4, 5, 0, 3}}));

    mat.set(-16, 15, -14, 13, -12, 11, -10, 9, -8, 7, -6, 5, -6, 3, -2, 1);
    mat += Matrix4x4D(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);
    EXPECT_TRUE(mat.isSimilar({{-15, 17, -11, 17}, {-7, 17, -3, 17}, {1, 17, 5, 17}, {7, 17, 13, 17}}));

    mat.set(-16, 15, -14, 13, -12, 11, -10, 9, -8, 7, -6, 5, -6, 3, -2, 1);
    mat -= 2.0;
    EXPECT_TRUE(mat.isSimilar({{-18, 13, -16, 11}, {-14, 9, -12, 7}, {-10, 5, -8, 3}, {-8, 1, -4, -1}}));

    mat.set(-16, 15, -14, 13, -12, 11, -10, 9, -8, 7, -6, 5, -6, 3, -2, 1);
    mat -= Matrix4x4D(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);
    EXPECT_TRUE(mat.isSimilar({{-17, 13, -17, 9}, {-17, 5, -17, 1}, {-17, -3, -17, -7}, {-19, -11, -17, -15}}));

    mat.set(-16, 15, -14, 13, -12, 11, -10, 9, -8, 7, -6, 5, -6, 3, -2, 1);
    mat *= 2.0;
    EXPECT_TRUE(mat.isSimilar({{-32, 30, -28, 26}, {-24, 22, -20, 18}, {-16, 14, -12, 10}, {-12, 6, -4, 2}}));

    mat.set(-16, 15, -14, 13, -12, 11, -10, 9, -8, 7, -6, 5, -6, 3, -2, 1);
    mat *= Matrix4x4D(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);
    EXPECT_TRUE(mat.isSimilar(
            {{-88, 70, -60, 50}, {-256, 214, -188, 162}, {-424, 358, -316, 274}, {-592, 502, -444, 386}}));

    mat.set(-16, 15, -14, 13, -12, 11, -10, 9, -8, 7, -6, 5, -6, 3, -2, 1);
    mat /= 2.0;
    EXPECT_TRUE(mat.isSimilar({{-8.0, 15.0 / 2.0, -7.0, 13.0 / 2.0},
                               {-6.0, 11.0 / 2.0, -5.0, 9.0 / 2.0},
                               {-4.0, 7.0 / 2.0, -3.0, 5.0 / 2.0},
                               {-3.0, 3.0 / 2.0, -1.0, 1.0 / 2.0}}));
}

TEST(Matrix4x4, GetterOperatorOverloadings) {
    Matrix4x4D mat(-16, 15, -14, 13, -12, 11, -10, 9, -8, 7, -6, 5, -4, 3, -2, 1);

    double sign = -1.0;
    for (int i = 0; i < 16; ++i) {
        EXPECT_DOUBLE_EQ(sign * (16 - i), mat[i]);
        sign *= -1.0;

        mat[i] *= -1.0;
    }

    sign = 1.0;
    for (int i = 0; i < 16; ++i) {
        EXPECT_DOUBLE_EQ(sign * (16 - i), mat[i]);
        sign *= -1.0;
    }

    sign = 1.0;
    for (int j = 0; j < 4; ++j) {
        for (int i = 0; i < 4; ++i) {
            double ans = sign * (16 - (i + 4 * j));
            EXPECT_DOUBLE_EQ(ans, mat(i, j));
            sign *= -1.0;
        }
    }

    mat.set(-16, 15, -14, 13, -12, 11, -10, 9, -8, 7, -6, 5, -4, 3, -2, 1);
    EXPECT_TRUE(mat == Matrix4x4D(-16, 15, -14, 13, -12, 11, -10, 9, -8, 7, -6, 5, -4, 3, -2, 1));

    mat.set(16, -15, 14, -13, 12, -11, 10, -9, -8, 7, -6, 5, -4, 3, -2, 1);
    EXPECT_TRUE(mat != Matrix4x4D(-16, 15, -14, 13, -12, 11, -10, 9, -8, 7, -6, 5, -4, 3, -2, 1));
}

TEST(Matrix4x4, Helpers) {
    Matrix4x4D mat = Matrix4x4D::makeZero();
    for (int i = 0; i < 16; ++i) {
        EXPECT_EQ(0.0, mat[i]);
    }

    mat = Matrix4x4D::makeIdentity();
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (i == j) {
                EXPECT_DOUBLE_EQ(1.0, mat(i, j));
            } else {
                EXPECT_DOUBLE_EQ(0.0, mat(i, j));
            }
        }
    }

    mat = makeScaleMatrix(3.0, -4.0, 2.4);
    EXPECT_TRUE(
            mat.isSimilar(Matrix4x4D(3.0, 0.0, 0.0, 0.0, 0.0, -4.0, 0.0, 0.0, 0.0, 0.0, 2.4, 0.0, 0.0, 0.0, 0.0, 1.0)));

    mat = makeScaleMatrix(Vector3D(-2.0, 5.0, 3.5));
    EXPECT_TRUE(
            mat.isSimilar(Matrix4x4D(-2.0, 0.0, 0.0, 0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 3.5, 0.0, 0.0, 0.0, 0.0, 1.0)));

    mat = makeRotationMatrix(Vector3D(-1.0 / 3.0, 2.0 / 3.0, 2.0 / 3.0), -74.0 / 180.0 * kPiD);
    EXPECT_TRUE(
            mat.isSimilar(Matrix4x4D(0.36, -0.8, 0.48, 0, 0.48, 0.60, 0.64, 0, -0.8, 0.0, 0.6, 0, 0, 0, 0, 1), 0.05));

    mat = makeTranslationMatrix(Point3D(-2.0, 5.0, 3.5));
    EXPECT_TRUE(
            mat.isSimilar(Matrix4x4D(1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, -2.0, 5.0, 3.5, 1.0)));
}

// MARK: -
TEST(Matrix4x4, multiply) {
    const auto a = Matrix4x4D(1, 2, 3.3, 4, 5, 6, 7, 8, 9, 10.9, 11, 12, 13, 14, 15, 16);
    const auto b = Matrix4x4D(16, 15, 14, 13, 12, 11, 10, 9, 8.88, 7, 6, 5, 4, 3, 2, 1);
    const Matrix4x4D out = a * b;
    EXPECT_TRUE(out.isSimilar(Matrix4x4D(386, 456.60000000000002, 506.80000000000001, 560, 274, 325, 361.60000000000002,
                                         400, 162.88, 195.16000000000003, 219.304, 243.52000000000001, 50,
                                         61.799999999999997, 71.200000000000003, 80)));
}

TEST(Matrix4x4, lerp) {
    const auto a = Matrix4x4D(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);
    const auto b = Matrix4x4D(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);
    const auto c = lerp(a, b, 0.7);
    EXPECT_TRUE(c.isSimilar(Matrix4x4D(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16)));
}

TEST(Matrix4x4, rotationQuaternion) {
    const auto q = QuaternionD(1, 2, 3, 4);
    const auto out = q.matrix4();
    EXPECT_TRUE(out.isSimilar(Matrix4x4D(-25, 28, -10, 0, -20, -19, 20, 0, 22, 4, -9, 0, 0, 0, 0, 1)));
}

TEST(Matrix4x4, rotationAxisAngle) {
    auto out = Matrix4x4D();
    out.rotateAxisAngle(Vector3D(0, 1, 0), M_PI / 3);
    EXPECT_TRUE(out.isSimilar(Matrix4x4D(0.5000000000000001, 0, -0.8660254037844386, 0, 0, 1, 0, 0, 0.8660254037844386,
                                         0, 0.5000000000000001, 0, 0, 0, 0, 1)));
}

TEST(Matrix4x4, rotationTranslation) {
    const auto q = QuaternionD(1, 0.5, 2, 1);
    const auto v = Point3D(1, 1, 1);
    const auto out = makeRotationTranslationMatrix(q, v);
    EXPECT_TRUE(out.isSimilar(Matrix4x4D(-7.5, 5, 3, 0, -3, -9, 4, 0, 5, 0, -1.5, 0, 1, 1, 1, 1)));
}

TEST(Matrix4x4, affineTransformation) {
    const auto q = QuaternionD(1, 0.5, 2, 1);
    const auto v = Point3D(1, 1, 1);
    const auto s = Vector3D(1, 0.5, 2);
    const auto out = makeAffineMatrix(s, q, v);
    EXPECT_TRUE(out.isSimilar(Matrix4x4D(-7.5, 5, 3, 0, -1.5, -4.5, 2, 0, 10, 0, -3, 0, 1, 1, 1, 1)));
}

TEST(Matrix4x4, scaling) {
    const auto a = Matrix4x4D();
    auto out = a.scale(Vector3D(1, 2, 0.5));
    EXPECT_TRUE(out.isSimilar(Matrix4x4D(1, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0.5, 0, 0, 0, 0, 1)));
}

TEST(Matrix4x4, translation) {
    const auto v = Point3D(1, 2, 0.5);
    const auto out = makeTranslationMatrix(v);
    EXPECT_TRUE(out.isSimilar(Matrix4x4D(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 2, 0.5, 1)));
}

TEST(Matrix4x4, invert) {
    const auto a = Matrix4x4D(1, 2, 3.3, 4, 5, 6, 7, 8, 9, 10.9, 11, 12, 13, 14, 15, 16);
    const auto out = a.inverse();
    EXPECT_TRUE(out.isSimilar(Matrix4x4D(
            -1.1111111111105691, 1.370370370369675, -0.74074074074036778, 0.14814814814808058, 0, -0.55555555555526703,
            1.1111111111105605, -0.55555555555528457, 3.3333333333317077, -4.9999999999975442, 0, 1.666666666665845,
            -2.2222222222211383, 4.0601851851831885, -0.37037037037018389, -1.1342592592587024)));
}

TEST(Matrix4x4, lookAt) {
    auto eye = Point3D(0, 0, -8);
    auto target = Point3D(0, 0, 0);
    auto up = Vector3D(0, 1, 0);
    auto out = makeLookAtMatrix(eye, target, up);
    EXPECT_TRUE(out.isSimilar(Matrix4x4D(-1, 0, 0, 0, 0, 1, 0, 0, 0, 0, -1, 0, 0, 0, -8, 1)));

    eye = Point3D(0, 0, 0);
    target = Point3D(0, 1, -1);
    up = Vector3D(0, 1, 0);
    out = makeLookAtMatrix(eye, target, up);
    EXPECT_TRUE(out.isSimilar(Matrix4x4D(1, 0, 0, 0, 0, 0.70710678118654746, -0.70710678118654746, 0, 0,
                                         0.70710678118654746, 0.70710678118654746, 0, 0, 0, 0, 1)));
}

TEST(Matrix4x4, ortho) {
    const Matrix4x4D out = makeOrtho<double>(0, 2, -1, 1, 0.1, 100);
    EXPECT_TRUE(out.isSimilar(
            Matrix4x4D(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, -0.02002002002002002, 0, -1, 0, -1.002002002002002, 1)));
}

TEST(Matrix4x4, perspective) {
    const Matrix4x4D out = makePerspective<double>(1, 1.5, 0.1, 100);
    EXPECT_TRUE(out.isSimilar(Matrix4x4D(1.2203251478083013, 0, 0, 0, 0, 1.830487721712452, 0, 0, 0, 0,
                                         -1.002002002002002, -1, 0, 0, -0.20020020020020018, 0)));
}

TEST(Matrix4x4, rotateAxisAngle) {
    const auto a = Matrix4x4D(1, 2, 3.3, 4, 5, 6, 7, 8, 9, 10.9, 11, 12, 13, 14, 15, 16);
    const auto out = a.rotateAxisAngle(Vector3D(0, 1, 0), M_PI / 3);
    EXPECT_TRUE(out.isSimilar(Matrix4x4D(-7.294228634059947, -8.439676901250381, -7.876279441628824, -8.392304845413264,
                                         5, 6, 7, 8, 5.366025403784439, 7.182050807568878, 8.357883832488648,
                                         9.464101615137757, 13, 14, 15, 16)));
}

TEST(Matrix4x4, scale) {
    const auto a = Matrix4x4D(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);
    const auto out = a.scale(Vector3D(1, 2, 0.5));
    EXPECT_TRUE(out.isSimilar(Matrix4x4D(1, 2, 3, 4, 10, 12, 14, 16, 4.5, 5, 5.5, 6, 13, 14, 15, 16)));
}

TEST(Matrix4x4, translate) {
    const auto a = Matrix4x4D(1, 2, 3.3, 4, 5, 6, 7, 8, 9, 10.9, 11, 12, 13, 14, 15, 16);
    const auto out = a.translate(Vector3D(1, 2, 0.5));
    EXPECT_TRUE(out.isSimilar(Matrix4x4D(1, 2, 3.3, 4, 5, 6, 7, 8, 9, 10.9, 11, 12, 28.5, 33.45, 37.8, 42)));
}

TEST(Matrix4x4, transpose) {
    const auto a = Matrix4x4D(1, 2, 3.3, 4, 5, 6, 7, 8, 9, 10.9, 11, 12, 13, 14, 15, 16);
    const auto out = a.transposed();
    EXPECT_TRUE(out.isSimilar(Matrix4x4D(1, 5, 9, 13, 2, 6, 10.9, 14, 3.3, 7, 11, 15, 4, 8, 12, 16)));
}

TEST(Matrix4x4, determinant) {
    const auto a = Matrix4x4D(1, 2, 3, 4, 5, 6, 7, 8, 9, 10.9, 11, 12, 13, 14, 15, 16);
    EXPECT_FLOAT_EQ(a.determinant(), 0);
}

TEST(Matrix4x4, decompose) {
    const auto a = Matrix4x4D(1, 2, 3, 4, 5, 6, 7, 8, 9, 10.9, 11, 12, 13, 14, 15, 16);
    // const a = new Matrix(0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);
    auto pos = Point3D();
    auto quat = QuaternionD();
    auto scale = Vector3D();

    decompose(a, pos, quat, scale);
    EXPECT_VECTOR3_EQ(pos, Point3D(13, 14, 15));
    EXPECT_VECTOR4_EQ(
            quat, QuaternionD(0.018790394403601106, -0.095541305726454986, 0.018447618636927929, 0.78317953036995014));
    EXPECT_VECTOR3_EQ(scale, Vector3D(3.7416573867739413, 10.488088481701515, 17.911169699380327));
}

TEST(Matrix4x4, getXXX) {
    const auto a = Matrix4x4D(1, 2, 3, 4, 5, 6, 7, 8, 9, 10.9, 11, 12, 13, 14, 15, 16);

    // getRotation
    auto quat = getRotation(a);
    EXPECT_VECTOR4_EQ(quat,
                      QuaternionD(-0.44736068104759547, 0.6882472016116852, -0.3441236008058426, 2.179449471770337));

    // getScaling
    auto scale = getScaling(a);
    EXPECT_VECTOR3_EQ(scale, Vector3D(3.7416573867739413, 10.488088481701515, 17.911169699380327));

    // getTranslation
    auto translation = getTranslation(a);
    EXPECT_VECTOR3_EQ(translation, Point3D(13, 14, 15));
}

TEST(Matrix4x4, transform3) {
    const auto a = Point3D(2, 3, 4);
    const auto b = Vector4D(2, 3, 4, 1);
    const auto m4 = Matrix4x4D(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1);
    auto out = m4 * a;
    const auto out4 = m4 * b;
    EXPECT_FLOAT_EQ(out.x, out4.x / out4.w);
    EXPECT_FLOAT_EQ(out.y, out4.y / out4.w);
    EXPECT_FLOAT_EQ(out.z, out4.z / out4.w);

    out = QuaternionD() * a;
    EXPECT_VECTOR3_EQ(a, Point3D(out.x, out.y, out.z));
    out = QuaternionD(2, 3, 4, 5) * a;
    EXPECT_VECTOR3_EQ(out, Point3D(108, 162, 216));
}

TEST(Vector4, transform4) {
    const auto a = Vector4D(2, 3, 4, 5);
    const auto m4 = Matrix4x4D(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0);
    Vector4D out = m4 * a;
    EXPECT_VECTOR4_EQ(out, Vector4D(2, 3, 9, 0));

    out = QuaternionD() * a;
    EXPECT_VECTOR4_EQ(a, Vector4D(out.x, out.y, out.z, out.w));
    out = QuaternionD(2, 3, 4, 5) * a;
    EXPECT_VECTOR4_EQ(out, Vector4D(108, 162, 216, 5));
}
