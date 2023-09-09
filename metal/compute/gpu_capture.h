//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <Metal/Metal.hpp>
#include <string>

namespace vox::compute {
std::shared_ptr<MTL::CaptureScope> create_capture_scope(const std::string &name, MTL::Device &device);

}// namespace vox::compute