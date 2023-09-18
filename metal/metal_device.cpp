//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "metal_buffer.h"
#include "metal_texture.h"
#include "metal_stream.h"
#include "metal_event.h"
#include "metal_swapchain.h"
#include "metal_device.h"

// extensions

#include "metal_debug_capture.h"

namespace vox::compute::metal {

MetalDevice::MetalDevice(const DeviceConfig *config) noexcept
    : DeviceInterface{},
      _inqueue_buffer_limit{config == nullptr || config->inqueue_buffer_limit} {

    auto device_index = config == nullptr ? 0u : config->device_index;
    auto all_devices = MTL::CopyAllDevices();
    auto device_count = all_devices->count();
    ASSERT(device_index < device_count,
           "Metal device index out of range.");
    _handle = all_devices->object<MTL::Device>(device_index)->retain();
    all_devices->release();

    ASSERT(_handle->supportsFamily(MTL::GPUFamilyMetal3),
           "Metal device '{}' at index {} does not support Metal 3.",
           _handle->name()->utf8String(), device_index);

    LOGI("Created Metal device '{}' at index {}.",
         _handle->name()->utf8String(), device_index);
}

MetalDevice::~MetalDevice() noexcept {
    _handle->release();
}

void *MetalDevice::native_handle() const noexcept {
    return _handle;
}

uint MetalDevice::compute_warp_size() const noexcept {
    //    return _builtin_update_bindless_slots->threadExecutionWidth();
}

[[nodiscard]] inline auto create_device_buffer(MTL::Device *device, size_t element_stride, size_t element_count) noexcept {
    auto buffer_size = element_stride * element_count;
    auto buffer = new MetalBuffer(device, buffer_size);
    BufferCreationInfo info{};
    info.handle = reinterpret_cast<uint64_t>(buffer);
    info.native_handle = buffer->handle();
    info.element_stride = element_stride;
    info.total_size_bytes = buffer_size;
    return info;
}

BufferCreationInfo MetalDevice::create_buffer(size_t elem_count) noexcept {
    return with_autorelease_pool([=, this] {
        // todo
        return create_device_buffer(_handle, 1, elem_count);
    });
}

void MetalDevice::destroy_buffer(uint64_t handle) noexcept {
    with_autorelease_pool([=] {
        auto buffer = reinterpret_cast<MetalBufferBase *>(handle);
        delete buffer;
    });
}

ResourceCreationInfo MetalDevice::create_texture(PixelFormat format, uint dimension,
                                                 uint width, uint height, uint depth, uint mipmap_levels,
                                                 bool allow_simultaneous_access) noexcept {
    return with_autorelease_pool([=, this] {
        auto texture = new MetalTexture(
            _handle, format, dimension, width, height, depth,
            mipmap_levels, allow_simultaneous_access);
        ResourceCreationInfo info{};
        info.handle = reinterpret_cast<uint64_t>(texture);
        info.native_handle = texture->handle();
        return info;
    });
}

void MetalDevice::destroy_texture(uint64_t handle) noexcept {
    with_autorelease_pool([=] {
        auto texture = reinterpret_cast<MetalTexture *>(handle);
        delete texture;
    });
}

ResourceCreationInfo MetalDevice::create_stream(StreamTag stream_tag) noexcept {
    return with_autorelease_pool([=, this] {
        auto stream = new MetalStream(
            _handle, _inqueue_buffer_limit ? 4u : 0u);
        ResourceCreationInfo info{};
        info.handle = reinterpret_cast<uint64_t>(stream);
        info.native_handle = stream->queue();
        return info;
    });
}

void MetalDevice::destroy_stream(uint64_t handle) noexcept {
    with_autorelease_pool([=] {
        auto stream = reinterpret_cast<MetalStream *>(handle);
        delete stream;
    });
}

void MetalDevice::synchronize_stream(uint64_t stream_handle) noexcept {
    with_autorelease_pool([=] {
        auto stream = reinterpret_cast<MetalStream *>(stream_handle);
        stream->synchronize();
    });
}

void MetalDevice::dispatch(uint64_t stream_handle, CommandList &&list) noexcept {
    with_autorelease_pool([stream_handle, &list] {
        auto stream = reinterpret_cast<MetalStream *>(stream_handle);
        stream->dispatch(std::move(list));
    });
}

SwapchainCreationInfo MetalDevice::create_swapchain(uint64_t window_handle, uint64_t stream_handle,
                                                    uint width, uint height, bool allow_hdr,
                                                    bool vsync, uint back_buffer_size) noexcept {
    return with_autorelease_pool([=, this] {
        auto swapchain = new MetalSwapchain(
            this, window_handle, width, height,
            allow_hdr, vsync, back_buffer_size);
        SwapchainCreationInfo info{};
        info.handle = reinterpret_cast<uint64_t>(swapchain);
        info.native_handle = swapchain->layer();
        info.storage = swapchain->pixel_storage();
        return info;
    });
}

void MetalDevice::destroy_swap_chain(uint64_t handle) noexcept {
    with_autorelease_pool([=] {
        auto swpachain = reinterpret_cast<MetalSwapchain *>(handle);
        delete swpachain;
    });
}

void MetalDevice::present_display_in_stream(uint64_t stream_handle, uint64_t swapchain_handle, uint64_t image_handle) noexcept {
    with_autorelease_pool([=] {
        auto stream = reinterpret_cast<MetalStream *>(stream_handle);
        auto swapchain = reinterpret_cast<MetalSwapchain *>(swapchain_handle);
        auto image = reinterpret_cast<MetalTexture *>(image_handle);
        stream->present(swapchain, image);
    });
}

ResourceCreationInfo MetalDevice::create_event() noexcept {
    return with_autorelease_pool([=, this] {
        auto event = new MetalEvent(_handle);
        ResourceCreationInfo info{};
        info.handle = reinterpret_cast<uint64_t>(event);
        info.native_handle = event->handle();
        return info;
    });
}

void MetalDevice::destroy_event(uint64_t handle) noexcept {
    with_autorelease_pool([=] {
        auto event = reinterpret_cast<MetalEvent *>(handle);
        delete event;
    });
}

void MetalDevice::signal_event(uint64_t handle, uint64_t stream_handle, uint64_t value) noexcept {
    // TODO: fence not implemented
    with_autorelease_pool([=] {
        auto event = reinterpret_cast<MetalEvent *>(handle);
        auto stream = reinterpret_cast<MetalStream *>(stream_handle);
        stream->signal(event, value);
    });
}

void MetalDevice::wait_event(uint64_t handle, uint64_t stream_handle, uint64_t value) noexcept {
    // TODO: fence not implemented
    with_autorelease_pool([=] {
        auto event = reinterpret_cast<MetalEvent *>(handle);
        auto stream = reinterpret_cast<MetalStream *>(stream_handle);
        stream->wait(event, value);
    });
}

void MetalDevice::synchronize_event(uint64_t handle, uint64_t value) noexcept {
    // TODO: fence not implemented
    with_autorelease_pool([=] {
        auto event = reinterpret_cast<MetalEvent *>(handle);
        event->synchronize(value);
    });
}

bool MetalDevice::is_event_completed(uint64_t handle, uint64_t value) const noexcept {
    // TODO: fence not implemented
    return with_autorelease_pool([=] {
        auto event = reinterpret_cast<MetalEvent *>(handle);
        return event->is_completed(value);
    });
}

std::string MetalDevice::query(std::string_view property) noexcept {
    WARNING_WITH_LOCATION("Device property \"{}\" is not supported on Metal.", property);
    return {};
}

DeviceExtension *MetalDevice::extension(std::string_view name) noexcept {
    return with_autorelease_pool([=, this]() noexcept -> DeviceExtension * {
        if (name == DebugCaptureExt::name) {
            std::scoped_lock lock{_ext_mutex};
            if (!_debug_capture_ext) { _debug_capture_ext = std::make_unique<MetalDebugCaptureExt>(this); }
            return _debug_capture_ext.get();
        }
        WARNING_WITH_LOCATION("Device extension \"{}\" is not supported on Metal.", name);
        return nullptr;
    });
}

void MetalDevice::set_name(Resource::Tag resource_tag,
                           uint64_t resource_handle, std::string_view name) noexcept {

    with_autorelease_pool([=] {
        switch (resource_tag) {
            case Resource::Tag::BUFFER: {
                auto buffer = reinterpret_cast<MetalBufferBase *>(resource_handle);
                buffer->set_name(name);
                break;
            }
            case Resource::Tag::TEXTURE: {
                auto texture = reinterpret_cast<MetalTexture *>(resource_handle);
                texture->set_name(name);
                break;
            }
            case Resource::Tag::STREAM: {
                auto stream = reinterpret_cast<MetalStream *>(resource_handle);
                stream->set_name(name);
                break;
            }
            case Resource::Tag::EVENT: {
                auto event = reinterpret_cast<MetalEvent *>(resource_handle);
                event->set_name(name);
                break;
            }
            case Resource::Tag::SWAP_CHAIN: {
                auto swapchain = reinterpret_cast<MetalSwapchain *>(resource_handle);
                swapchain->set_name(name);
                break;
            }
        }
    });
}

}// namespace vox::compute::metal
