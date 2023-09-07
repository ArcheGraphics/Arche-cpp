//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <memory>
#include <string>
#include <typeindex>
#include <vector>

namespace vox {
class Entity;
class Scene;

/**
 * The base class of the components.
 */
class Component {
public:
    explicit Component(Entity *entity);

    Component(Component &&other) = default;

    virtual ~Component();

    /**
     * Indicates whether the component is enabled.
     */
    [[nodiscard]] bool is_enabled() const;

    void set_enabled(bool value);

    /**
     * The entity which the component belongs to.
     */
    [[nodiscard]] Entity *get_entity() const;

    /**
     * The scene which the component's entity belongs to.
     */
    Scene *get_scene();

public:
    virtual void on_awake() {}

    virtual void on_enable() {}

    virtual void on_disable() {}

    virtual void on_active() {}

    virtual void on_inactive() {}

protected:
    friend class Entity;

    void set_active(bool value);

    vox::Entity *entity_;

private:
    bool enabled_ = true;
    bool awoken_ = false;
};

}// namespace vox
