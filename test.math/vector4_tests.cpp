//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "unit_tests_utils.h"
#include "math/vector4.h"

using namespace vox;

TEST(Vector4, BasicGetterMethods) {
    Vector4F vec(3.f, 7.f, -1.f, 11.f), vec2(-3.f, -7.f, 1.f, 4.f);
    Vector4F vec3(3.f, 1.f, -5.f, 4.f), vec4(-3.f, 2.f, 1.f, -4.f);

    float sum = vec.sum();
    EXPECT_FLOAT_EQ(20.f, sum);

    float avg = vec.avg();
    EXPECT_FLOAT_EQ(5.f, avg);

    float min = vec.min();
    EXPECT_FLOAT_EQ(-1.f, min);

    float max = vec.max();
    EXPECT_FLOAT_EQ(11.f, max);

    float absmin = vec2.absmin();
    EXPECT_FLOAT_EQ(1.f, absmin);

    float absmax = vec2.absmax();
    EXPECT_FLOAT_EQ(-7.f, absmax);

    size_t daxis = vec3.dominantAxis();
    EXPECT_EQ((size_t)2, daxis);

    size_t saxis = vec4.subminantAxis();
    EXPECT_EQ((size_t)2, saxis);

    float eps = 1e-6f;
    vec2 = vec.normalized();
    float lenSqr = vec2.x * vec2.x + vec2.y * vec2.y + vec2.z * vec2.z + vec2.w * vec2.w;
    EXPECT_TRUE(lenSqr - 1.f < eps);

    vec2.imul(2.f);
    float len = vec2.length();
    EXPECT_TRUE(len - 2.f < eps);

    lenSqr = vec2.lengthSquared();
    EXPECT_TRUE(lenSqr - 4.f < eps);
}

// MARK: -
TEST(Vector4, add) {
    const auto a = Vector4D(2, 3, 4, 1);
    const auto b = Vector4D(-3, 5, 0, 2);
    const auto out = a + b;

    EXPECT_VECTOR4_EQ(out, Vector4D(-1, 8, 4, 3));
}

TEST(Vector4, subtract) {
    const auto a = Vector4D(2, 3, 4, 1);
    const auto b = Vector4D(-3, 5, 1, 2);
    const auto out = a - b;

    EXPECT_VECTOR4_EQ(out, Vector4D(5, -2, 3, -1));
}

TEST(Vector4, multiply) {
    const auto a = Vector4D(2, 3, 4, 1);
    const auto b = Vector4D(-3, 5, 0.2, 2);
    const auto out = a * b;

    EXPECT_VECTOR4_EQ(out, Vector4D(-6, 15, 0.8, 2));
}

TEST(Vector4, divide) {
    const auto a = Vector4D(2, 3, 4, 1);
    const auto b = Vector4D(-4, 5, 16, 2);
    const auto out = a / b;

    EXPECT_VECTOR4_EQ(out, Vector4D(-0.5, 0.6, 0.25, 0.5));
}

TEST(Vector4, dot) {
    const auto a = Vector4D(2, 3, 1, 1);
    const auto b = Vector4D(-4, 5, 1, 1);

    EXPECT_FLOAT_EQ(a.dot(b), 9);
}

TEST(Vector4, distance) {
    const auto a = Vector4D(1, 2, 3, 0);
    const auto b = Vector4D(4, 6, 3, 0);

    EXPECT_FLOAT_EQ(a.distanceTo(b), 5);
    EXPECT_FLOAT_EQ(a.distanceSquaredTo(b), 25);
}

TEST(Vector4, lerp) {
    const auto a = Vector4D(0, 1, 2, 0);
    const auto b = Vector4D(2, 2, 0, 0);
    const auto out = lerp(a, b, 0.5);
    EXPECT_VECTOR4_EQ(out, Vector4D(1, 1.5, 1, 0));
}

TEST(Vector4, max) {
    const auto a = Vector4D(0, 10, 1, -1);
    const auto b = Vector4D(2, 3, 5, -5);
    const auto out = max(a, b);
    EXPECT_VECTOR4_EQ(out, Vector4D(2, 10, 5, -1));
}

TEST(Vector4, min) {
    const auto a = Vector4D(0, 10, 1, -1);
    const auto b = Vector4D(2, 3, 5, -5);
    const auto out = min(a, b);
    EXPECT_VECTOR4_EQ(out, Vector4D(0, 3, 1, -5));
}

TEST(Vector4, negate) {
    const auto a = Vector4D(4, -4, 0, 1);
    const auto out = -a;

    EXPECT_VECTOR4_EQ(out, Vector4D(-4, 4, 0, -1));
}

TEST(Vector4, normalize) {
    Vector4D a = Vector4D(3, 4, 0, 0);
    Vector4D out = a.normalized();
    EXPECT_VECTOR4_NEAR(out, Vector4D(0.6, 0.8, 0, 0), 1.0e-5);
}

TEST(Vector4, scale) {
    const auto a = Vector4D(3, 4, 5, 0);
    const auto out = a * 3.0;
    EXPECT_VECTOR4_EQ(out, Vector4D(9, 12, 15, 0));
}
