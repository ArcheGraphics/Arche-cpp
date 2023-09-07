//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "ecs/components_manager.h"
#include "ecs/entity.h"
#include "components/camera.h"
#include "components/renderer.h"
#include "components/script.h"
#include "framework/common/logging.h"

namespace vox {
ComponentsManager *ComponentsManager::get_singleton_ptr() { return ms_singleton; }

ComponentsManager &ComponentsManager::get_singleton() {
    assert(ms_singleton);
    return (*ms_singleton);
}

void ComponentsManager::add_on_start_script(Script *script) {
    auto iter = std::find(on_start_scripts_.begin(), on_start_scripts_.end(), script);
    if (iter == on_start_scripts_.end()) {
        on_start_scripts_.push_back(script);
    } else {
        LOGE("Script already attached.")
    }
}

void ComponentsManager::remove_on_start_script(Script *script) {
    auto iter = std::find(on_start_scripts_.begin(), on_start_scripts_.end(), script);
    if (iter != on_start_scripts_.end()) {
        on_start_scripts_.erase(iter);
    }
}

void ComponentsManager::add_on_update_script(Script *script) {
    auto iter = std::find(on_update_scripts_.begin(), on_update_scripts_.end(), script);
    if (iter == on_update_scripts_.end()) {
        on_update_scripts_.push_back(script);
    } else {
        LOGE("Script already attached.")
    }
}

void ComponentsManager::remove_on_update_script(Script *script) {
    auto iter = std::find(on_update_scripts_.begin(), on_update_scripts_.end(), script);
    if (iter != on_update_scripts_.end()) {
        on_update_scripts_.erase(iter);
    }
}

void ComponentsManager::add_destroy_component(Script *component) { destroy_components_.push_back(component); }

void ComponentsManager::call_component_destroy() {
    if (!destroy_components_.empty()) {
        for (auto &destroy_component : destroy_components_) {
            destroy_component->on_destroy();
        }
        destroy_components_.clear();
    }
}

void ComponentsManager::call_script_on_start() {
    if (!on_start_scripts_.empty()) {
        // The 'onStartScripts.length' maybe add if you add some Script with addComponent() in some Script's on_start()
        for (auto &script : on_start_scripts_) {
            script->set_is_started(true);
            script->on_start();
        }
        on_start_scripts_.clear();
    }
}

void ComponentsManager::call_script_on_update(float delta_time) {
    for (auto &element : on_update_scripts_) {
        if (element->is_started()) {
            element->on_update(delta_time);
        }
    }
}

void ComponentsManager::call_script_on_late_update(float delta_time) {
    for (auto &element : on_update_scripts_) {
        if (element->is_started()) {
            element->on_late_update(delta_time);
        }
    }
}

void ComponentsManager::call_script_input_event(const InputEvent &input_event) {
    for (auto &element : on_update_scripts_) {
        if (element->is_started()) {
            element->input_event(input_event);
        }
    }
}

void ComponentsManager::call_script_resize(uint32_t win_width, uint32_t win_height, uint32_t fb_width, uint32_t fb_height) {
    for (auto &element : on_update_scripts_) {
        if (element->is_started()) {
            element->resize(win_width, win_height, fb_width, fb_height);
        }
    }
}

// MARK: - Renderer
void ComponentsManager::add_renderer(Renderer *renderer) {
    auto iter = std::find(renderers_.begin(), renderers_.end(), renderer);
    if (iter == renderers_.end()) {
        renderers_.push_back(renderer);
    } else {
        LOGE("Renderer already attached.")
    }
}

void ComponentsManager::remove_renderer(Renderer *renderer) {
    auto iter = std::find(renderers_.begin(), renderers_.end(), renderer);
    if (iter != renderers_.end()) {
        renderers_.erase(iter);
    }
}

void ComponentsManager::call_renderer_on_update(float delta_time) {
    for (auto &renderer : renderers_) {
        renderer->update(delta_time);
    }
}

void ComponentsManager::call_render(Camera *camera,
                                    std::vector<RenderElement> &opaque_queue,
                                    std::vector<RenderElement> &alpha_test_queue,
                                    std::vector<RenderElement> &transparent_queue) {
    for (auto &element : renderers_) {
        // filter by camera culling mask.
        if (!(camera->culling_mask_ & element->entity_->layer)) {
            continue;
        }

        // filter by camera frustum.
        if (camera->enable_frustum_culling_) {
            element->is_culled_ = !camera->get_frustum().intersectsBox(element->get_bounds());
            if (element->is_culled_) {
                continue;
            }
        }

        const auto &transform = camera->get_entity()->transform;
        const auto kPosition = transform->get_world_position();
        auto center = element->get_bounds().midPoint();
        if (camera->is_orthographic()) {
            const auto kForward = transform->get_world_forward();
            const auto kOffset = center - kPosition;
            element->set_distance_for_sort(kOffset.dot(kForward));
        } else {
            element->set_distance_for_sort(center.distanceSquaredTo(kPosition));
        }

        element->render(opaque_queue, alpha_test_queue, transparent_queue);
    }
}

void ComponentsManager::call_render(const BoundingFrustum &frustum,
                                    std::vector<RenderElement> &opaque_queue,
                                    std::vector<RenderElement> &alpha_test_queue,
                                    std::vector<RenderElement> &transparent_queue) {
    for (auto &renderer : renderers_) {
        // filter by renderer castShadow and frustum cull
        if (frustum.intersectsBox(renderer->get_bounds())) {
            renderer->render(opaque_queue, alpha_test_queue, transparent_queue);
        }
    }
}

// MARK: - Camera
void ComponentsManager::call_camera_on_begin_render(Camera *camera) {
    const auto &cam_comps = camera->get_entity()->get_scripts();
    for (auto cam_comp : cam_comps) {
        cam_comp->on_begin_render(camera);
    }
}

void ComponentsManager::call_camera_on_end_render(Camera *camera) {
    const auto &cam_comps = camera->get_entity()->get_scripts();
    for (auto cam_comp : cam_comps) {
        cam_comp->on_end_render(camera);
    }
}

std::vector<Component *> ComponentsManager::get_active_changed_temp_list() {
    return !components_container_pool_.empty() ? *(components_container_pool_.end() - 1) : std::vector<Component *>{};
}

void ComponentsManager::put_active_changed_temp_list(std::vector<Component *> &component_container) {
    component_container.clear();
    components_container_pool_.push_back(component_container);
}

}// namespace vox
