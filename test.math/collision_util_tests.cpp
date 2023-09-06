//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "unit_tests_utils.h"
#include "math/bounding_frustum.h"
#include "math/collision_utils.h"
#include "math/ray.h"

using namespace vox;

class CollisionUtilTest : public testing::Test {
public:
    void SetUp() override {
        plane = BoundingPlane3F(Vector3F(0, 1, 0), -5);
        viewMatrix = Matrix4x4F(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, -20, 1);
        projectionMatrix = Matrix4x4F(0.03954802080988884, 0, 0, 0, 0, 0.10000000149011612, 0, 0, 0, 0,
                                      -0.0200200192630291, 0, -0, -0, -1.0020020008087158, 1);
        vpMatrix = projectionMatrix * viewMatrix;
        frustum = BoundingFrustum(vpMatrix);
    }

    BoundingPlane3F plane;
    Matrix4x4F viewMatrix;
    Matrix4x4F projectionMatrix;
    Matrix4x4F vpMatrix;
    BoundingFrustum frustum;
};

TEST_F(CollisionUtilTest, distancePlaneAndPoint) {
    const auto point = Point3F(0, 10, 0);

    const auto distance = distancePlaneAndPoint(plane, point);
    EXPECT_FLOAT_EQ(distance, 5);
}

TEST_F(CollisionUtilTest, intersectsPlaneAndPoint) {
    const auto point1 = Point3F(0, 10, 0);
    const auto point2 = Point3F(2, 5, -9);
    const auto point3 = Point3F(0, 3, 0);

    const auto intersection1 = intersectsPlaneAndPoint(plane, point1);
    const auto intersection2 = intersectsPlaneAndPoint(plane, point2);
    const auto intersection3 = intersectsPlaneAndPoint(plane, point3);
    EXPECT_EQ(intersection1, PlaneIntersectionType::Front);
    EXPECT_EQ(intersection2, PlaneIntersectionType::Intersecting);
    EXPECT_EQ(intersection3, PlaneIntersectionType::Back);
}

TEST_F(CollisionUtilTest, intersectsPlaneAndBox) {
    const auto box1 = BoundingBox3F(Point3F(-1, 6, -2), Point3F(1, 10, 3));
    const auto box2 = BoundingBox3F(Point3F(-1, 5, -2), Point3F(1, 10, 3));
    const auto box3 = BoundingBox3F(Point3F(-1, 4, -2), Point3F(1, 5, 3));
    const auto box4 = BoundingBox3F(Point3F(-1, -5, -2), Point3F(1, 4.9, 3));

    const auto intersection1 = intersectsPlaneAndBox(plane, box1);
    const auto intersection2 = intersectsPlaneAndBox(plane, box2);
    const auto intersection3 = intersectsPlaneAndBox(plane, box3);
    const auto intersection4 = intersectsPlaneAndBox(plane, box4);
    EXPECT_EQ(intersection1, PlaneIntersectionType::Front);
    EXPECT_EQ(intersection2, PlaneIntersectionType::Intersecting);
    EXPECT_EQ(intersection3, PlaneIntersectionType::Intersecting);
    EXPECT_EQ(intersection4, PlaneIntersectionType::Back);
}

TEST_F(CollisionUtilTest, intersectsRayAndPlane) {
    const auto ray1 = Ray3F(Point3F(0, 0, 0), Vector3F(0, 1, 0));
    const auto ray2 = Ray3F(Point3F(0, 0, 0), Vector3F(0, -1, 0));

    const auto distance1 = intersectsRayAndPlane(ray1, plane);
    const auto distance2 = intersectsRayAndPlane(ray2, plane);
    EXPECT_EQ(distance1, 5);
    EXPECT_EQ(distance2, -1);
}

TEST_F(CollisionUtilTest, intersectsRayAndBox) {
    const auto ray = Ray3F(Point3F(0, 0, 0), Vector3F(0, 1, 0));
    const auto box1 = BoundingBox3F(Point3F(-1, 3, -1), Point3F(2, 8, 3));
    const auto box2 = BoundingBox3F(Point3F(1, 1, 1), Point3F(2, 2, 2));

    const auto distance1 = intersectsRayAndBox(ray, box1);
    const auto distance2 = intersectsRayAndBox(ray, box2);
    EXPECT_EQ(distance1, 3);
    EXPECT_EQ(distance2, -1);
}

TEST_F(CollisionUtilTest, intersectsFrustumAndBox) {
    const auto box1 = BoundingBox3F(Point3F(-2, -2, -2), Point3F(2, 2, 2));
    const auto flag1 = frustum.intersectsBox(box1);
    EXPECT_EQ(flag1, true);

    const auto box2 = BoundingBox3F(Point3F(-32, -2, -2), Point3F(-28, 2, 2));
    const auto flag2 = frustum.intersectsBox(box2);
    EXPECT_EQ(flag2, false);
}

TEST_F(CollisionUtilTest, frustumContainsBox) {
    const auto box1 = BoundingBox3F(Point3F(-2, -2, -2), Point3F(2, 2, 2));
    const auto box2 = BoundingBox3F(Point3F(-32, -2, -2), Point3F(-28, 2, 2));
    const auto box3 = BoundingBox3F(Point3F(-35, -2, -2), Point3F(-18, 2, 2));

    const auto expected1 = frustumContainsBox(frustum, box1);
    const auto expected2 = frustumContainsBox(frustum, box2);
    const auto expected3 = frustumContainsBox(frustum, box3);
    EXPECT_EQ(expected1, ContainmentType::Contains);
    EXPECT_EQ(expected2, ContainmentType::Disjoint);
    EXPECT_EQ(expected3, ContainmentType::Intersects);
}
