//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "common/logging.h"
#include "runtime/rhi/command.h"
#include "runtime/rhi/command_encoder.h"
#include <numeric>

namespace vox::compute {

std::byte *ShaderDispatchCmdEncoder::_make_space(size_t size) noexcept {
    auto offset = _argument_buffer.size();
    _argument_buffer.resize(offset + size);
    return _argument_buffer.data() + offset;
}

ShaderDispatchCmdEncoder::ShaderDispatchCmdEncoder(
    uint64_t handle,
    size_t arg_count,
    size_t uniform_size) noexcept
    : _handle{handle}, _argument_count{arg_count} {
    if (auto arg_size_bytes = arg_count * sizeof(Argument)) {
        _argument_buffer.reserve(arg_size_bytes + uniform_size);
        _argument_buffer.resize(arg_size_bytes);
    }
}

ShaderDispatchCmdEncoder::Argument &ShaderDispatchCmdEncoder::_create_argument() noexcept {
    auto idx = _argument_idx;
    _argument_idx++;
    return *std::launder(reinterpret_cast<Argument *>(_argument_buffer.data()) + idx);
}

void ShaderDispatchCmdEncoder::_encode_buffer(uint64_t handle, size_t offset, size_t size) noexcept {
    auto &&arg = _create_argument();
    arg.tag = Argument::Tag::BUFFER;
    arg.buffer = ShaderDispatchCommandBase::Argument::Buffer{handle, offset, size};
}

void ShaderDispatchCmdEncoder::_encode_texture(uint64_t handle, uint32_t level) noexcept {
    auto &&arg = _create_argument();
    arg.tag = Argument::Tag::TEXTURE;
    arg.texture = ShaderDispatchCommandBase::Argument::Texture{handle, level};
}

void ShaderDispatchCmdEncoder::_encode_uniform(const void *data, size_t size) noexcept {
    auto offset = _argument_buffer.size();
    for (int i = 0; i < size; ++i) {
        _argument_buffer.push_back(std::byte());
    }
    std::memcpy(_argument_buffer.data() + offset, data, size);
    auto &&arg = _create_argument();
    arg.tag = Argument::Tag::UNIFORM;
    arg.uniform.offset = offset;
    arg.uniform.size = size;
}

void ComputeDispatchCmdEncoder::set_dispatch_size(uint3 launch_size) noexcept {
    _dispatch_size = launch_size;
}

void ComputeDispatchCmdEncoder::set_dispatch_size(IndirectDispatchArg indirect_arg) noexcept {
    _dispatch_size = indirect_arg;
}

ComputeDispatchCmdEncoder::ComputeDispatchCmdEncoder(uint64_t handle, size_t arg_count, size_t uniform_size) noexcept
    : ShaderDispatchCmdEncoder{handle, arg_count, uniform_size} {}

void ComputeDispatchCmdEncoder::encode_buffer(uint64_t handle, size_t offset, size_t size) noexcept {
    _encode_buffer(handle, offset, size);
}

void ComputeDispatchCmdEncoder::encode_texture(uint64_t handle, uint32_t level) noexcept {
    _encode_texture(handle, level);
}

void ComputeDispatchCmdEncoder::encode_uniform(const void *data, size_t size) noexcept {
    _encode_uniform(data, size);
}

std::unique_ptr<ShaderDispatchCommand> ComputeDispatchCmdEncoder::build() && noexcept {
    if (_argument_idx != _argument_count) [[unlikely]] {
        LOGE("Required argument count {}. "
             "Actual argument count {}.",
             _argument_count, _argument_idx);
    }
    return std::make_unique<ShaderDispatchCommand>(
        _handle, std::move(_argument_buffer),
        _argument_count, _dispatch_size);
}

}// namespace vox::compute
