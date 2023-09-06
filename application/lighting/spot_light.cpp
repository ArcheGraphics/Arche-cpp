//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "spot_light.h"
#include "scene.h"
#include "matrix_utils.h"
#include "light_manager.h"

namespace vox {
SpotLight::SpotLight(Entity *entity) : Light(entity) {
}

void SpotLight::_onEnable() {
    LightManager::getSingleton().attachSpotLight(this);
}

void SpotLight::_onDisable() {
    LightManager::getSingleton().detachSpotLight(this);
}

void SpotLight::_updateShaderData(SpotLightData &shaderData) {
    shaderData.color = simd_make_float3(color.r * intensity, color.g * intensity, color.b * intensity);
    auto position = entity()->transform->worldPosition();
    shaderData.position = simd_make_float3(position.x, position.y, position.z);
    auto direction = entity()->transform->worldForward();
    shaderData.direction = simd_make_float3(direction.x, direction.y, direction.z);
    shaderData.distance = distance;
    shaderData.angleCos = std::cos(angle);
    shaderData.penumbraCos = std::cos(angle + penumbra);
}

// MARK: - Shadow
Matrix4x4F SpotLight::shadowProjectionMatrix() {
    const auto fov = std::min(M_PI / 2, angle * 2 * std::sqrt(2));
    return makepPerspective<float>(fov, 1, 0.1, distance + 5);
}

}// namespace vox
