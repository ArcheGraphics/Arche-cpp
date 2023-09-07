//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "controls/spherical.h"

namespace vox::control {
Spherical::Spherical(float radius, float phi, float theta) : radius_(radius), phi_(phi), theta_(theta) {}

void Spherical::Set(float radius, float phi, float theta) {
    radius_ = radius;
    phi_ = phi;
    theta_ = theta;
}

void Spherical::MakeSafe() { phi_ = clamp<float>(phi_, kEpsilonF, M_PI - kEpsilonF); }

void Spherical::SetFromVec3(const Vector3F &v_3) {
    radius_ = v_3.length();
    if (radius_ == 0) {
        theta_ = 0;
        phi_ = 0;
    } else {
        theta_ = std::atan2(v_3.x, v_3.z);
        phi_ = std::acos(clamp<float>(v_3.y / radius_, -1, 1));
    }
}

void Spherical::SetToVec3(Vector3F &v_3) const {
    const auto kSinPhiRadius = std::sin(phi_) * radius_;

    v_3.x = kSinPhiRadius * std::sin(theta_);
    v_3.y = std::cos(phi_) * radius_;
    v_3.z = kSinPhiRadius * std::cos(theta_);
}

}// namespace vox::control
