//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "common/logging.h"
#include "runtime/image.h"

namespace vox::compute ::detail {

void error_image_invalid_mip_levels(size_t level, size_t mip) noexcept {
    ERROR_WITH_LOCATION(
        "Invalid mipmap level {} for image with {} levels.",
        level, mip);
}
void image_size_zero_error() noexcept {
    ERROR_WITH_LOCATION("Image size must be non-zero.");
}
void volume_size_zero_error() noexcept {
    ERROR_WITH_LOCATION("Volume size must be non-zero.");
}
}// namespace vox::compute::detail
