#pragma once

#include "common/basic_types.h"
#include "ast/function.h"
#include "runtime/rhi/resource.h"
#include "runtime/rhi/stream_tag.h"
#include "runtime/rhi/command.h"
#include "runtime/command_list.h"

namespace vox {
class BinaryIO;
}// namespace vox

namespace vox::compute {

class Context;

namespace detail {
class ContextImpl;
}// namespace detail

namespace ir {
struct KernelModule;
struct Type;
template<class T>
struct CArc;
}// namespace ir

class Type;
struct AccelOption;

class DeviceConfigExt {
public:
    virtual ~DeviceConfigExt() noexcept = default;
};

struct DeviceConfig {
    mutable std::unique_ptr<DeviceConfigExt> extension;
    const BinaryIO *binary_io{nullptr};
    size_t device_index{std::numeric_limits<size_t>::max()};
    bool inqueue_buffer_limit{true};
    bool headless{false};
};

class DeviceExtension {
protected:
    ~DeviceExtension() noexcept = default;
};

class DeviceInterface : public std::enable_shared_from_this<DeviceInterface> {

protected:
    friend class Context;
    std::string _backend_name;
    std::shared_ptr<detail::ContextImpl> _ctx_impl;

public:
    explicit DeviceInterface(Context &&ctx) noexcept;
    virtual ~DeviceInterface() noexcept;
    DeviceInterface(DeviceInterface &&) = delete;
    DeviceInterface(DeviceInterface const &) = delete;

    [[nodiscard]] Context context() const noexcept;
    [[nodiscard]] auto backend_name() const noexcept { return std::string_view{_backend_name}; }

    // native handle
    [[nodiscard]] virtual void *native_handle() const noexcept = 0;
    [[nodiscard]] virtual uint compute_warp_size() const noexcept = 0;

public:
    [[nodiscard]] virtual BufferCreationInfo create_buffer(const Type *element, size_t elem_count) noexcept = 0;
    [[nodiscard]] virtual BufferCreationInfo create_buffer(const ir::CArc<ir::Type> *element, size_t elem_count) noexcept = 0;
    virtual void destroy_buffer(uint64_t handle) noexcept = 0;

    // texture
    [[nodiscard]] virtual ResourceCreationInfo create_texture(
        PixelFormat format, uint dimension,
        uint width, uint height, uint depth,
        uint mipmap_levels, bool simultaneous_access) noexcept = 0;
    virtual void destroy_texture(uint64_t handle) noexcept = 0;

    // bindless array
    [[nodiscard]] virtual ResourceCreationInfo create_bindless_array(size_t size) noexcept = 0;
    virtual void destroy_bindless_array(uint64_t handle) noexcept = 0;

    // stream
    [[nodiscard]] virtual ResourceCreationInfo create_stream(StreamTag stream_tag) noexcept = 0;
    virtual void destroy_stream(uint64_t handle) noexcept = 0;
    virtual void synchronize_stream(uint64_t stream_handle) noexcept = 0;
    virtual void dispatch(uint64_t stream_handle, CommandList &&list) noexcept = 0;

    // swap chain
    [[nodiscard]] virtual SwapchainCreationInfo create_swapchain(
        uint64_t window_handle, uint64_t stream_handle,
        uint width, uint height, bool allow_hdr,
        bool vsync, uint back_buffer_size) noexcept = 0;
    virtual void destroy_swap_chain(uint64_t handle) noexcept = 0;
    virtual void present_display_in_stream(uint64_t stream_handle, uint64_t swapchain_handle, uint64_t image_handle) noexcept = 0;

    // kernel
    [[nodiscard]] virtual ShaderCreationInfo create_shader(const ShaderOption &option, Function kernel) noexcept = 0;
    [[nodiscard]] virtual ShaderCreationInfo create_shader(const ShaderOption &option, const ir::KernelModule *kernel) noexcept = 0;
    [[nodiscard]] virtual ShaderCreationInfo load_shader(std::string_view name, std::span<const Type *const> arg_types) noexcept = 0;
    virtual Usage shader_argument_usage(uint64_t handle, size_t index) noexcept = 0;
    virtual void destroy_shader(uint64_t handle) noexcept = 0;

    // event
    [[nodiscard]] virtual ResourceCreationInfo create_event() noexcept = 0;
    virtual void destroy_event(uint64_t handle) noexcept = 0;
    virtual void signal_event(uint64_t handle, uint64_t stream_handle, uint64_t fence_value) noexcept = 0;
    virtual void wait_event(uint64_t handle, uint64_t stream_handle, uint64_t fence_value) noexcept = 0;
    virtual bool is_event_completed(uint64_t handle, uint64_t fence_value) const noexcept = 0;
    virtual void synchronize_event(uint64_t handle, uint64_t fence_value) noexcept = 0;

    // accel
    [[nodiscard]] virtual ResourceCreationInfo create_mesh(
        const AccelOption &option) noexcept = 0;
    virtual void destroy_mesh(uint64_t handle) noexcept = 0;

    [[nodiscard]] virtual ResourceCreationInfo create_procedural_primitive(
        const AccelOption &option) noexcept = 0;
    virtual void destroy_procedural_primitive(uint64_t handle) noexcept = 0;

    [[nodiscard]] virtual ResourceCreationInfo create_accel(const AccelOption &option) noexcept = 0;
    virtual void destroy_accel(uint64_t handle) noexcept = 0;

    // query
    [[nodiscard]] virtual std::string query(std::string_view property) noexcept { return {}; }
    [[nodiscard]] virtual DeviceExtension *extension(std::string_view name) noexcept { return nullptr; }
    virtual void set_name(vox::compute::Resource::Tag resource_tag, uint64_t resource_handle, std::string_view name) noexcept = 0;
};

}// namespace vox::compute
