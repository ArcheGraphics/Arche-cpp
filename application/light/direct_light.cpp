//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "direct_light.h"
#include "ecs/scene.h"
#include "light_manager.h"

namespace vox {
DirectLight::DirectLight(Entity *entity) : Light(entity) {
}

void DirectLight::_onEnable() {
    LightManager::getSingleton().attachDirectLight(this);
}

void DirectLight::_onDisable() {
    LightManager::getSingleton().detachDirectLight(this);
}

void DirectLight::_updateShaderData(DirectLightData &shaderData) {
    shaderData.color = simd_make_float3(color.r * intensity, color.g * intensity, color.b * intensity);
    auto direction = entity()->transform->worldForward();
    shaderData.direction = simd_make_float3(direction.x, direction.y, direction.z);
}

//MARK: - Shadow
Vector3F DirectLight::direction() {
    return entity()->transform->worldForward();
}

Matrix4x4F DirectLight::shadowProjectionMatrix() {
    assert(false && "cascade shadow don't use this projection");
    throw std::exception();
}

}// namespace vox
