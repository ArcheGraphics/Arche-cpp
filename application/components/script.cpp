//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "components/script.h"

#include "ecs/components_manager.h"
#include "ecs/entity.h"
#include "ecs/scene.h"

namespace vox {
Script::Script(Entity *entity) : Component(entity) {}

Script::~Script() { ComponentsManager::get_singleton().add_destroy_component(this); }

void Script::set_is_started(bool value) { started_ = value; }

bool Script::is_started() const { return started_; }

void Script::on_awake() { on_script_awake(); }

void Script::on_enable() {
    auto components_manager = ComponentsManager::get_singleton_ptr();
    if (!started_) {
        components_manager->add_on_start_script(this);
    }
    components_manager->add_on_update_script(this);
    entity_->add_script(this);
    on_script_enable();
}

void Script::on_disable() {
    auto components_manager = ComponentsManager::get_singleton_ptr();
    components_manager->remove_on_start_script(this);
    components_manager->remove_on_update_script(this);
    entity_->remove_script(this);
    on_script_disable();
}

}// namespace vox
