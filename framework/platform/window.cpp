//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "window.h"

#include <utility>

namespace vox {
Window::Window(Properties properties) : properties{std::move(properties)} {
}

void Window::process_events() {
}

Window::Extent Window::resize(const Extent &new_extent) {
    if (properties.resizable) {
        properties.extent.width = new_extent.width;
        properties.extent.height = new_extent.height;
    }

    return properties.extent;
}

const Window::Extent &Window::get_extent() const {
    return properties.extent;
}

float Window::get_content_scale_factor() const {
    return 1.0f;
}

Window::Mode Window::get_window_mode() const {
    return properties.mode;
}

}// namespace vox
