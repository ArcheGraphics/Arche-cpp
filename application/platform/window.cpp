//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "window.h"

namespace vox {
Window::Window(const Properties &properties) : _properties{properties} {
}

void Window::processEvents() {
}

Window::Extent Window::resize(const Extent &newExtent) {
    if (_properties.resizable) {
        _properties.extent.width = newExtent.width;
        _properties.extent.height = newExtent.height;
    }

    return _properties.extent;
}

const Window::Extent &Window::extent() const {
    return _properties.extent;
}

float Window::contentScaleFactor() const {
    return 1.0f;
}

Window::Mode Window::windowMode() const {
    return _properties.mode;
}
}// namespace vox
