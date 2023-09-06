//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "unit_tests_utils.h"
#include "math/bounding_box3.h"

using namespace vox;

TEST(BoundingBox3, Constructors) {
    {
        BoundingBox3D box;

        EXPECT_DOUBLE_EQ(kMaxD, box.lower_corner.x);
        EXPECT_DOUBLE_EQ(kMaxD, box.lower_corner.y);
        EXPECT_DOUBLE_EQ(kMaxD, box.lower_corner.z);

        EXPECT_DOUBLE_EQ(-kMaxD, box.upper_corner.x);
        EXPECT_DOUBLE_EQ(-kMaxD, box.upper_corner.y);
        EXPECT_DOUBLE_EQ(-kMaxD, box.upper_corner.z);
    }

    {
        BoundingBox3D box(Point3D(-2.0, 3.0, 5.0), Point3D(4.0, -2.0, 1.0));

        EXPECT_DOUBLE_EQ(-2.0, box.lower_corner.x);
        EXPECT_DOUBLE_EQ(-2.0, box.lower_corner.y);
        EXPECT_DOUBLE_EQ(1.0, box.lower_corner.z);

        EXPECT_DOUBLE_EQ(4.0, box.upper_corner.x);
        EXPECT_DOUBLE_EQ(3.0, box.upper_corner.y);
        EXPECT_DOUBLE_EQ(5.0, box.upper_corner.z);
    }

    {
        BoundingBox3D box(Point3D(-2.0, 3.0, 5.0), Point3D(4.0, -2.0, 1.0));
        BoundingBox3D box2(box);

        EXPECT_DOUBLE_EQ(-2.0, box2.lower_corner.x);
        EXPECT_DOUBLE_EQ(-2.0, box2.lower_corner.y);
        EXPECT_DOUBLE_EQ(1.0, box2.lower_corner.z);

        EXPECT_DOUBLE_EQ(4.0, box2.upper_corner.x);
        EXPECT_DOUBLE_EQ(3.0, box2.upper_corner.y);
        EXPECT_DOUBLE_EQ(5.0, box2.upper_corner.z);
    }
}

TEST(BoundingBox3, BasicGetters) {
    BoundingBox3D box(Point3D(-2.0, 3.0, 5.0), Point3D(4.0, -2.0, 1.0));

    EXPECT_DOUBLE_EQ(6.0, box.width());
    EXPECT_DOUBLE_EQ(5.0, box.height());
    EXPECT_DOUBLE_EQ(4.0, box.depth());
    EXPECT_DOUBLE_EQ(6.0, box.length(0));
    EXPECT_DOUBLE_EQ(5.0, box.length(1));
    EXPECT_DOUBLE_EQ(4.0, box.length(2));
}

TEST(BoundingBox3, Overlaps) {
    // x-axis is not overlapping
    {
        BoundingBox3D box1(Point3D(-2.0, -2.0, 1.0), Point3D(4.0, 3.0, 5.0));
        BoundingBox3D box2(Point3D(5.0, 1.0, 3.0), Point3D(8.0, 2.0, 4.0));

        EXPECT_FALSE(box1.overlaps(box2));
    }

    // y-axis is not overlapping
    {
        BoundingBox3D box1(Point3D(-2.0, -2.0, 1.0), Point3D(4.0, 3.0, 5.0));
        BoundingBox3D box2(Point3D(3.0, 4.0, 3.0), Point3D(8.0, 6.0, 4.0));

        EXPECT_FALSE(box1.overlaps(box2));
    }

    // z-axis is not overlapping
    {
        BoundingBox3D box1(Point3D(-2.0, -2.0, 1.0), Point3D(4.0, 3.0, 5.0));
        BoundingBox3D box2(Point3D(3.0, 1.0, 6.0), Point3D(8.0, 2.0, 9.0));

        EXPECT_FALSE(box1.overlaps(box2));
    }

    // overlapping
    {
        BoundingBox3D box1(Point3D(-2.0, -2.0, 1.0), Point3D(4.0, 3.0, 5.0));
        BoundingBox3D box2(Point3D(3.0, 1.0, 3.0), Point3D(8.0, 2.0, 7.0));

        EXPECT_TRUE(box1.overlaps(box2));
    }
}

TEST(BoundingBox3, Contains) {
    // Not containing (x-axis is out)
    {
        BoundingBox3D box(Point3D(-2.0, -2.0, 1.0), Point3D(4.0, 3.0, 5.0));
        Point3D point(-3.0, 0.0, 4.0);

        EXPECT_FALSE(box.contains(point));
    }

    // Not containing (y-axis is out)
    {
        BoundingBox3D box(Point3D(-2.0, -2.0, 1.0), Point3D(4.0, 3.0, 5.0));
        Point3D point(2.0, 3.5, 4.0);

        EXPECT_FALSE(box.contains(point));
    }

    // Not containing (z-axis is out)
    {
        BoundingBox3D box(Point3D(-2.0, -2.0, 1.0), Point3D(4.0, 3.0, 5.0));
        Point3D point(2.0, 0.0, 0.0);

        EXPECT_FALSE(box.contains(point));
    }

    // Containing
    {
        BoundingBox3D box(Point3D(-2.0, -2.0, 1.0), Point3D(4.0, 3.0, 5.0));
        Point3D point(2.0, 0.0, 4.0);

        EXPECT_TRUE(box.contains(point));
    }
}

