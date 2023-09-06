//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "component.h"

#include <algorithm>

#include "entity.h"

namespace vox {
Component::Component(Entity *entity) : _entity(entity) {
}

bool Component::enabled() {
    return _enabled;
}

void Component::setEnabled(bool value) {
    if (value == _enabled) {
        return;
    }
    _enabled = value;
    if (value) {
        if (_entity->isActiveInHierarchy()) {
            _onEnable();
        }
    } else {
        if (_entity->isActiveInHierarchy()) {
            _onDisable();
        }
    }
}

bool Component::destroyed() {
    return _destroyed;
}

Entity *Component::entity() const {
    return _entity;
}

Scene *Component::scene() {
    return _entity->scene();
}

void Component::destroy() {
    if (_destroyed) {
        return;
    }
    _entity->_removeComponent(this);
    if (_entity->isActiveInHierarchy()) {
        if (_enabled) {
            _onDisable();
        }
        _onInActive();
    }
    _destroyed = true;
    _onDestroy();
}

void Component::_setActive(bool value) {
    if (value) {
        if (!_awoken) {
            _awoken = true;
            _onAwake();
        }
        // You can do isActive = false in onAwake function.
        if (_entity->_isActiveInHierarchy) {
            _onActive();
            if (_enabled) {
                _onEnable();
            }
        }
    } else {
        if (_enabled) {
            _onDisable();
        }
        _onInActive();
    }
}

}// namespace vox
