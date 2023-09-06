//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "scene_forward.h"
#include <memory>
#include <string>
#include <typeindex>
#include <vector>

namespace vox {
/**
 * The base class of the components.
 */
class Component {
public:
    explicit Component(Entity *entity);

    Component(Component &&other) = default;

    virtual ~Component() = default;

    /**
     * Indicates whether the component is enabled.
     */
    bool enabled();

    void setEnabled(bool value);

    /**
     * Indicates whether the component is destroyed.
     */
    bool destroyed();

    /**
     * The entity which the component belongs to.
     */
    Entity *entity() const;

    /**
     * The scene which the component's entity belongs to.
     */
    Scene *scene();

    /**
     * Destroy this instance.
     */
    void destroy();

public:
    virtual void _onAwake() {
    }

    virtual void _onEnable() {
    }

    virtual void _onDisable() {
    }

    virtual void _onDestroy() {
    }

    virtual void _onActive() {
    }

    virtual void _onInActive() {
    }

protected:
    friend class Entity;

    void _setActive(bool value);

    Entity *_entity;
    bool _destroyed = false;

private:
    bool _enabled = true;
    bool _awoken = false;
};

}// namespace vox
