//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "common/logging.h"
#include "common/pool.h"

namespace vox {

void  detail::memory_pool_check_memory_leak(size_t expected, size_t actual) noexcept {
    if (expected != actual) [[unlikely]] {
        WARNING_WITH_LOCATION(
            "Leaks detected in pool: "
            "expected {} objects but got {}.",
            expected, actual);
    }
}

}// namespace vox

