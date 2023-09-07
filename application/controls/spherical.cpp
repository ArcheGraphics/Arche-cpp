//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "spherical.h"
#include "math/constants.h"

namespace vox {
namespace control {
Spherical::Spherical(float radius, float phi, float theta) : _radius(radius),
                                                             _phi(phi),
                                                             _theta(theta) {
}

void Spherical::set(float radius, float phi, float theta) {
    _radius = radius;
    _phi = phi;
    _theta = theta;
}

void Spherical::makeSafe() {
    _phi = clamp<float>(_phi, kEpsilonF, M_PI - kEpsilonF);
}

void Spherical::setFromVec3(const Vector3F &v3) {
    _radius = v3.length();
    if (_radius == 0) {
        _theta = 0;
        _phi = 0;
    } else {
        _theta = std::atan2(v3.x, v3.z);
        _phi = std::acos(clamp<float>(v3.y / _radius, -1, 1));
    }
}

void Spherical::setToVec3(Vector3F &v3) {
    const auto sinPhiRadius = std::sin(_phi) * _radius;

    v3.x = sinPhiRadius * std::sin(_theta);
    v3.y = std::cos(_phi) * _radius;
    v3.z = sinPhiRadius * std::cos(_theta);
}

}
}// namespace vox::control
