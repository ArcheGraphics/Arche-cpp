//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "light/direct_light.h"

#include "ecs/entity.h"
#include "light/light_manager.h"

namespace vox {
DirectLight::DirectLight(Entity *entity) : Light(entity) {}

void DirectLight::on_enable() { LightManager::get_singleton().attach_direct_light(this); }

void DirectLight::on_disable() { LightManager::get_singleton().detach_direct_light(this); }

void DirectLight::update_shader_data(DirectLight::DirectLightData &shader_data) {
    shader_data.color = Vector3F(color_.r * intensity_, color_.g * intensity_, color_.b * intensity_);
    auto direction = get_entity()->transform->get_world_forward();
    shader_data.direction = Vector3F(direction.x, direction.y, direction.z);
}

// MARK: - Shadow
Vector3F DirectLight::get_direction() { return get_entity()->transform->get_world_forward(); }

Matrix4x4F DirectLight::get_shadow_projection_matrix() {
    assert(false && "cascade shadow don't use this projection");
    throw std::exception();
}

}// namespace vox
