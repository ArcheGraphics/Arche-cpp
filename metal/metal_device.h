//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "runtime/rhi/device_interface.h"
#include "metal_api.h"
#include <mutex>

namespace vox::compute::metal {

class MetalDebugCaptureExt;

class MetalDevice : public DeviceInterface {

public:
    static constexpr auto update_bindless_slots_block_size = 256u;
    static constexpr auto update_accel_instances_block_size = 256u;
    static constexpr auto prepare_indirect_dispatches_block_size = 64u;

private:
    MTL::Device *_handle{nullptr};
    MTL::ComputePipelineState *_builtin_update_bindless_slots{nullptr};
    MTL::ComputePipelineState *_builtin_update_accel_instances{nullptr};
    MTL::ComputePipelineState *_builtin_prepare_indirect_dispatches{nullptr};
    MTL::RenderPipelineState *_builtin_swapchain_present_ldr{nullptr};
    MTL::RenderPipelineState *_builtin_swapchain_present_hdr{nullptr};
    bool _inqueue_buffer_limit;

private:
    std::mutex _ext_mutex;
    std::unique_ptr<MetalDebugCaptureExt> _debug_capture_ext;

public:
    [[nodiscard]] auto handle() const noexcept { return _handle; }
    [[nodiscard]] auto builtin_update_bindless_slots() const noexcept { return _builtin_update_bindless_slots; }
    [[nodiscard]] auto builtin_update_accel_instances() const noexcept { return _builtin_update_accel_instances; }
    [[nodiscard]] auto builtin_prepare_indirect_dispatches() const noexcept { return _builtin_prepare_indirect_dispatches; }
    [[nodiscard]] auto builtin_swapchain_present_ldr() const noexcept { return _builtin_swapchain_present_ldr; }
    [[nodiscard]] auto builtin_swapchain_present_hdr() const noexcept { return _builtin_swapchain_present_hdr; }

public:
    MetalDevice(const DeviceConfig *config) noexcept;
    ~MetalDevice() noexcept override;
    void *native_handle() const noexcept override;
    uint compute_warp_size() const noexcept override;
    BufferCreationInfo create_buffer(size_t elem_count) noexcept override;
    void destroy_buffer(uint64_t handle) noexcept override;
    ResourceCreationInfo create_texture(PixelFormat format, uint dimension, uint width, uint height, uint depth, uint mipmap_levels, bool simultaneous_access) noexcept override;
    void destroy_texture(uint64_t handle) noexcept override;
    ResourceCreationInfo create_bindless_array(size_t size) noexcept override;
    void destroy_bindless_array(uint64_t handle) noexcept override;
    ResourceCreationInfo create_stream(StreamTag stream_tag) noexcept override;
    void destroy_stream(uint64_t handle) noexcept override;
    void synchronize_stream(uint64_t stream_handle) noexcept override;
    void dispatch(uint64_t stream_handle, CommandList &&list) noexcept override;
    SwapchainCreationInfo create_swapchain(uint64_t window_handle, uint64_t stream_handle, uint width, uint height, bool allow_hdr, bool vsync, uint back_buffer_size) noexcept override;
    void destroy_swap_chain(uint64_t handle) noexcept override;
    void present_display_in_stream(uint64_t stream_handle, uint64_t swapchain_handle, uint64_t image_handle) noexcept override;
    ShaderCreationInfo load_shader(std::string_view name) noexcept override;
    Usage shader_argument_usage(uint64_t handle, size_t index) noexcept override;
    void destroy_shader(uint64_t handle) noexcept override;
    ResourceCreationInfo create_event() noexcept override;
    void destroy_event(uint64_t handle) noexcept override;
    void signal_event(uint64_t handle, uint64_t stream_handle, uint64_t value) noexcept override;
    bool is_event_completed(uint64_t handle, uint64_t value) const noexcept override;
    void wait_event(uint64_t handle, uint64_t stream_handle, uint64_t value) noexcept override;
    void synchronize_event(uint64_t handle, uint64_t value) noexcept override;

    std::string query(std::string_view property) noexcept override;
    DeviceExtension *extension(std::string_view name) noexcept override;
    void set_name(Resource::Tag resource_tag, uint64_t resource_handle, std::string_view name) noexcept override;
};

}// namespace vox::compute::metal

