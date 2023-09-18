//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "common/logging.h"
#include "runtime/volume.h"

namespace vox::compute ::detail {

void error_volume_invalid_mip_levels(size_t level, size_t mip) noexcept {
    ERROR_WITH_LOCATION(
        "Invalid mipmap level {} for volume with {} levels.",
        level, mip);
}

}// namespace vox::compute::detail
