//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "ecs/scene.h"

#include <queue>

#include "components/camera.h"
#include "ecs/entity.h"
#include "framework/common/logging.h"

namespace vox {
Scene::Scene(MTL::Device &device) : device(device), shader_data(device) {
    set_ambient_light(std::make_shared<vox::AmbientLight>());
}

Scene::~Scene() { root_entities.clear(); }

MTL::Device &Scene::get_device() { return device; }

const std::shared_ptr<AmbientLight> &Scene::get_ambient_light() const { return ambient_light; }

void Scene::set_ambient_light(const std::shared_ptr<vox::AmbientLight> &light) {
    if (!light) {
        LOGE("The scene must have one ambient light")
        return;
    }

    auto last_ambient_light = ambient_light;
    if (last_ambient_light != light) {
        light->set_scene(this);
        ambient_light = light;
    }
}

size_t Scene::get_root_entities_count() { return root_entities.size(); }

const std::vector<std::unique_ptr<Entity>> &Scene::get_root_entities() const { return root_entities; }

void Scene::play() { process_active(true); }

bool Scene::is_playing() const { return is_active_in_engine; }

Entity *Scene::create_root_entity(const std::string &name) {
    auto entity = std::make_unique<Entity>(name);
    auto entity_ptr = entity.get();
    add_root_entity(std::move(entity));
    return entity_ptr;
}

void Scene::add_root_entity(std::unique_ptr<Entity> &&entity) {
    const auto kIsRoot = entity->is_root;

    // let entity become root
    if (!kIsRoot) {
        entity->is_root = true;
        entity->remove_from_parent();
    }

    // add or remove from scene's root_entities
    Entity *entity_ptr = entity.get();
    const auto kOldScene = entity->scene;
    if (kOldScene != this) {
        if (kOldScene && kIsRoot) {
            kOldScene->remove_entity(entity_ptr);
        }
        Entity::traverse_set_owner_scene(entity_ptr, this);
        root_entities.emplace_back(std::move(entity));
    } else if (!kIsRoot) {
        root_entities.emplace_back(std::move(entity));
    }

    // process entity active/inActive
    if (is_active_in_engine) {
        if (!entity_ptr->_is_active_in_hierarchy && entity_ptr->_is_active) {
            entity_ptr->process_active();
        }
    } else {
        if (entity_ptr->_is_active_in_hierarchy) {
            entity_ptr->process_inactive();
        }
    }
}

void Scene::remove_root_entity(vox::Entity *entity) {
    if (entity->is_root && entity->scene == this) {
        remove_entity(entity);
        if (is_active_in_engine) {
            entity->process_inactive();
        }
        Entity::traverse_set_owner_scene(entity, nullptr);
    }
}

Entity *Scene::get_root_entity(size_t index) { return root_entities[index].get(); }

Entity *Scene::find_entity_by_name(const std::string &name) {
    const auto &children = root_entities;
    for (const auto &child : children) {
        if (child->name == name) {
            return child.get();
        }
    }

    for (const auto &child : children) {
        const auto kEntity = child->find_by_name(name);
        if (kEntity) {
            return kEntity;
        }
    }
    return nullptr;
}

void Scene::attach_render_camera(vox::Camera *camera) {
    auto iter = std::find(active_cameras.begin(), active_cameras.end(), camera);
    if (iter == active_cameras.end()) {
        active_cameras.push_back(camera);
    } else {
        LOGI("Camera already attached.")
    }
}

void Scene::detach_render_camera(vox::Camera *camera) {
    auto iter = std::find(active_cameras.begin(), active_cameras.end(), camera);
    if (iter != active_cameras.end()) {
        active_cameras.erase(iter);
    }
}

void Scene::process_active(bool active) {
    is_active_in_engine = active;
    for (const auto &entity : root_entities) {
        if (entity->_is_active) {
            active ? entity->process_active() : entity->process_inactive();
        }
    }
}

void Scene::remove_entity(vox::Entity *entity) {
    auto &old_root_entities = root_entities;
    old_root_entities.erase(std::remove_if(old_root_entities.begin(), old_root_entities.end(),
                                           [entity](auto &old_entity) { return old_entity.get() == entity; }),
                            old_root_entities.end());
}

// MARK: - Update Loop
void Scene::update_shader_data() {
    // union scene and camera macro.
    for (auto &camera : active_cameras) {
        camera->update();
    }
}

}// namespace vox
