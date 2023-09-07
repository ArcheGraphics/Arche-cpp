//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "math/vector3.h"

namespace vox::control {
// Spherical.
class Spherical {
public:
    explicit Spherical(float radius = 1.0, float phi = 0, float theta = 0);

    void Set(float radius, float phi, float theta);

    void MakeSafe();

    void SetFromVec3(const Vector3F &v_3);

    void SetToVec3(Vector3F &v_3) const;

private:
    friend class OrbitControl;

    friend class FreeControl;

    float radius_;
    float phi_;
    float theta_;
};

}// namespace vox::control