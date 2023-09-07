//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "application.h"

#include "framework/platform/window.h"
#include "framework/platform/platform.h"

namespace vox {
Application::Application() : name{"Sample Name"} {
}

bool Application::prepare(Platform &engine) {
    assert(options.window != nullptr && "Window must be valid");

    window = &engine.get_window();

    return true;
}

void Application::finish() {
}

bool Application::resize(const uint32_t /*width*/, const uint32_t /*height*/,
                         uint32_t fb_width, uint32_t fb_height) {
    return true;
}

void Application::input_event(const InputEvent &input_event) {
}

void Application::update(float delta_time) {
    fps = 1.0f / delta_time;
    frame_time = delta_time * 1000.0f;
}

const std::string &Application::get_name() const {
    return name;
}

void Application::set_name(const std::string &name_) {
    name = name_;
}

}// namespace vox
