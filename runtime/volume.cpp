#include "core/logging.h"
#include "runtime/volume.h"

namespace vox::compute ::detail {

void error_volume_invalid_mip_levels(size_t level, size_t mip) noexcept {
    LUISA_ERROR_WITH_LOCATION(
        "Invalid mipmap level {} for volume with {} levels.",
        level, mip);
}

}// namespace vox::compute::detail
