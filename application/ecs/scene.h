//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "base/background.h"
#include "light/ambient_light.h"
#include "platform/input_events.h"
#include "shader/shader_data.h"

namespace vox {
class Entity;
class Camera;

/// @brief A collection of entities organized in a tree structure.
///		   It can contain more than one root entity.
struct Scene final {
public:
    /** Scene name. */
    std::string name;

    /** The background of the scene. */
    Background background = Background();

    /** Scene-related shader data. */
    ShaderData shader_data;

    /**
     * Create scene.
     * @param device - Device
     */
    explicit Scene(MTL::Device &device);

    ~Scene();

    MTL::Device &get_device();

    /**
     * Ambient light.
     */
    [[nodiscard]] const std::shared_ptr<AmbientLight> &get_ambient_light() const;

    void set_ambient_light(const std::shared_ptr<vox::AmbientLight> &light);

    /**
     * Count of root entities.
     */
    size_t get_root_entities_count();

    /**
     * Root entity collection.
     */
    [[nodiscard]] const std::vector<std::unique_ptr<Entity>> &get_root_entities() const;

    /**
     * Play the scene
     */
    void play();

    /**
     * Returns true if the scene is playing
     */
    [[nodiscard]] bool is_playing() const;

    /**
     * Create root entity.
     * @param name - Entity name
     * @returns Entity
     */
    Entity *create_root_entity(const std::string &name = "");

    /**
     * Append an entity.
     * @param entity - The root entity to add
     */
    void add_root_entity(std::unique_ptr<Entity> &&entity);

    /**
     * Remove an entity.
     * @param entity - The root entity to remove
     */
    void remove_root_entity(Entity *entity);

    /**
     * Get root entity from index.
     * @param index - Index
     * @returns Entity
     */
    Entity *get_root_entity(size_t index = 0);

    /**
     * Find entity globally by name.
     * @param name - Entity name
     * @returns Entity
     */
    Entity *find_entity_by_name(const std::string &name);

    void attach_render_camera(Camera *camera);

    void detach_render_camera(Camera *camera);

public:
    void update_shader_data();

private:
    friend class SceneManager;

    void process_active(bool active);

    void remove_entity(Entity *old_entity);

    std::vector<Camera *> active_cameras{};

    bool is_active_in_engine = false;

    std::vector<std::unique_ptr<Entity>> root_entities;
    std::shared_ptr<vox::AmbientLight> ambient_light;

    MTL::Device &device;
};

}// namespace vox
