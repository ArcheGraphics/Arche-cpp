//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <cstdint>

namespace vox::compute {

enum class StreamTag : uint32_t {
    GRAPHICS,// capable of graphics, compute, and copy commands
    COMPUTE, // capable of compute and copy commands
    COPY,    // only copy commands,
    CUSTOM   // custom stream
};

}// namespace vox::compute
