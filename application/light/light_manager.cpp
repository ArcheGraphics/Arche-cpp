//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "light/light_manager.h"

#include "components/camera.h"
#include "ecs/scene.h"
#include "shader/internal_variant_name.h"
#include "shader/shader_manager.h"

namespace vox {
LightManager *LightManager::get_singleton_ptr() { return ms_singleton; }

LightManager &LightManager::get_singleton() {
    assert(ms_singleton);
    return (*ms_singleton);
}

LightManager::LightManager(Scene *scene)
    : scene_(scene),
      point_light_property_("pointLight"),
      spot_light_property_("spotLight"),
      direct_light_property_("directLight") {
}

// MARK: - Point Light
void LightManager::attach_point_light(PointLight *light) {
    auto iter = std::find(point_lights_.begin(), point_lights_.end(), light);
    if (iter == point_lights_.end()) {
        point_lights_.push_back(light);
    } else {
        LOGE("Light already attached.")
    }
}

void LightManager::detach_point_light(PointLight *light) {
    auto iter = std::find(point_lights_.begin(), point_lights_.end(), light);
    if (iter != point_lights_.end()) {
        point_lights_.erase(iter);
    }
}

const std::vector<PointLight *> &LightManager::get_point_lights() const { return point_lights_; }

// MARK: - Spot Light
void LightManager::attach_spot_light(SpotLight *light) {
    auto iter = std::find(spot_lights_.begin(), spot_lights_.end(), light);
    if (iter == spot_lights_.end()) {
        spot_lights_.push_back(light);
    } else {
        LOGE("Light already attached.")
    }
}

void LightManager::detach_spot_light(SpotLight *light) {
    auto iter = std::find(spot_lights_.begin(), spot_lights_.end(), light);
    if (iter != spot_lights_.end()) {
        spot_lights_.erase(iter);
    }
}

const std::vector<SpotLight *> &LightManager::get_spot_lights() const { return spot_lights_; }

// MARK: - Direct Light
void LightManager::attach_direct_light(DirectLight *light) {
    auto iter = std::find(direct_lights_.begin(), direct_lights_.end(), light);
    if (iter == direct_lights_.end()) {
        direct_lights_.push_back(light);
    } else {
        LOGE("Light already attached.")
    }
}

void LightManager::detach_direct_light(DirectLight *light) {
    auto iter = std::find(direct_lights_.begin(), direct_lights_.end(), light);
    if (iter != direct_lights_.end()) {
        direct_lights_.erase(iter);
    }
}

const std::vector<DirectLight *> &LightManager::get_direct_lights() const { return direct_lights_; }

void LightManager::update_shader_data(ShaderData &shader_data) {
    size_t point_light_count = point_lights_.size();
    point_light_datas_.resize(point_light_count);
    size_t spot_light_count = spot_lights_.size();
    spot_light_datas_.resize(spot_light_count);
    size_t direct_light_count = direct_lights_.size();
    direct_light_datas_.resize(direct_light_count);

    for (size_t i = 0; i < point_light_count; i++) {
        point_lights_[i]->update_shader_data(point_light_datas_[i]);
    }

    for (size_t i = 0; i < spot_light_count; i++) {
        spot_lights_[i]->update_shader_data(spot_light_datas_[i]);
    }

    for (size_t i = 0; i < direct_light_count; i++) {
        direct_lights_[i]->update_shader_data(direct_light_datas_[i]);
    }

    if (direct_light_count) {
        shader_data.add_define(DIRECT_LIGHT_COUNT + std::to_string(direct_light_count));
        shader_data.set_data(LightManager::direct_light_property_, direct_light_datas_);
    } else {
        shader_data.remove_define(DIRECT_LIGHT_COUNT);
    }

    if (point_light_count) {
        shader_data.add_define(POINT_LIGHT_COUNT + std::to_string(point_light_count));
        shader_data.set_data(LightManager::point_light_property_, point_light_datas_);
    } else {
        shader_data.remove_define(POINT_LIGHT_COUNT);
    }

    if (spot_light_count) {
        shader_data.add_define(SPOT_LIGHT_COUNT + std::to_string(spot_light_count));
        shader_data.set_data(LightManager::spot_light_property_, spot_light_datas_);
    } else {
        shader_data.remove_define(SPOT_LIGHT_COUNT);
    }
}

}// namespace vox
