//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <memory>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include "base/layer.h"
#include "base/update_flag.h"
#include "components/transform.h"

namespace vox {
class Component;
class Scene;
class Script;

/// @brief A leaf of the tree structure which can have children and a single parent.
struct Entity final {
public:
    /** The name of entity. */
    std::string name;
    /** The layer the entity belongs to. */
    int layer = Layer::Layer0;
    /** Transform component. */
    Transform *transform;

    /**
     * Create a entity.
     */
    explicit Entity(std::string name = "");

    ~Entity();

    /**
     * Whether to activate locally.
     */
    [[nodiscard]] bool is_active() const;

    void set_is_active(bool value);

    /**
     * Whether it is active in the hierarchy.
     */
    [[nodiscard]] bool is_active_in_hierarchy() const;

    /**
     * The parent entity.
     */
    Entity *get_parent();

    /**
     * The children entities
     */
    [[nodiscard]] const std::vector<std::unique_ptr<Entity>> &get_children() const;

    /**
     * Number of the children entities
     */
    size_t get_child_count();

    /**
     * The scene the entity belongs to.
     */
    Scene *get_scene();

    /**
     * Add component based on the component type.
     * @returns    The component which has been added.
     */
    template<typename T, typename... Args>
    T *add_component(Args &&...args) {
        // ComponentsDependencies._addCheck(this, type);
        auto component = std::make_unique<T>(this, args...);
        T *component_ptr = component.get();
        components.emplace_back(std::move(component));
        if (_is_active_in_hierarchy) {
            component_ptr->set_active(true);
        }
        return component_ptr;
    }

    /**
     * Get component which match the type.
     * @returns    The first component which match type.
     */
    template<typename T>
    T *get_component() {
        for (auto &_component : components) {
            T *component = dynamic_cast<T *>(_component.get());
            if (component) {
                return component;
            }
        }
        return nullptr;
    }

    /**
     * Get components which match the type.
     * @returns    The components which match type.
     */
    template<typename T>
    std::vector<T *> get_components() {
        std::vector<T *> results;
        for (auto &_component : components) {
            T *component = dynamic_cast<T *>(_component.get());
            if (component) {
                results.push_back(component);
            }
        }
        return results;
    }

    /**
     * Get the components which match the type of the entity and it's children.
     * @returns    The components collection which match the type.
     */
    template<typename T>
    std::vector<T *> get_components_include_children() {
        std::vector<T *> results;
        get_components_in_children<T>(results);
        return results;
    }

    /**
     * Add child entity.
     * @param child - The child entity which want to be added.
     */
    void add_child(std::unique_ptr<Entity> &&child);

    /**
     * Remove child entity.
     * @param child - The child entity which want to be removed.
     */
    std::unique_ptr<Entity> remove_child(Entity *child);

    /**
     * Find child entity by index.
     * @param index - The index of the child entity.
     * @returns    The component which be found.
     */
    Entity *get_child(int index);

    /**
     * Find child entity by name.
     * @param name - The name of the entity which want to be found.
     * @returns The component which be found.
     */
    Entity *find_by_name(const std::string &name);

    /**
     * Create child entity.
     * @param name - The child entity's name.
     * @returns The child entity.
     */
    Entity *create_child(const std::string &name = "");

    /**
     * Clear children entities.
     */
    void clear_children();

    void remove_component(Component *component);

    /**fre
     * Clone
     * @returns Cloned entity.
     */
    std::unique_ptr<Entity> clone();

public:
    std::vector<Script *> get_scripts();

private:
    friend class Component;

    friend class Transform;

    friend class Script;

    friend class Scene;

    void add_script(Script *script);

    void remove_script(Script *script);

    std::unique_ptr<Entity> remove_from_parent();

    void process_active();

    void process_inactive();

    template<typename T>
    void get_components_in_children(std::vector<T *> &results) {
        for (auto &_component : components) {
            T *component = dynamic_cast<T *>(_component.get());
            if (component) {
                results.push_back(component);
            }
        }
        for (auto &i : children) {
            i->get_components_in_children(results);
        }
    }

    void set_active_components(bool is_active);

    void set_active_in_hierarchy(std::vector<Component *> &active_changed_components);

    void set_inactive_in_hierarchy(std::vector<Component *> &active_changed_components);

    void set_transform_dirty();

    static Entity *find_child_by_name(Entity *root, const std::string &name);

    static void traverse_set_owner_scene(Entity *entity, vox::Scene *scene);

    bool _is_active_in_hierarchy = false;
    std::vector<std::unique_ptr<Component>> components{};
    std::vector<Script *> scripts{};
    std::vector<std::unique_ptr<Entity>> children{};
    vox::Scene *scene = nullptr;
    bool is_root = false;
    bool _is_active = true;

    Entity *parent = nullptr;
    std::vector<Component *> active_changed_components{};

    std::unique_ptr<UpdateFlag> inverse_world_mat_flag = nullptr;
};

}// namespace vox
