//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "light/point_light.h"

#include "math/matrix_utils.h"
#include "ecs/entity.h"
#include "light/light_manager.h"

namespace vox {
PointLight::PointLight(Entity *entity) : Light(entity) {}

void PointLight::on_enable() { LightManager::get_singleton().attach_point_light(this); }

void PointLight::on_disable() { LightManager::get_singleton().detach_point_light(this); }

void PointLight::update_shader_data(PointLight::PointLightData &shader_data) {
    shader_data.color = Vector3F(color_.r * intensity_, color_.g * intensity_, color_.b * intensity_);
    auto position = get_entity()->transform->get_world_position();
    shader_data.position = Vector3F(position.x, position.y, position.z);
    shader_data.distance = distance_;
}

// MARK: - Shadow
Matrix4x4F PointLight::get_shadow_projection_matrix() { return makePerspective<float>(degreesToRadians(120.f), 1, 0.1, 100); }

}// namespace vox
