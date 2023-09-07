//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "light/spot_light.h"

#include "math/matrix_utils.h"
#include "ecs/entity.h"
#include "light/light_manager.h"

namespace vox {
SpotLight::SpotLight(Entity *entity) : Light(entity) {}

void SpotLight::on_enable() { LightManager::get_singleton().attach_spot_light(this); }

void SpotLight::on_disable() { LightManager::get_singleton().detach_spot_light(this); }

void SpotLight::update_shader_data(SpotLightData &shader_data) {
    shader_data.color = Vector3F(color_.r * intensity_, color_.g * intensity_, color_.b * intensity_);
    auto position = get_entity()->transform->get_world_position();
    shader_data.position = Vector3F(position.x, position.y, position.z);
    auto direction = get_entity()->transform->get_world_forward();
    shader_data.direction = Vector3F(direction.x, direction.y, direction.z);
    shader_data.distance = distance_;
    shader_data.angle_cos = std::cos(angle_);
    shader_data.penumbra_cos = std::cos(angle_ + penumbra_);
}

// MARK: - Shadow
Matrix4x4F SpotLight::get_shadow_projection_matrix() {
    const auto kFov = std::min<float>(M_PI / 2, angle_ * 2.f * std::sqrt(2.f));
    return makePerspective<float>(kFov, 1, 0.1, distance_ + 5);
}

}// namespace vox
