//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "runtime/rhi/resource.h"
#include "runtime/rhi/device_interface.h"

namespace vox::compute {
struct IndirectKernelDispatch {};
class IndirectDispatchBuffer;
}// namespace vox::compute

namespace vox::compute {

namespace detail {
class IndirectDispatchBufferExprProxy;
}// namespace detail

class IndirectDispatchBuffer final : public Resource {

private:
    size_t _capacity{};
    size_t _byte_size{};

private:
    friend class Device;
    IndirectDispatchBuffer(DeviceInterface *device, size_t capacity, BufferCreationInfo info) noexcept
        : Resource{device, Tag::BUFFER, info},
          _capacity{capacity},
          _byte_size{info.total_size_bytes} {}

public:
    IndirectDispatchBuffer(DeviceInterface *device, size_t capacity) noexcept
        : IndirectDispatchBuffer{device, capacity,
                                 device->create_buffer(capacity)} {
    }
    IndirectDispatchBuffer() noexcept = default;
    ~IndirectDispatchBuffer() noexcept override;
    IndirectDispatchBuffer(IndirectDispatchBuffer &&) noexcept = default;
    IndirectDispatchBuffer(IndirectDispatchBuffer const &) noexcept = delete;
    IndirectDispatchBuffer &operator=(IndirectDispatchBuffer &&rhs) noexcept {
        _move_from(std::move(rhs));
        return *this;
    }
    IndirectDispatchBuffer &operator=(IndirectDispatchBuffer const &) noexcept = delete;
    using Resource::operator bool;
    [[nodiscard]] auto capacity() const noexcept {
        _check_is_valid();
        return _capacity;
    }
    [[nodiscard]] auto size_bytes() const noexcept {
        _check_is_valid();
        return _byte_size;
    }

    // DSL interface
    [[nodiscard]] auto operator->() const noexcept {
        _check_is_valid();
        return reinterpret_cast<const detail::IndirectDispatchBufferExprProxy *>(this);
    }
};

}// namespace vox::compute
