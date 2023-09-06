//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "math/bounding_frustum.h"

#include "math/collision_utils.h"

namespace vox {
BoundingFrustum::BoundingFrustum() = default;

BoundingFrustum::BoundingFrustum(const Matrix4x4F &matrix) {
    near = BoundingPlane3F();
    far = BoundingPlane3F();
    left = BoundingPlane3F();
    right = BoundingPlane3F();
    top = BoundingPlane3F();
    bottom = BoundingPlane3F();
    calculateFromMatrix(matrix);
}

BoundingPlane3F BoundingFrustum::getPlane(int index) const {
    switch (index) {
        case 0:
            return near;
        case 1:
            return far;
        case 2:
            return left;
        case 3:
            return right;
        case 4:
            return top;
        case 5:
            return bottom;
        default:
            assert(false && "out of bounds");
    }
}

void BoundingFrustum::calculateFromMatrix(const Matrix4x4F &matrix) {
    const auto &m_11 = matrix[0];
    const auto &m_12 = matrix[1];
    const auto &m_13 = matrix[2];
    const auto &m_14 = matrix[3];
    const auto &m_21 = matrix[4];
    const auto &m_22 = matrix[5];
    const auto &m_23 = matrix[6];
    const auto &m_24 = matrix[7];
    const auto &m_31 = matrix[8];
    const auto &m_32 = matrix[9];
    const auto &m_33 = matrix[10];
    const auto &m_34 = matrix[11];
    const auto &m_41 = matrix[12];
    const auto &m_42 = matrix[13];
    const auto &m_43 = matrix[14];
    const auto &m_44 = matrix[15];

    // near
    auto &near_normal = near.normal;
    near_normal.x = -m_14 - m_13;
    near_normal.y = -m_24 - m_23;
    near_normal.z = -m_34 - m_33;
    near.distance = -m_44 - m_43;
    near.normalize();

    // far
    auto &far_normal = far.normal;
    far_normal.x = m_13 - m_14;
    far_normal.y = m_23 - m_24;
    far_normal.z = m_33 - m_34;
    far.distance = m_43 - m_44;
    far.normalize();

    // left
    auto &left_normal = left.normal;
    left_normal.x = -m_14 - m_11;
    left_normal.y = -m_24 - m_21;
    left_normal.z = -m_34 - m_31;
    left.distance = -m_44 - m_41;
    left.normalize();

    // right
    auto &right_normal = right.normal;
    right_normal.x = m_11 - m_14;
    right_normal.y = m_21 - m_24;
    right_normal.z = m_31 - m_34;
    right.distance = m_41 - m_44;
    right.normalize();

    // top
    auto &top_normal = top.normal;
    top_normal.x = m_12 - m_14;
    top_normal.y = m_22 - m_24;
    top_normal.z = m_32 - m_34;
    top.distance = m_42 - m_44;
    top.normalize();

    // bottom
    auto &bottom_normal = bottom.normal;
    bottom_normal.x = -m_14 - m_12;
    bottom_normal.y = -m_24 - m_22;
    bottom_normal.z = -m_34 - m_32;
    bottom.distance = -m_44 - m_42;
    bottom.normalize();
}

bool BoundingFrustum::intersectsBox(const BoundingBox3F &box) const { return intersectsFrustumAndBox(*this, box); }

}  // namespace vox
