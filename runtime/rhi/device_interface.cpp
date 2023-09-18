#include "runtime/rhi/device_interface.h"
#include "runtime/context.h"

namespace vox::compute {

DeviceInterface::DeviceInterface(Context &&ctx) noexcept
    : _ctx_impl{std::move(ctx).impl()} {}

DeviceInterface::~DeviceInterface() noexcept = default;

Context DeviceInterface::context() const noexcept {
    return Context{_ctx_impl};
}

}// namespace vox::compute
