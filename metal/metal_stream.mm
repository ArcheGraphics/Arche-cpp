//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <Metal/Metal.h>
#include "common/logging.h"

extern void luisa_compute_metal_stream_print_function_logs(id<MTLLogContainer> logs) {
    if (logs != nullptr) {
        for (id<MTLFunctionLog> log in logs) {
            LOGI("[MTLFunctionLog] {}", log.debugDescription.UTF8String);
        }
    }
}
