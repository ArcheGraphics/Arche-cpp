#include "core/logging.h>
#include "runtime/device.h>

namespace vox::compute {

void Device::_check_no_implicit_binding(Function func, std::string_view shader_path) noexcept {
#ifndef NDEBUG
    for (auto &&b : func.bound_arguments()) {
        if (!holds_alternative<monostate>(b)) {
            LUISA_ERROR("Kernel {} with resource "
                        "bindings cannot be saved!",
                        shader_path);
        }
    }
#endif
}

}// namespace vox::compute
