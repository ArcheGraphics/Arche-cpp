//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "unit_tests_utils.h"
#include "math/bounding_frustum.h"

using namespace vox;

class BoundingFrustumTest : public testing::Test {
public:
    void SetUp() override {
        viewMatrix = Matrix4x4F(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, -20, 1);
        projectionMatrix = Matrix4x4F(0.03954802080988884, 0, 0, 0, 0, 0.10000000149011612, 0, 0, 0, 0,
                                      -0.0200200192630291, 0, -0, -0, -1.0020020008087158, 1);
        vpMatrix = projectionMatrix * viewMatrix;
        frustum = BoundingFrustum(vpMatrix);
    }

    Matrix4x4F viewMatrix;
    Matrix4x4F projectionMatrix;
    Matrix4x4F vpMatrix;
    BoundingFrustum frustum;
};

TEST_F(BoundingFrustumTest, intersectsBox) {
    const auto box1 = BoundingBox3F(Point3F(-2, -2, -2), Point3F(2, 2, 2));
    const auto flag1 = frustum.intersectsBox(box1);
    EXPECT_EQ(flag1, true);

    const auto box2 = BoundingBox3F(Point3F(-32, -2, -2), Point3F(-28, 2, 2));
    const auto flag2 = frustum.intersectsBox(box2);
    EXPECT_EQ(flag2, false);
}

TEST_F(BoundingFrustumTest, calculateFromMatrix) {
    auto a = BoundingFrustum();
    a.calculateFromMatrix(vpMatrix);

    for (int i = 0; i < 6; ++i) {
        const auto aPlane = a.getPlane(i);
        const auto bPlane = frustum.getPlane(i);

        EXPECT_EQ(aPlane.distance, bPlane.distance);
        EXPECT_EQ(aPlane.normal, bPlane.normal);
    }
}

TEST(BoundingPlane, Constructor) {
    const auto point1 = Point3F(0, 1, 0);
    const auto point2 = Point3F(0, 1, 1);
    const auto point3 = Point3F(1, 1, 0);
    auto plane1 = BoundingPlane3F(point1, point2, point3);
    auto plane2 = BoundingPlane3F(Vector3F(0, 1, 0), -1);

    EXPECT_EQ(plane1.distance, plane2.distance);
    plane1.normalize();
    plane2.normalize();
    EXPECT_EQ(plane1.normal, plane2.normal);
}
