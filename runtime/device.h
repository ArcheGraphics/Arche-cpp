//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "runtime/rhi/device_interface.h"

namespace vox::compute {
class Context;
class Event;
class TimelineEvent;
class Stream;
class Swapchain;
class IndirectDispatchBuffer;
class ByteBuffer;

template<typename T>
class SOA;

template<typename T>
class Buffer;

template<typename T>
class Image;

template<typename T>
class Volume;

template<size_t dim, typename... Args>
class Shader;

template<size_t dim, typename... Args>
class AOTShader;

template<size_t N, typename... Args>
class Kernel;

template<typename... Args>
struct Kernel1D;

template<typename... Args>
struct Kernel2D;

template<typename... Args>
struct Kernel3D;

namespace detail {

template<typename T>
struct is_dsl_kernel : std::false_type {};

template<size_t N, typename... Args>
struct is_dsl_kernel<Kernel<N, Args...>> : std::true_type {};

template<typename... Args>
struct is_dsl_kernel<Kernel1D<Args...>> : std::true_type {};

template<typename... Args>
struct is_dsl_kernel<Kernel2D<Args...>> : std::true_type {};

template<typename... Args>
struct is_dsl_kernel<Kernel3D<Args...>> : std::true_type {};

}// namespace detail

template<typename T>
concept device_extension = std::is_base_of_v<DeviceExtension, T> &&
                           std::is_same_v<const std::string_view, decltype(T::name)>;

class Device {

public:
    using Deleter = void(DeviceInterface *);
    using Creator = DeviceInterface *(Context && /* context */, const DeviceConfig * /* properties */);
    using Handle = std::shared_ptr<DeviceInterface>;

private:
    Handle _impl;

    template<typename T, typename... Args>
    [[nodiscard]] auto _create(Args &&...args) noexcept {
        return T{this->_impl.get(), std::forward<Args>(args)...};
    }

public:
    Device() noexcept = default;
    // Device construct from backend handle, use Context::create_device for convenience
    explicit Device(Handle handle) noexcept : _impl{std::move(handle)} {}
    // The backend name in lower case, can be used to recognize the corresponding backend
    [[nodiscard]] auto backend_name() const noexcept { return _impl->backend_name(); }
    // The backend implementation, can be used by other frontend language
    [[nodiscard]] auto impl() const noexcept { return _impl.get(); }
    [[nodiscard]] auto compute_warp_size() const noexcept { return _impl->compute_warp_size(); }
    // Is device initialized
    [[nodiscard]] explicit operator bool() const noexcept { return static_cast<bool>(_impl); }
    // backend native plugins & extensions interface
    template<device_extension Ext>
    [[nodiscard]] auto extension() const noexcept {
        return static_cast<Ext *>(_impl->extension(Ext::name));
    }
    // see definition in runtime/stream.cpp
    [[nodiscard]] Stream create_stream(StreamTag stream_tag = StreamTag::COMPUTE) noexcept;
    // see definition in runtime/event.cpp
    [[nodiscard]] Event create_event() noexcept;
    // see definition in runtime/event.cpp
    [[nodiscard]] TimelineEvent create_timeline_event() noexcept;
    // see definition in runtime/swap_chain.cpp
    [[nodiscard]] Swapchain create_swapchain(
        uint64_t window_handle, const Stream &stream, uint2 resolution,
        bool allow_hdr = true, bool vsync = true, uint back_buffer_count = 1) noexcept;
    // see definition in runtime/dispatch_buffer.cpp
    [[nodiscard]] IndirectDispatchBuffer create_indirect_dispatch_buffer(size_t capacity) noexcept;

    template<typename T>
    [[nodiscard]] auto create_image(PixelStorage pixel, uint width, uint height, uint mip_levels = 1u, bool simultaneous_access = false) noexcept {
        return _create<Image<T>>(pixel, make_uint2(width, height), mip_levels, simultaneous_access);
    }

    template<typename T>
    [[nodiscard]] auto create_image(PixelStorage pixel, uint2 size, uint mip_levels = 1u, bool simultaneous_access = false) noexcept {
        return _create<Image<T>>(pixel, size, mip_levels, simultaneous_access);
    }

    template<typename T>
    [[nodiscard]] auto create_volume(PixelStorage pixel, uint width, uint height, uint depth, uint mip_levels = 1u, bool simultaneous_access = false) noexcept {
        return _create<Volume<T>>(pixel, make_uint3(width, height, depth), mip_levels, simultaneous_access);
    }

    template<typename T>
    [[nodiscard]] auto create_volume(PixelStorage pixel, uint3 size, uint mip_levels = 1u, bool simultaneous_access = false) noexcept {
        return _create<Volume<T>>(pixel, size, mip_levels, simultaneous_access);
    }

    [[nodiscard]] ByteBuffer create_byte_buffer(size_t byte_size) noexcept;

    template<typename T>
    [[nodiscard]] auto create_buffer(size_t size) noexcept {
        return _create<Buffer<T>>(size);
    }

    template<typename T>
    [[nodiscard]] auto create_soa(size_t size) noexcept {
        return SOA<T>{*this, size};
    }

    template<size_t N, typename... Args>
    [[nodiscard]] auto compile(const Kernel<N, Args...> &kernel,
                               const ShaderOption &option = {}) noexcept {
        return _create<Shader<N, Args...>>(kernel.function()->function(), option);
    }

    template<typename Kernel>
    void compile_to(Kernel &&kernel,
                    std::string_view name,
                    bool enable_fast_math = true,
                    bool enable_debug_info = false) noexcept {
        ShaderOption option{
            .enable_cache = false,
            .enable_fast_math = enable_fast_math,
            .enable_debug_info = enable_debug_info,
            .compile_only = true,
            .name = std::string{name}};
        static_cast<void>(this->compile(std::forward<Kernel>(kernel), option));
    }

    template<size_t N, typename Func>
        requires(std::negation_v<detail::is_dsl_kernel<std::remove_cvref_t<Func>>> && N >= 1 && N <= 3)
    [[nodiscard]] auto compile(Func &&f, const ShaderOption &option = {}) noexcept {
        if constexpr (N == 1u) {
            return compile(Kernel1D{std::forward<Func>(f)}, option);
        } else if constexpr (N == 2u) {
            return compile(Kernel2D{std::forward<Func>(f)}, option);
        } else {
            return compile(Kernel3D{std::forward<Func>(f)}, option);
        }
    }

    template<size_t N, typename Kernel>
    void compile_to(Kernel &&kernel,
                    std::string_view name,
                    bool enable_fast_math = true,
                    bool enable_debug_info = false) noexcept {
        ShaderOption option{
            .enable_cache = false,
            .enable_fast_math = enable_fast_math,
            .enable_debug_info = enable_debug_info,
            .compile_only = true,
            .name = std::string{name}};
        static_cast<void>(this->compile<N>(std::forward<Kernel>(kernel), option));
    }

    template<size_t N, typename... Args>
    [[nodiscard]] auto load_shader(std::string_view shader_name) noexcept {
        return _create<Shader<N, Args...>>(shader_name);
    }

    [[nodiscard]] auto query(std::string_view meta_expr) const noexcept {
        return _impl->query(meta_expr);
    }

    template<typename T, typename... Args>
    [[nodiscard]] auto create(Args &&...args) noexcept {
        return _create<T>(std::forward<Args>(args)...);
    }
};

}// namespace vox::compute
