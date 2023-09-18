#pragma once

#include "ast/function.h"
#include "runtime/rhi/command.h"

namespace vox::compute {

class ShaderDispatchCmdEncoder {

public:
    using Argument = ShaderDispatchCommandBase::Argument;

protected:
    uint64_t _handle;
    size_t _argument_count;
    size_t _argument_idx{0};
    std::vector<std::byte> _argument_buffer;
    ShaderDispatchCmdEncoder(uint64_t handle,
                             size_t arg_count,
                             size_t uniform_size) noexcept;
    void _encode_buffer(uint64_t handle, size_t offset, size_t size) noexcept;
    void _encode_texture(uint64_t handle, uint32_t level) noexcept;
    void _encode_uniform(const void *data, size_t size) noexcept;
    void _encode_bindless_array(uint64_t handle) noexcept;
    void _encode_accel(uint64_t handle) noexcept;
    [[nodiscard]] std::byte *_make_space(size_t size) noexcept;
    [[nodiscard]] Argument &_create_argument() noexcept;

public:
    [[nodiscard]] static size_t compute_uniform_size(std::span<const Variable> arguments) noexcept;
    [[nodiscard]] static size_t compute_uniform_size(std::span<const Type *const> arg_types) noexcept;
};

class ComputeDispatchCmdEncoder final : public ShaderDispatchCmdEncoder {

private:
    std::variant<uint3, IndirectDispatchArg> _dispatch_size;

public:
    explicit ComputeDispatchCmdEncoder(uint64_t handle, size_t arg_count, size_t uniform_size) noexcept;
    ComputeDispatchCmdEncoder(ComputeDispatchCmdEncoder &&) noexcept = default;
    ComputeDispatchCmdEncoder &operator=(ComputeDispatchCmdEncoder &&) noexcept = default;
    ~ComputeDispatchCmdEncoder() noexcept = default;
    void set_dispatch_size(uint3 launch_size) noexcept;
    void set_dispatch_size(IndirectDispatchArg indirect_arg) noexcept;

    void encode_buffer(uint64_t handle, size_t offset, size_t size) noexcept;
    void encode_texture(uint64_t handle, uint32_t level) noexcept;
    void encode_uniform(const void *data, size_t size) noexcept;
    void encode_bindless_array(uint64_t handle) noexcept;
    void encode_accel(uint64_t handle) noexcept;
    std::unique_ptr<ShaderDispatchCommand> build() && noexcept;
};

}// namespace vox::compute
