//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "ecs/entity.h"

#include <utility>

#include "ecs/component.h"
#include "ecs/components_manager.h"
#include "ecs/scene.h"
#include "components/script.h"

namespace vox {
Entity *Entity::find_child_by_name(vox::Entity *root, const std::string &name) {
    const auto &children = root->children;
    for (const auto &child : children) {
        if (child->name == name) {
            return child.get();
        }
    }
    return nullptr;
}

void Entity::traverse_set_owner_scene(vox::Entity *entity, vox::Scene *scene) {
    entity->scene = scene;
    const auto &children = entity->children;
    for (size_t i = 0; i < entity->children.size(); i++) {
        traverse_set_owner_scene(children[i].get(), scene);
    }
}

Entity::Entity(std::string name) : name(std::move(name)) {
    transform = add_component<Transform>();
    inverse_world_mat_flag = transform->register_world_change_flag();
}

Entity::~Entity() {
    if (parent) {
        LOGE("use RemoveChild instead!")
    }

    for (auto &i : children) {
        remove_child(i.get());
    }
    children.clear();

    for (auto &component : components) {
        remove_component(component.get());
    }
    components.clear();
}

bool Entity::is_active() const { return _is_active; }

void Entity::set_is_active(bool value) {
    if (value != _is_active) {
        _is_active = value;
        if (value) {
            if ((parent != nullptr && parent->_is_active_in_hierarchy) || (is_root)) {
                process_active();
            }
        } else {
            if (_is_active_in_hierarchy) {
                process_inactive();
            }
        }
    }
}

bool Entity::is_active_in_hierarchy() const { return _is_active_in_hierarchy; }

Entity *Entity::get_parent() { return parent; }

const std::vector<std::unique_ptr<Entity>> &Entity::get_children() const { return children; }

size_t Entity::get_child_count() { return children.size(); }

Scene *Entity::get_scene() { return scene; }

void Entity::add_child(std::unique_ptr<Entity> &&child) {
    if (child->parent != this) {
        child->remove_from_parent();
        child->parent = this;

        if (child->scene != scene) {
            Entity::traverse_set_owner_scene(child.get(), scene);
        }

        if (_is_active_in_hierarchy) {
            if (!child->_is_active_in_hierarchy && child->_is_active) {
                child->process_active();
            }
        } else {
            if (child->_is_active_in_hierarchy) {
                child->process_inactive();
            }
        }
        child->set_transform_dirty();
        children.emplace_back(std::move(child));
    }
}

std::unique_ptr<Entity> Entity::remove_child(vox::Entity *child) {
    if (child->parent == this) {
        auto mem = child->remove_from_parent();
        if (child->_is_active_in_hierarchy) {
            child->process_inactive();
        }
        Entity::traverse_set_owner_scene(child, nullptr);
        child->set_transform_dirty();
        return mem;
    } else {
        return nullptr;
    }
}

Entity *Entity::get_child(int index) { return children[index].get(); }

Entity *Entity::find_by_name(const std::string &name) {
    const auto kChild = Entity::find_child_by_name(this, name);
    if (kChild) return kChild;
    for (const auto &child : children) {
        const auto kGrandson = child->find_by_name(name);
        if (kGrandson) {
            return kGrandson;
        }
    }
    return nullptr;
}

Entity *Entity::create_child(const std::string &name) {
    auto child = std::make_unique<Entity>(name);
    auto child_ptr = child.get();
    child->layer = layer;
    add_child(std::move(child));
    return child_ptr;
}

void Entity::clear_children() {
    for (auto &child : children) {
        child->parent = nullptr;
        if (child->_is_active_in_hierarchy) {
            child->process_inactive();
        }
        Entity::traverse_set_owner_scene(child.get(), nullptr);// Must after child.process_in_active().
    }
    children.clear();
}

void Entity::remove_component(vox::Component *component) {
    // ComponentsDependencies._removeCheck(this, component.constructor as any);
    components.erase(std::remove_if(components.begin(), components.end(),
                                    [&](const std::unique_ptr<Component> &x) { return x.get() == component; }),
                     components.end());
}

std::unique_ptr<Entity> Entity::clone() {
    auto clone_entity = std::make_unique<Entity>(name);

    clone_entity->_is_active = _is_active;
    clone_entity->transform->set_local_matrix(transform->get_local_matrix());

    for (size_t i = 0, len = children.size(); i < len; i++) {
        const auto &child = children[i];
        clone_entity->add_child(child->clone());
    }

    for (const auto &source_comp : components) {
        if (!(dynamic_cast<Transform *>(source_comp.get()))) {
            // TODO
        }
    }

    return clone_entity;
}

void Entity::add_script(vox::Script *script) {
    auto iter = std::find(scripts.begin(), scripts.end(), script);
    if (iter == scripts.end()) {
        scripts.push_back(script);
    } else {
        LOGE("Script already attached.")
    }
}

void Entity::remove_script(vox::Script *script) {
    auto iter = std::find(scripts.begin(), scripts.end(), script);
    if (iter != scripts.end()) {
        scripts.erase(iter);
    }
}

std::unique_ptr<Entity> Entity::remove_from_parent() {
    std::unique_ptr<Entity> mem{nullptr};
    if (parent != nullptr) {
        auto &old_parent_children = parent->children;
        auto iter = std::find_if(old_parent_children.begin(), old_parent_children.end(),
                                 [&](const auto &child) { return child.get() == this; });
        mem = std::move(*iter);
        parent = nullptr;
    }
    return mem;
}

void Entity::process_active() {
    active_changed_components = ComponentsManager::get_singleton().get_active_changed_temp_list();
    set_active_in_hierarchy(active_changed_components);
    set_active_components(true);
}

void Entity::process_inactive() {
    active_changed_components = ComponentsManager::get_singleton().get_active_changed_temp_list();
    set_inactive_in_hierarchy(active_changed_components);
    set_active_components(false);
}

void Entity::set_active_components(bool is_active) {
    for (size_t i = 0, length = active_changed_components.size(); i < length; ++i) {
        active_changed_components[i]->set_active(is_active);
    }
    ComponentsManager::get_singleton().put_active_changed_temp_list(active_changed_components);
    active_changed_components.clear();
}

void Entity::set_active_in_hierarchy(std::vector<Component *> &active_changed_components) {
    _is_active_in_hierarchy = true;
    for (auto &component : components) {
        active_changed_components.push_back(component.get());
    }
    for (const auto &child : children) {
        if (child->is_active()) {
            child->set_active_in_hierarchy(active_changed_components);
        }
    }
}

void Entity::set_inactive_in_hierarchy(std::vector<Component *> &active_changed_components) {
    _is_active_in_hierarchy = false;
    for (auto &component : components) {
        active_changed_components.push_back(component.get());
    }
    for (auto &child : children) {
        if (child->is_active()) {
            child->set_inactive_in_hierarchy(active_changed_components);
        }
    }
}

void Entity::set_transform_dirty() {
    if (transform) {
        transform->parent_change();
    } else {
        for (auto &i : children) {
            i->set_transform_dirty();
        }
    }
}

std::vector<Script *> Entity::get_scripts() { return scripts; }

}// namespace vox