TEST(BoundingBox3, Intersects) {
    BoundingBox3D box(Point3D(-2.0, -2.0, 1.0), Point3D(4.0, 3.0, 5.0));

    Ray3D ray1(Point3D(-3, 0, 2), Vector3D(2, 1, 1).normalized());
    EXPECT_TRUE(box.intersects(ray1));

    Ray3D ray2(Point3D(3, -1, 3), Vector3D(-1, 2, -3).normalized());
    EXPECT_TRUE(box.intersects(ray2));

    Ray3D ray3(Point3D(1, -5, 1), Vector3D(2, 1, 2).normalized());
    EXPECT_FALSE(box.intersects(ray3));
}

TEST(BoundingBox3, ClosestIntersection) {
    BoundingBox3D box(Point3D(-2.0, -2.0, -1.0), Point3D(1.0, 0.0, 1.0));

    Ray3D ray1(Point3D(-4, -3, 0), Vector3D(1, 1, 0).normalized());
    BoundingBoxRayIntersection3D intersection1 = box.closestIntersection(ray1);
    EXPECT_TRUE(intersection1.is_intersecting);
    EXPECT_DOUBLE_EQ(Vector3D(2, 2, 0).length(), intersection1.t_near);
    EXPECT_DOUBLE_EQ(Vector3D(3, 3, 0).length(), intersection1.t_far);

    Ray3D ray2(Point3D(0, -1, 0), Vector3D(-2, 1, 1).normalized());
    BoundingBoxRayIntersection3D intersection2 = box.closestIntersection(ray2);
    EXPECT_TRUE(intersection2.is_intersecting);
    EXPECT_DOUBLE_EQ(Vector3D(2, 1, 1).length(), intersection2.t_near);
}

TEST(BoundingBox3, MidPoint) {
    BoundingBox3D box(Point3D(-2.0, -2.0, 1.0), Point3D(4.0, 3.0, 5.0));
    Point3D midPoint = box.midPoint();

    EXPECT_DOUBLE_EQ(1.0, midPoint.x);
    EXPECT_DOUBLE_EQ(0.5, midPoint.y);
    EXPECT_DOUBLE_EQ(3.0, midPoint.z);
}

TEST(BoundingBox3, DiagonalLength) {
    BoundingBox3D box(Point3D(-2.0, -2.0, 1.0), Point3D(4.0, 3.0, 5.0));
    double diagLen = box.diagonalLength();

    EXPECT_DOUBLE_EQ(std::sqrt(6.0 * 6.0 + 5.0 * 5.0 + 4.0 * 4.0), diagLen);
}

TEST(BoundingBox3, DiagonalLengthSquared) {
    BoundingBox3D box(Point3D(-2.0, -2.0, 1.0), Point3D(4.0, 3.0, 5.0));
    double diagLenSqr = box.diagonalLengthSquared();

    EXPECT_DOUBLE_EQ(6.0 * 6.0 + 5.0 * 5.0 + 4.0 * 4.0, diagLenSqr);
}

TEST(BoundingBox3, Reset) {
    BoundingBox3D box(Point3D(-2.0, -2.0, 1.0), Point3D(4.0, 3.0, 5.0));
    box.reset();

    static const double maxDouble = std::numeric_limits<double>::max();

    EXPECT_DOUBLE_EQ(maxDouble, box.lower_corner.x);
    EXPECT_DOUBLE_EQ(maxDouble, box.lower_corner.y);
    EXPECT_DOUBLE_EQ(maxDouble, box.lower_corner.z);

    EXPECT_DOUBLE_EQ(-maxDouble, box.upper_corner.x);
    EXPECT_DOUBLE_EQ(-maxDouble, box.upper_corner.y);
    EXPECT_DOUBLE_EQ(-maxDouble, box.upper_corner.z);
}

TEST(BoundingBox3, Merge) {
    // Merge with point
    {
        BoundingBox3D box(Point3D(-2.0, -2.0, 1.0), Point3D(4.0, 3.0, 5.0));
        Point3D point(5.0, 1.0, -1.0);

        box.merge(point);

        EXPECT_DOUBLE_EQ(-2.0, box.lower_corner.x);
        EXPECT_DOUBLE_EQ(-2.0, box.lower_corner.y);
        EXPECT_DOUBLE_EQ(-1.0, box.lower_corner.z);

        EXPECT_DOUBLE_EQ(5.0, box.upper_corner.x);
        EXPECT_DOUBLE_EQ(3.0, box.upper_corner.y);
        EXPECT_DOUBLE_EQ(5.0, box.upper_corner.z);
    }

    // Merge with other box
    {
        BoundingBox3D box1(Point3D(-2.0, -2.0, 1.0), Point3D(4.0, 3.0, 5.0));
        BoundingBox3D box2(Point3D(3.0, 1.0, 3.0), Point3D(8.0, 2.0, 7.0));

        box1.merge(box2);

        EXPECT_DOUBLE_EQ(-2.0, box1.lower_corner.x);
        EXPECT_DOUBLE_EQ(-2.0, box1.lower_corner.y);
        EXPECT_DOUBLE_EQ(1.0, box1.lower_corner.z);

        EXPECT_DOUBLE_EQ(8.0, box1.upper_corner.x);
        EXPECT_DOUBLE_EQ(3.0, box1.upper_corner.y);
        EXPECT_DOUBLE_EQ(7.0, box1.upper_corner.z);
    }
}

