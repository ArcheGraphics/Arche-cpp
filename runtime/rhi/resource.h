//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <string>
#include <memory>
#include "runtime/rhi/pixel.h"

namespace vox::compute {

class DeviceInterface;
class Device;

constexpr auto invalid_resource_handle = ~0ull;

struct ResourceCreationInfo {
    uint64_t handle;
    void *native_handle;

    [[nodiscard]] constexpr auto valid() const noexcept { return handle != invalid_resource_handle; }

    void invalidate() noexcept {
        handle = invalid_resource_handle;
        native_handle = nullptr;
    }

    [[nodiscard]] static constexpr auto make_invalid() noexcept {
        return ResourceCreationInfo{invalid_resource_handle, nullptr};
    }
};

struct BufferCreationInfo : public ResourceCreationInfo {
    size_t element_stride;
    size_t total_size_bytes;
    [[nodiscard]] static constexpr auto make_invalid() noexcept {
        BufferCreationInfo info{
            .element_stride = 0,
            .total_size_bytes = 0};
        info.handle = invalid_resource_handle;
        info.native_handle = nullptr;
        return info;
    }
};

struct SwapchainCreationInfo : public ResourceCreationInfo {
    PixelStorage storage;
};

struct ShaderCreationInfo : public ResourceCreationInfo {
    // vox::string name;
    uint3 block_size;

    [[nodiscard]] static auto make_invalid() noexcept {
        ShaderCreationInfo info{};
        info.invalidate();
        return info;
    }
};

/// \brief Options for shader creation.
struct ShaderOption {
    /// \brief Whether to enable shader cache.
    /// \details LuisaCompute uses shader cache to avoid redundant shader
    ///   compilation. Cache read/write behaviors are controlled by the
    ///   `read_shader_cache` and `write_shader_cache` methods in `BinaryIO`
    ///   passed via `class DeviceConfig` to backends on device creation.
    ///   This field has no effects if a user-defined `name` is provided.
    /// \sa DeviceConfig
    /// \sa BinaryIO
    bool enable_cache{true};
    /// \brief Whether to enable fast math.
    bool enable_fast_math{true};
    /// \brief Whether to enable debug info.
    bool enable_debug_info{false};
    /// \brief Whether to create the shader object.
    /// \details No shader object will be created if this field is set to
    ///   `true`. This field is useful for AOT compilation.
    bool compile_only{false};
    /// \brief A user-defined name for the shader.
    /// \details If provided, the shader will be read from or written to disk
    ///   via the `BinaryIO` object (passed to backends on device creation)
    ///   through the `read_shader_bytecode` and `write_shader_bytecode` methods.
    ///   The `enable_cache` field will be ignored if this field is not empty.
    /// \sa DeviceConfig
    /// \sa BinaryIO
    std::string name;
    /// \brief Include code written in the native shading language.
    /// \details If provided, backend will include this string into the generated
    ///   shader code. This field is useful for interoperation with external callables.
    /// \sa ExternalCallable
    std::string native_include;
};

class Resource {
    friend class Device;

public:
    enum struct Tag : uint32_t {
        BUFFER,
        TEXTURE,
        EVENT,
        SHADER,
        SWAP_CHAIN,
        DSTORAGE_FILE,
        DSTORAGE_PINNED_MEMORY
    };

private:
    std::shared_ptr<DeviceInterface> _device{nullptr};
    ResourceCreationInfo _info{};
    Tag _tag{};

private:
    static void _error_invalid() noexcept;

protected:
    static void _check_same_derived_types(const Resource &lhs,
                                          const Resource &rhs) noexcept;

    // helper method for derived classes to implement move assignment
    template<typename Derived>
    void _move_from(Derived &&rhs) noexcept {
        if (this != &rhs) [[likely]] {
            // check if the two resources are compatible if both are valid
            _check_same_derived_types(*this, rhs);
            using Self = std::remove_cvref_t<Derived>;
            static_assert(std::is_base_of_v<Resource, Self> &&
                              !std::is_same_v<Resource, Self>,
                          "Resource::_move_from can only be used in derived classes");
            auto self = static_cast<Self *>(this);
            // destroy the old resource
            self->~Self();
            // move the new resource
            new (std::launder(self)) Self{static_cast<Self &&>(rhs)};
        }
    }

    void _check_is_valid() const noexcept {
#ifndef NDEBUG
        if (!*this) [[unlikely]] { _error_invalid(); }
#endif
    }

protected:
    // protected constructors for derived classes
    Resource() noexcept { _info.invalidate(); }
    Resource(DeviceInterface *device, Tag tag, const ResourceCreationInfo &info) noexcept;
    Resource(Resource &&) noexcept;
    // protected destructor for derived classes

public:
    virtual ~Resource() noexcept = default;
    Resource(const Resource &) noexcept = delete;
    Resource &operator=(Resource &&) noexcept = delete;// use _move_from in derived classes
    Resource &operator=(const Resource &) noexcept = delete;
    [[nodiscard]] auto device() const noexcept { return _device.get(); }
    [[nodiscard]] auto handle() const noexcept { return _info.handle; }
    [[nodiscard]] auto native_handle() const noexcept { return _info.native_handle; }
    [[nodiscard]] auto tag() const noexcept { return _tag; }
    [[nodiscard]] explicit operator bool() const noexcept { return _info.valid(); }
    void set_name(std::string_view name) const noexcept;
};

}// namespace vox::compute
