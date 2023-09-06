//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "light.h"
#include "entity.h"

namespace vox {
Light::Light(Entity *entity) : Component(entity) {
}

Matrix4x4F Light::viewMatrix() {
    return entity()->transform->worldMatrix().inverse();
}

Matrix4x4F Light::inverseViewMatrix() {
    return entity()->transform->worldMatrix();
}

//MARK: - Shadow
bool Light::enableShadow() {
    return _enableShadow;
}

void Light::setEnableShadow(bool enabled) {
    _enableShadow = enabled;
}

float Light::shadowBias() {
    return _shadowBias;
}

void Light::setShadowBias(float value) {
    _shadowBias = value;
}

float Light::shadowIntensity() {
    return _shadowIntensity;
}

void Light::setShadowIntensity(float value) {
    _shadowIntensity = value;
}

float Light::shadowRadius() {
    return _shadowRadius;
}

void Light::setShadowRadius(float value) {
    _shadowRadius = value;
}

}// namespace vox