TEST(BoundingBox3, Expand) {
    BoundingBox3D box(Point3D(-2.0, -2.0, 1.0), Point3D(4.0, 3.0, 5.0));
    box.expand(3.0);

    EXPECT_DOUBLE_EQ(-5.0, box.lower_corner.x);
    EXPECT_DOUBLE_EQ(-5.0, box.lower_corner.y);
    EXPECT_DOUBLE_EQ(-2.0, box.lower_corner.z);

    EXPECT_DOUBLE_EQ(7.0, box.upper_corner.x);
    EXPECT_DOUBLE_EQ(6.0, box.upper_corner.y);
    EXPECT_DOUBLE_EQ(8.0, box.upper_corner.z);
}

TEST(BoundingBox3, Corner) {
    BoundingBox3D box(Point3D(-2.0, -2.0, 1.0), Point3D(4.0, 3.0, 5.0));

    EXPECT_VECTOR3_EQ(Point3D(-2.0, -2.0, 1.0), box.corner(0));
    EXPECT_VECTOR3_EQ(Point3D(4.0, -2.0, 1.0), box.corner(1));
    EXPECT_VECTOR3_EQ(Point3D(-2.0, 3.0, 1.0), box.corner(2));
    EXPECT_VECTOR3_EQ(Point3D(4.0, 3.0, 1.0), box.corner(3));
    EXPECT_VECTOR3_EQ(Point3D(-2.0, -2.0, 5.0), box.corner(4));
    EXPECT_VECTOR3_EQ(Point3D(4.0, -2.0, 5.0), box.corner(5));
    EXPECT_VECTOR3_EQ(Point3D(-2.0, 3.0, 5.0), box.corner(6));
    EXPECT_VECTOR3_EQ(Point3D(4.0, 3.0, 5.0), box.corner(7));
}

TEST(BoundingBox3D, IsEmpty) {
    BoundingBox3D box(Point3D(-2.0, -2.0, 1.0), Point3D(4.0, 3.0, 5.0));
    EXPECT_FALSE(box.isEmpty());

    box.lower_corner = Point3D(5.0, 1.0, 3.0);
    EXPECT_TRUE(box.isEmpty());

    box.lower_corner = Point3D(2.0, 4.0, 3.0);
    EXPECT_TRUE(box.isEmpty());

    box.lower_corner = Point3D(2.0, 1.0, 6.0);
    EXPECT_TRUE(box.isEmpty());

    box.lower_corner = Point3D(4.0, 1.0, 3.0);
    EXPECT_TRUE(box.isEmpty());
}

// MARK: -
TEST(BoundingBox3D, transform) {
    auto box = BoundingBox3D(Point3D(-1, -1, -1), Point3D(1, 1, 1));
    const auto matrix = Matrix4x4D(2, 0, 0, 0, 0, 2, 0, 0, 0, 0, 2, 0, 1, 0.5, -1, 1);
    box.transform(matrix);

    const auto newMin = Point3D(-1, -1.5, -3);
    const auto newMax = Point3D(3, 2.5, 1);
    EXPECT_VECTOR3_EQ(box.lower_corner, Point3D(newMin.x, newMin.y, newMin.z));
    EXPECT_VECTOR3_EQ(box.upper_corner, Point3D(newMax.x, newMax.y, newMax.z));
}

TEST(BoundingBox3D, merge) {
    auto box1 = BoundingBox3D(Point3D(-1, -1, -1), Point3D(2, 2, 2));
    const auto box2 = BoundingBox3D(Point3D(-2, -0.5, -2), Point3D(3, 0, 3));
    box1.merge(box2);
    EXPECT_VECTOR3_EQ(box1.lower_corner, Point3D(-2, -1, -2));
    EXPECT_VECTOR3_EQ(box1.upper_corner, Point3D(3, 2, 3));
}

TEST(BoundingBox3D, getCenter) {
    const auto box = BoundingBox3D(Point3D(-1, -1, -1), Point3D(3, 3, 3));
    const auto center = box.midPoint();
    EXPECT_VECTOR3_EQ(center, Point3D(1, 1, 1));
}

TEST(BoundingBox3D, getExtent) {
    const auto box = BoundingBox3D(Point3D(-1, -1, -1), Point3D(3, 3, 3));
    const auto extent = box.extent();
    EXPECT_VECTOR3_EQ(extent, Point3D(2, 2, 2));
}
