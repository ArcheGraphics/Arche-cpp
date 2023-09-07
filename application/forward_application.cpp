//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "forward_application.h"
#include "rendering/subpasses/forward_subpass.h"
#include "platform/platform.h"
#include "components/camera.h"
#include "framework/common/metal_helpers.h"

namespace vox {
ForwardApplication::~ForwardApplication() {
    // release first
    scene_manager_.reset();

    components_manager_.reset();
    light_manager_.reset();

    texture_manager_->collect_garbage();
    texture_manager_.reset();
    mesh_manager_->collect_garbage();
    mesh_manager_.reset();
}

bool ForwardApplication::prepare(Platform &engine) {
    before_prepare();
    MetalApplication::prepare(engine);
    after_prepare();

    // resource loader
    texture_manager_ = std::make_unique<TextureManager>(*_device);
    mesh_manager_ = std::make_unique<MeshManager>(*_device);

    // logic system
    components_manager_ = std::make_unique<ComponentsManager>();
    scene_manager_ = std::make_unique<SceneManager>(*_device);
    auto scene = scene_manager_->get_current_scene();

    light_manager_ = std::make_unique<LightManager>(scene);
    {
        _mainCamera = load_scene();
        auto extent = engine.get_window().get_extent();
        auto factor = engine.get_window().get_content_scale_factor();
        components_manager_->call_script_resize(extent.width, extent.height, factor * extent.width,
                                                factor * extent.height);
        _mainCamera->resize(extent.width, extent.height, factor * extent.width, factor * extent.height);
    }

    _renderPass->addSubpass(std::make_unique<ForwardSubpass>(_renderContext.get(), scene, _mainCamera));
    if (_gui) {
        _renderPass->setGUI(_gui.get());
    }

    after_load_scene();

    return true;
}

void ForwardApplication::update(float delta_time) {
    {
        components_manager_->call_script_on_start();

        components_manager_->call_script_on_update(delta_time);
        components_manager_->call_script_on_late_update(delta_time);

        components_manager_->call_renderer_on_update(delta_time);
        scene_manager_->get_current_scene()->update_shader_data();
    }

    MetalApplication::update(delta_time);
}

bool ForwardApplication::resize(uint32_t win_width, uint32_t win_height,
                                uint32_t fb_width, uint32_t fb_height) {
    MetalApplication::resize(win_width, win_height, fb_width, fb_height);
    components_manager_->call_script_resize(win_width, win_height, fb_width, fb_height);
    _mainCamera->resize(win_width, win_height, fb_width, fb_height);
    return true;
}

void ForwardApplication::input_event(const InputEvent &inputEvent) {
    MetalApplication::input_event(inputEvent);
    components_manager_->call_script_input_event(inputEvent);
}

}// namespace vox
