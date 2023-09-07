//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "unix_platform.h"

#include "glfw_window.h"
#include "common/filesystem.h"

namespace vox {

namespace fs {
void create_directory(const std::string &path) {
    if (!is_directory(path)) {
        mkdir(path.c_str(), 0777);
    }
}
}// namespace fs

void UnixPlatform::create_window(const Window::Properties &properties) {
    window = std::make_unique<GlfwWindow>(this, properties);
}
}// namespace vox
