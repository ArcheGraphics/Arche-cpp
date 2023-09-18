//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <utility>

#include "runtime/rhi/resource.h"
#include "runtime/event.h"
#include "runtime/command_list.h"
#include "runtime/image.h"
#include "runtime/stream_event.h"

namespace vox::compute {

class Swapchain final : public Resource {
public:
    struct Present {
        const Swapchain *chain{nullptr};
        ImageView<float> frame;
        void operator()(DeviceInterface *device, uint64_t stream_handle) && noexcept;
    };

private:
    friend class Device;
    PixelStorage _storage{};
    Swapchain(DeviceInterface *device, const SwapchainCreationInfo &create_info) noexcept;
    Swapchain(DeviceInterface *device, uint64_t window_handle,
              uint64_t stream_handle, uint width, uint height,
              bool allow_hdr, bool vsync, uint back_buffer_size) noexcept;

public:
    Swapchain() noexcept = default;
    ~Swapchain() noexcept override;
    using Resource::operator bool;
    Swapchain(Swapchain &&) noexcept = default;
    Swapchain(Swapchain const &) noexcept = delete;
    Swapchain &operator=(Swapchain &&rhs) noexcept {
        _move_from(std::move(rhs));
        return *this;
    }
    Swapchain &operator=(Swapchain const &) noexcept = delete;
    [[nodiscard]] PixelStorage backend_storage() const {
        _check_is_valid();
        return _storage;
    }
    [[nodiscard]] Present present(ImageView<float> frame) const noexcept;
};

VOX_MARK_STREAM_EVENT_TYPE(Swapchain::Present)

}// namespace vox::compute
