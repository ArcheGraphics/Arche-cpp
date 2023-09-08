//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <Metal/Metal.hpp>
#include <span>

namespace vox::compute {

// Sets data for a |device_buffer| via a CPU staging buffer by invoking
// |staging_buffer_setter| on the pointer pointing to the start of the CPU
// staging buffer. |device_buffer| is expected to have
// VK_BUFFER_USAGE_TRANSFER_DST_BIT bit.
void set_device_buffer_via_staging_buffer(
    MTL::Device &device, MTL::Buffer &device_buffer,
    size_t buffer_size_in_bytes,
    const std::function<void(void *, size_t)> &staging_buffer_setter);

// Convenience overload of `SetDeviceBufferViaStagingBuffer` that passes
// in a span of type |ElemetType| to the getter |stagin_beffer_setter|.
template<typename ElementType>
void set_device_buffer_via_staging_buffer(
    MTL::Device &device, MTL::Buffer &device_buffer,
    size_t buffer_size_in_bytes,
    const std::function<void(std::span<ElementType>)> &staging_buffer_setter) {
    return SetDeviceBufferViaStagingBuffer(
        device, device_buffer, buffer_size_in_bytes,
        [&staging_buffer_setter](void *buffer, size_t size) {
            staging_buffer_setter(std::span(static_cast<ElementType *>(buffer),
                                            size / sizeof(ElementType)));
        });
}

// Get data from a |device_buffer| via a CPU staging buffer by invoking
// |staging_buffer_getter| on the pointer pointing to the start of the CPU
// staging buffer. |device_buffer| is expected to have
// VK_BUFFER_USAGE_TRANSFER_SRC_BIT bit.
void get_device_buffer_via_staging_buffer(
    MTL::Device &device, MTL::Buffer &device_buffer,
    size_t buffer_size_in_bytes,
    const std::function<void(void *, size_t)> &staging_buffer_getter);

// Convenience overload of `GetDeviceBufferViaStagingBuffer` that passes
// in a span of type |ElemetType| to the getter |stagin_beffer_getter|.
template<typename ElementType>
void get_device_buffer_via_staging_buffer(
    MTL::Device &device, MTL::Buffer &device_buffer,
    size_t buffer_size_in_bytes,
    const std::function<void(std::span<const ElementType>)>
        &staging_buffer_getter) {
    return GetDeviceBufferViaStagingBuffer(
        device, device_buffer, buffer_size_in_bytes,
        [&staging_buffer_getter](void *buffer, size_t size) {
            staging_buffer_getter(
                std::span(static_cast<const ElementType *>(buffer),
                          size / sizeof(ElementType)));
        });
}

}// namespace vox::compute