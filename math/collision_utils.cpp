//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "math/collision_utils.h"

#include "math/ray.h"

namespace vox {

float distancePlaneAndPoint(const BoundingPlane3F &plane, const Point3F &point) {
    return point.dot(plane.normal) + plane.distance;
}

PlaneIntersectionType intersectsPlaneAndPoint(const BoundingPlane3F &plane, const Point3F &point) {
    const auto kDistance = distancePlaneAndPoint(plane, point);
    if (kDistance > 0) {
        return PlaneIntersectionType::Front;
    }
    if (kDistance < 0) {
        return PlaneIntersectionType::Back;
    }
    return PlaneIntersectionType::Intersecting;
}

PlaneIntersectionType intersectsPlaneAndBox(const BoundingPlane3F &plane, const BoundingBox3F &box) {
    const auto &min = box.lower_corner;
    const auto &max = box.upper_corner;
    const auto &normal = plane.normal;
    Point3F front;
    Point3F back;

    if (normal.x >= 0) {
        front.x = max.x;
        back.x = min.x;
    } else {
        front.x = min.x;
        back.x = max.x;
    }
    if (normal.y >= 0) {
        front.y = max.y;
        back.y = min.y;
    } else {
        front.y = min.y;
        back.y = max.y;
    }
    if (normal.z >= 0) {
        front.z = max.z;
        back.z = min.z;
    } else {
        front.z = min.z;
        back.z = max.z;
    }

    if (distancePlaneAndPoint(plane, front) < 0) {
        return PlaneIntersectionType::Back;
    }

    if (distancePlaneAndPoint(plane, back) > 0) {
        return PlaneIntersectionType::Front;
    }

    return PlaneIntersectionType::Intersecting;
}

float intersectsRayAndPlane(const Ray3F &ray, const BoundingPlane3F &plane) {
    const auto &normal = plane.normal;

    const auto kDir = normal.dot(ray.direction);
    // Parallel
    if (std::abs(kDir) < kEpsilonF) {
        return -1;
    }

    const auto kPosition = ray.origin.dot(normal);
    auto distance = (-plane.distance - kPosition) / kDir;

    if (distance < 0) {
        if (distance < -kEpsilonF) {
            return -1;
        }

        distance = 0;
    }

    return distance;
}

float intersectsRayAndBox(const Ray3F &ray, const BoundingBox3F &box) {
    const auto &origin = ray.origin;
    const auto &direction = ray.direction;
    const auto &min = box.lower_corner;
    const auto &max = box.upper_corner;
    const auto &dir_x = direction.x;
    const auto &dir_y = direction.y;
    const auto &dir_z = direction.z;
    const auto &ori_x = origin.x;
    const auto &ori_y = origin.y;
    const auto &ori_z = origin.z;
    float distance = 0;
    float tmax = std::numeric_limits<float>::max();

    if (std::abs(dir_x) < kEpsilonF) {
        if (ori_x < min.x || ori_x > max.x) {
            return -1;
        }
    } else {
        const float kInverse = 1.f / dir_x;
        float t_1 = (min.x - ori_x) * kInverse;
        float t_2 = (max.x - ori_x) * kInverse;

        if (t_1 > t_2) {
            const auto kTemp = t_1;
            t_1 = t_2;
            t_2 = kTemp;
        }

        distance = std::max(t_1, distance);
        tmax = std::min(t_2, tmax);

        if (distance > tmax) {
            return -1;
        }
    }

    if (std::abs(dir_y) < kEpsilonF) {
        if (ori_y < min.y || ori_y > max.y) {
            return -1;
        }
    } else {
        const float kInverse = 1.f / dir_y;
        float t_1 = (min.y - ori_y) * kInverse;
        float t_2 = (max.y - ori_y) * kInverse;

        if (t_1 > t_2) {
            const auto kTemp = t_1;
            t_1 = t_2;
            t_2 = kTemp;
        }

        distance = std::max(t_1, distance);
        tmax = std::min(t_2, tmax);

        if (distance > tmax) {
            return -1;
        }
    }

    if (std::abs(dir_z) < kEpsilonF) {
        if (ori_z < min.z || ori_z > max.z) {
            return -1;
        }
    } else {
        const float kInverse = 1.f / dir_z;
        float t_1 = (min.z - ori_z) * kInverse;
        float t_2 = (max.z - ori_z) * kInverse;

        if (t_1 > t_2) {
            const auto kTemp = t_1;
            t_1 = t_2;
            t_2 = kTemp;
        }

        distance = std::max(t_1, distance);
        tmax = std::min(t_2, tmax);

        if (distance > tmax) {
            return -1;
        }
    }

    return distance;
}

bool intersectsBoxAndBox(const BoundingBox3F &box_a, const BoundingBox3F &box_b) {
    if (box_a.lower_corner.x > box_b.upper_corner.x || box_b.lower_corner.x > box_a.upper_corner.x) {
        return false;
    }

    if (box_a.lower_corner.y > box_b.upper_corner.y || box_b.lower_corner.y > box_a.upper_corner.y) {
        return false;
    }

    return !(box_a.lower_corner.z > box_b.upper_corner.z || box_b.lower_corner.z > box_a.upper_corner.z);
}

bool intersectsFrustumAndBox(const BoundingFrustum &frustum, const BoundingBox3F &box) {
    const auto &min = box.lower_corner;
    const auto &max = box.upper_corner;
    Vector3F back;

    for (int i = 0; i < 6; ++i) {
        const auto kPlane = frustum.getPlane(i);
        const auto &normal = kPlane.normal;

        back.x = normal.x >= 0 ? min.x : max.x;
        back.y = normal.y >= 0 ? min.y : max.y;
        back.z = normal.z >= 0 ? min.z : max.z;
        if (kPlane.normal.dot(back) > -kPlane.distance) {
            return false;
        }
    }

    return true;
}

ContainmentType frustumContainsBox(const BoundingFrustum &frustum, const BoundingBox3F &box) {
    const auto &min = box.lower_corner;
    const auto &max = box.upper_corner;
    Point3F front;
    Point3F back;
    auto result = ContainmentType::Contains;

    for (int i = 0; i < 6; ++i) {
        const auto kPlane = frustum.getPlane(i);
        const auto &normal = kPlane.normal;

        if (normal.x >= 0) {
            front.x = max.x;
            back.x = min.x;
        } else {
            front.x = min.x;
            back.x = max.x;
        }
        if (normal.y >= 0) {
            front.y = max.y;
            back.y = min.y;
        } else {
            front.y = min.y;
            back.y = max.y;
        }
        if (normal.z >= 0) {
            front.z = max.z;
            back.z = min.z;
        } else {
            front.z = min.z;
            back.z = max.z;
        }

        if (intersectsPlaneAndPoint(kPlane, back) == PlaneIntersectionType::Front) {
            return ContainmentType::Disjoint;
        }

        if (intersectsPlaneAndPoint(kPlane, front) == PlaneIntersectionType::Front) {
            result = ContainmentType::Intersects;
        }
    }

    return result;
}

}  // namespace vox
