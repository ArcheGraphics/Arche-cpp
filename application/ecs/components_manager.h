//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <unordered_map>
#include <vector>

#include "base/singleton.h"
#include "math/bounding_frustum.h"
#include "math/matrix4x4.h"
#include "platform/input_events.h"
#include "rendering/render_element.h"

namespace vox {
class Script;
class Camera;
class Component;
class Renderer;

/**
 * The manager of the components.
 */
class ComponentsManager : public Singleton<ComponentsManager> {
public:
    static ComponentsManager &get_singleton();

    static ComponentsManager *get_singleton_ptr();

    void add_on_start_script(Script *script);

    void remove_on_start_script(Script *script);

    void add_on_update_script(Script *script);

    void remove_on_update_script(Script *script);

    void add_destroy_component(Script *component);

    void call_script_on_start();

    void call_script_on_update(float delta_time);

    void call_script_on_late_update(float delta_time);

    void call_script_input_event(const InputEvent &input_event);

    void call_script_resize(uint32_t win_width, uint32_t win_height, uint32_t fb_width, uint32_t fb_height);

    void call_component_destroy();

public:
    void add_renderer(Renderer *renderer);

    void remove_renderer(Renderer *renderer);

    void call_renderer_on_update(float delta_time);

    void call_render(Camera *camera,
                     std::vector<RenderElement> &opaque_queue,
                     std::vector<RenderElement> &alpha_test_queue,
                     std::vector<RenderElement> &transparent_queue);

    void call_render(const BoundingFrustum &frustum,
                     std::vector<RenderElement> &opaque_queue,
                     std::vector<RenderElement> &alpha_test_queue,
                     std::vector<RenderElement> &transparent_queue);

public:
    static void call_camera_on_begin_render(Camera *camera);

    static void call_camera_on_end_render(Camera *camera);

    std::vector<Component *> get_active_changed_temp_list();

    void put_active_changed_temp_list(std::vector<Component *> &component_container);

private:
    // Script
    std::vector<Script *> on_start_scripts_;
    std::vector<Script *> on_update_scripts_;
    std::vector<Script *> destroy_components_;

    // Render
    std::vector<Renderer *> renderers_;

    // Delay dispose active/inActive Pool
    std::vector<std::vector<Component *>> components_container_pool_;
};

template<>
inline ComponentsManager *Singleton<ComponentsManager>::ms_singleton{nullptr};

}// namespace vox
