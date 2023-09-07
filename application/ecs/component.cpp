//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "ecs/component.h"
#include "ecs/entity.h"

namespace vox {
Component::Component(vox::Entity *entity) : entity_(entity) {}

Component::~Component() {
    if (entity_->is_active_in_hierarchy()) {
        if (enabled_) {
            on_disable();
        }
        on_inactive();
    }
}

bool Component::is_enabled() const { return enabled_; }

void Component::set_enabled(bool value) {
    if (value == enabled_) {
        return;
    }
    enabled_ = value;
    if (value) {
        if (entity_->is_active_in_hierarchy()) {
            on_enable();
        }
    } else {
        if (entity_->is_active_in_hierarchy()) {
            on_disable();
        }
    }
}

Entity *Component::get_entity() const { return entity_; }

Scene *Component::get_scene() { return entity_->get_scene(); }

void Component::set_active(bool value) {
    if (value) {
        if (!awoken_) {
            awoken_ = true;
            on_awake();
        }
        // You can do is_active = false in onAwake function.
        if (entity_->_is_active_in_hierarchy) {
            on_active();
            if (enabled_) {
                on_enable();
            }
        }
    } else {
        if (enabled_) {
            on_disable();
        }
        on_inactive();
    }
}

}// namespace vox
