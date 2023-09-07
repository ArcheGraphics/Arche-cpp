//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "ecs/scene_manager.h"

#include <utility>

#include "components/camera.h"
#include "ecs/entity.h"
#include "light/direct_light.h"

namespace vox {
SceneManager *SceneManager::get_singleton_ptr() { return ms_singleton; }

SceneManager &SceneManager::get_singleton() {
    assert(ms_singleton);
    return (*ms_singleton);
}

SceneManager::SceneManager(MTL::Device &device, std::string p_scene_root_folder)
    : device_(device), scene_root_folder_(std::move(p_scene_root_folder)) {
    load_empty_scene();
}

SceneManager::~SceneManager() { unload_current_scene(); }

void SceneManager::update() {
    if (delayed_load_call_) {
        delayed_load_call_();
        delayed_load_call_ = nullptr;
    }
}

void SceneManager::load_empty_scene() {
    unload_current_scene();

    current_scene_ = std::make_unique<Scene>(device_);
    current_scene_->process_active(false);
}

void SceneManager::load_empty_lighted_scene() {
    load_empty_scene();

    auto root_entity = current_scene_->create_root_entity();
    auto camera_entity = root_entity->create_child("MainCamera");
    camera_entity->transform->set_position(10, 10, 10);
    camera_entity->transform->look_at(Point3F(0, 0, 0));
    camera_entity->add_component<Camera>();

    // init directional light
    auto light = root_entity->create_child("light");
    light->transform->set_position(0, 3, 0);
    light->add_component<DirectLight>();
}

void SceneManager::unload_current_scene() {
    if (current_scene_) {
        current_scene_.reset();
        current_scene_ = nullptr;
    }

    forget_current_scene_source_path();
}

bool SceneManager::has_current_scene() const { return current_scene_ != nullptr; }

Scene *SceneManager::get_current_scene() { return current_scene_.get(); }

std::string SceneManager::get_current_scene_source_path() const { return current_scene_source_path_; }

bool SceneManager::is_current_scene_loaded_from_disk() const { return current_scene_loaded_from_path_; }

void SceneManager::store_current_scene_source_path(const std::string &p_path) {
    current_scene_source_path_ = p_path;
    current_scene_loaded_from_path_ = true;
}

void SceneManager::forget_current_scene_source_path() {
    current_scene_source_path_ = "";
    current_scene_loaded_from_path_ = false;
}

}// namespace vox
