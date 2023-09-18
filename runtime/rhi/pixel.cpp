#include "core/logging.h"
#include "runtime/rhi/pixel.h"

namespace vox::compute::detail {

void error_pixel_invalid_format(const char *name) noexcept {
    LUISA_ERROR_WITH_LOCATION("Invalid pixel storage for {} format.", name);
}

}// namespace vox::compute::detail
