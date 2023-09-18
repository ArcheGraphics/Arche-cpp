//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "common/logging.h"
#include "runtime/rhi/pixel.h"

namespace vox::compute::detail {

void error_pixel_invalid_format(const char *name) noexcept {
    ERROR_WITH_LOCATION("Invalid pixel storage for {} format.", name);
}

}// namespace vox::compute::detail
