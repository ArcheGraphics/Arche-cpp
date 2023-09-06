//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef SRC_TESTS_UNIT_TESTS_UNIT_TESTS_UTILS_H_
#define SRC_TESTS_UNIT_TESTS_UNIT_TESTS_UTILS_H_

#include <gtest/gtest.h>

#include "math/point3.h"
#include "math/vector3.h"

#define EXPECT_VECTOR2_EQ(expected, actual)     \
    EXPECT_DOUBLE_EQ((expected).x, (actual).x); \
    EXPECT_DOUBLE_EQ((expected).y, (actual).y);

#define EXPECT_VECTOR2_NEAR(expected, actual, eps) \
    EXPECT_NEAR((expected).x, (actual).x, eps);    \
    EXPECT_NEAR((expected).y, (actual).y, eps);

#define EXPECT_VECTOR3_EQ(expected, actual)     \
    EXPECT_DOUBLE_EQ((expected).x, (actual).x); \
    EXPECT_DOUBLE_EQ((expected).y, (actual).y); \
    EXPECT_DOUBLE_EQ((expected).z, (actual).z);

#define EXPECT_VECTOR3_NEAR(expected, actual, eps) \
    EXPECT_NEAR((expected).x, (actual).x, eps);    \
    EXPECT_NEAR((expected).y, (actual).y, eps);    \
    EXPECT_NEAR((expected).z, (actual).z, eps);

#define EXPECT_VECTOR4_EQ(expected, actual)     \
    EXPECT_DOUBLE_EQ((expected).x, (actual).x); \
    EXPECT_DOUBLE_EQ((expected).y, (actual).y); \
    EXPECT_DOUBLE_EQ((expected).z, (actual).z); \
    EXPECT_DOUBLE_EQ((expected).w, (actual).w);

#define EXPECT_VECTOR4_NEAR(expected, actual, eps) \
    EXPECT_NEAR((expected).x, (actual).x, eps);    \
    EXPECT_NEAR((expected).y, (actual).y, eps);    \
    EXPECT_NEAR((expected).z, (actual).z, eps);    \
    EXPECT_NEAR((expected).w, (actual).w, eps);

#define EXPECT_COLOR_EQ(expected, actual)       \
    EXPECT_DOUBLE_EQ((expected).r, (actual).r); \
    EXPECT_DOUBLE_EQ((expected).g, (actual).g); \
    EXPECT_DOUBLE_EQ((expected).b, (actual).b); \
    EXPECT_DOUBLE_EQ((expected).a, (actual).a);

#define EXPECT_COLOR_NEAR(expected, actual, eps) \
    EXPECT_NEAR((expected).r, (actual).r, eps);  \
    EXPECT_NEAR((expected).g, (actual).g, eps);  \
    EXPECT_NEAR((expected).b, (actual).b, eps);  \
    EXPECT_NEAR((expected).a, (actual).a, eps);

#define EXPECT_BOUNDING_BOX2_EQ(expected, actual)                      \
    EXPECT_VECTOR2_EQ((expected).lower_corner, (actual).lower_corner); \
    EXPECT_VECTOR2_EQ((expected).upper_corner, (actual).upper_corner);

#define EXPECT_BOUNDING_BOX2_NEAR(expected, actual, eps)                      \
    EXPECT_VECTOR2_NEAR((expected).lower_corner, (actual).lower_corner, eps); \
    EXPECT_VECTOR2_NEAR((expected).upper_corner, (actual).upper_corner, eps);

#define EXPECT_BOUNDING_BOX3_EQ(expected, actual)                      \
    EXPECT_VECTOR3_EQ((expected).lower_corner, (actual).lower_corner); \
    EXPECT_VECTOR3_EQ((expected).upper_corner, (actual).upper_corner);

#define EXPECT_BOUNDING_BOX3_NEAR(expected, actual, eps)                      \
    EXPECT_VECTOR3_NEAR((expected).lower_corner, (actual).lower_corner, eps); \
    EXPECT_VECTOR3_NEAR((expected).upper_corner, (actual).upper_corner, eps);

namespace vox {

const Point2D* getSamplePoints2();

size_t getNumberOfSamplePoints2();

const Point3D* getSamplePoints3();

size_t getNumberOfSamplePoints3();

const Vector2D* getSampleDirs2();

size_t getNumberOfSampleDirs2();

const Vector3D* getSampleDirs3();

size_t getNumberOfSampleDirs3();

const char* getCubeTriMesh3x3x3Obj();

const char* getSphereTriMesh5x5Obj();

}  // namespace vox

#endif  // SRC_TESTS_UNIT_TESTS_UNIT_TESTS_UTILS_H_
