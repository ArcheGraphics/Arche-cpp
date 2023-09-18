//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <cstdlib>
#include <array>
#include <variant>
#include <span>

#include "common/macro.h"
#include "common/basic_types.h"
#include "runtime/rhi/pixel.h"
#include "runtime/rhi/stream_tag.h"
#include "runtime/rhi/sampler.h"
#include "runtime/rhi/argument.h"
#include "usage.h"

namespace vox::compute {

struct IndirectDispatchArg {
    uint64_t handle;
    uint32_t offset;
    uint32_t max_dispatch_size;
};

#define VOX_COMPUTE_RUNTIME_COMMANDS \
    BufferUploadCommand,             \
        BufferDownloadCommand,       \
        BufferCopyCommand,           \
        BufferToTextureCopyCommand,  \
        ShaderDispatchCommand,       \
        TextureUploadCommand,        \
        TextureDownloadCommand,      \
        TextureCopyCommand,          \
        TextureToBufferCopyCommand,  \
        CustomCommand

#define VOX_MAKE_COMMAND_FWD_DECL(CMD) class CMD;
VOX_MAP(VOX_MAKE_COMMAND_FWD_DECL, VOX_COMPUTE_RUNTIME_COMMANDS)
#undef VOX_MAKE_COMMAND_FWD_DECL

struct CommandVisitor {
#define VOX_MAKE_COMMAND_VISITOR_INTERFACE(CMD) \
    virtual void visit(const CMD *) noexcept = 0;
    VOX_MAP(VOX_MAKE_COMMAND_VISITOR_INTERFACE, VOX_COMPUTE_RUNTIME_COMMANDS)
#undef VOX_MAKE_COMMAND_VISITOR_INTERFACE
    virtual ~CommandVisitor() noexcept = default;
};

struct MutableCommandVisitor {
#define VOX_MAKE_COMMAND_VISITOR_INTERFACE(CMD) \
    virtual void visit(CMD *) noexcept = 0;
    VOX_MAP(VOX_MAKE_COMMAND_VISITOR_INTERFACE, VOX_COMPUTE_RUNTIME_COMMANDS)
#undef VOX_MAKE_COMMAND_VISITOR_INTERFACE
    virtual ~MutableCommandVisitor() noexcept = default;
};

class Command;
class CommandList;

#define VOX_MAKE_COMMAND_COMMON_ACCEPT()                                                  \
    void accept(CommandVisitor &visitor) const noexcept override { visitor.visit(this); } \
    void accept(MutableCommandVisitor &visitor) noexcept override { visitor.visit(this); }

#define VOX_MAKE_COMMAND_COMMON(Type) \
    VOX_MAKE_COMMAND_COMMON_ACCEPT()  \
    StreamTag stream_tag() const noexcept override { return Type; }

class Command {
public:
    enum struct Tag {
#define VOX_MAKE_COMMAND_TAG(Cmd) E##Cmd,
        VOX_MAP(VOX_MAKE_COMMAND_TAG, VOX_COMPUTE_RUNTIME_COMMANDS)
#undef VOX_MAKE_COMMAND_TAG
    };

private:
    Tag _tag;

public:
    explicit Command(Tag tag) noexcept : _tag(tag) {}
    virtual ~Command() noexcept = default;
    virtual void accept(CommandVisitor &visitor) const noexcept = 0;
    virtual void accept(MutableCommandVisitor &visitor) noexcept = 0;
    [[nodiscard]] auto tag() const noexcept { return _tag; }
    [[nodiscard]] virtual StreamTag stream_tag() const noexcept = 0;
};

class ShaderDispatchCommandBase {
public:
    using Argument = vox::compute::Argument;

private:
    uint64_t _handle;
    std::vector<std::byte> _argument_buffer;
    size_t _argument_count;

protected:
    ShaderDispatchCommandBase(uint64_t shader_handle,
                              std::vector<std::byte> &&argument_buffer,
                              size_t argument_count) noexcept
        : _handle{shader_handle},
          _argument_buffer{std::move(argument_buffer)},
          _argument_count{argument_count} {}
    ~ShaderDispatchCommandBase() noexcept = default;

public:
    [[nodiscard]] auto handle() const noexcept { return _handle; }
    [[nodiscard]] auto arguments() const noexcept {
        return std::span{reinterpret_cast<const Argument *>(_argument_buffer.data()), _argument_count};
    }
    [[nodiscard]] auto uniform(const Argument::Uniform &u) const noexcept {
        return std::span{_argument_buffer}.subspan(u.offset, u.size);
    }
};

class ShaderDispatchCommand final : public Command, public ShaderDispatchCommandBase {
public:
    using DispatchSize = std::variant<uint3, IndirectDispatchArg>;

private:
    DispatchSize _dispatch_size;

public:
    ShaderDispatchCommand(uint64_t shader_handle,
                          std::vector<std::byte> &&argument_buffer,
                          size_t argument_count,
                          DispatchSize dispatch_size) noexcept
        : Command{Tag::EShaderDispatchCommand},
          ShaderDispatchCommandBase{shader_handle,
                                    std::move(argument_buffer),
                                    argument_count},
          _dispatch_size{dispatch_size} {}
    ShaderDispatchCommand(ShaderDispatchCommand const &) = delete;
    ShaderDispatchCommand(ShaderDispatchCommand &&) = default;
    [[nodiscard]] auto is_indirect() const noexcept { return std::holds_alternative<IndirectDispatchArg>(_dispatch_size); }
    [[nodiscard]] auto dispatch_size() const noexcept { return std::get<uint3>(_dispatch_size); }
    [[nodiscard]] auto indirect_dispatch() const noexcept { return std::get<IndirectDispatchArg>(_dispatch_size); }
    VOX_MAKE_COMMAND_COMMON(StreamTag::COMPUTE)
};

class BufferUploadCommand final : public Command {
private:
    uint64_t _handle{};
    size_t _offset{};
    size_t _size{};
    const void *_data{};

private:
    BufferUploadCommand() noexcept
        : Command{Command::Tag::EBufferUploadCommand} {}

public:
    BufferUploadCommand(uint64_t handle,
                        size_t offset_bytes,
                        size_t size_bytes,
                        const void *data) noexcept
        : Command{Command::Tag::EBufferUploadCommand},
          _handle{handle}, _offset{offset_bytes}, _size{size_bytes}, _data{data} {}
    [[nodiscard]] auto handle() const noexcept { return _handle; }
    [[nodiscard]] auto offset() const noexcept { return _offset; }
    [[nodiscard]] auto size() const noexcept { return _size; }
    [[nodiscard]] auto data() const noexcept { return _data; }
    VOX_MAKE_COMMAND_COMMON(StreamTag::COPY)
};

class BufferDownloadCommand final : public Command {
private:
    uint64_t _handle{};
    size_t _offset{};
    size_t _size{};
    void *_data{};

private:
    BufferDownloadCommand() noexcept
        : Command{Command::Tag::EBufferDownloadCommand} {}

public:
    BufferDownloadCommand(uint64_t handle, size_t offset_bytes, size_t size_bytes, void *data) noexcept
        : Command{Command::Tag::EBufferDownloadCommand},
          _handle{handle}, _offset{offset_bytes}, _size{size_bytes}, _data{data} {}
    [[nodiscard]] auto handle() const noexcept { return _handle; }
    [[nodiscard]] auto offset() const noexcept { return _offset; }
    [[nodiscard]] auto size() const noexcept { return _size; }
    [[nodiscard]] auto data() const noexcept { return _data; }
    VOX_MAKE_COMMAND_COMMON(StreamTag::COPY)
};

class BufferCopyCommand final : public Command {
private:
    uint64_t _src_handle{};
    uint64_t _dst_handle{};
    size_t _src_offset{};
    size_t _dst_offset{};
    size_t _size{};

private:
    BufferCopyCommand() noexcept
        : Command{Command::Tag::EBufferCopyCommand} {}

public:
    BufferCopyCommand(uint64_t src, uint64_t dst, size_t src_offset, size_t dst_offset, size_t size) noexcept
        : Command{Command::Tag::EBufferCopyCommand},
          _src_handle{src}, _dst_handle{dst},
          _src_offset{src_offset}, _dst_offset{dst_offset}, _size{size} {}
    [[nodiscard]] auto src_handle() const noexcept { return _src_handle; }
    [[nodiscard]] auto dst_handle() const noexcept { return _dst_handle; }
    [[nodiscard]] auto src_offset() const noexcept { return _src_offset; }
    [[nodiscard]] auto dst_offset() const noexcept { return _dst_offset; }
    [[nodiscard]] auto size() const noexcept { return _size; }
    VOX_MAKE_COMMAND_COMMON(StreamTag::COPY)
};

class BufferToTextureCopyCommand final : public Command {
private:
    uint64_t _buffer_handle{};
    size_t _buffer_offset{};
    uint64_t _texture_handle{};
    PixelStorage _pixel_storage{};
    uint _texture_level{};
    uint _texture_offset[3]{};
    uint _texture_size[3]{};

private:
    BufferToTextureCopyCommand() noexcept
        : Command{Command::Tag::EBufferToTextureCopyCommand} {}

public:
    BufferToTextureCopyCommand(uint64_t buffer, size_t buffer_offset,
                               uint64_t texture, PixelStorage storage,
                               uint level, uint3 size, uint3 texture_offset = uint3::zero()) noexcept
        : Command{Command::Tag::EBufferToTextureCopyCommand},
          _buffer_handle{buffer}, _buffer_offset{buffer_offset},
          _texture_handle{texture}, _pixel_storage{storage}, _texture_level{level},
          _texture_offset{texture_offset.x, texture_offset.y, texture_offset.z},
          _texture_size{size.x, size.y, size.z} {}
    [[nodiscard]] auto buffer() const noexcept { return _buffer_handle; }
    [[nodiscard]] auto buffer_offset() const noexcept { return _buffer_offset; }
    [[nodiscard]] auto texture() const noexcept { return _texture_handle; }
    [[nodiscard]] auto storage() const noexcept { return _pixel_storage; }
    [[nodiscard]] auto level() const noexcept { return _texture_level; }
    [[nodiscard]] auto texture_offset() const noexcept { return uint3(_texture_offset[0], _texture_offset[1], _texture_offset[2]); }
    [[nodiscard]] auto size() const noexcept { return uint3(_texture_size[0], _texture_size[1], _texture_size[2]); }
    VOX_MAKE_COMMAND_COMMON(StreamTag::COPY)
};

class TextureToBufferCopyCommand final : public Command {
private:
    uint64_t _buffer_handle{};
    size_t _buffer_offset{};
    uint64_t _texture_handle{};
    PixelStorage _pixel_storage{};
    uint _texture_level{};
    uint _texture_offset[3]{};
    uint _texture_size[3]{};

private:
    TextureToBufferCopyCommand() noexcept
        : Command{Command::Tag::ETextureToBufferCopyCommand} {}

public:
    TextureToBufferCopyCommand(uint64_t buffer, size_t buffer_offset,
                               uint64_t texture, PixelStorage storage,
                               uint level, uint3 size, uint3 texture_offset = uint3::zero()) noexcept
        : Command{Command::Tag::ETextureToBufferCopyCommand},
          _buffer_handle{buffer}, _buffer_offset{buffer_offset},
          _texture_handle{texture}, _pixel_storage{storage}, _texture_level{level},
          _texture_offset{texture_offset.x, texture_offset.y, texture_offset.z},
          _texture_size{size.x, size.y, size.z} {}
    [[nodiscard]] auto buffer() const noexcept { return _buffer_handle; }
    [[nodiscard]] auto buffer_offset() const noexcept { return _buffer_offset; }
    [[nodiscard]] auto texture() const noexcept { return _texture_handle; }
    [[nodiscard]] auto storage() const noexcept { return _pixel_storage; }
    [[nodiscard]] auto level() const noexcept { return _texture_level; }
    [[nodiscard]] auto texture_offset() const noexcept { return uint3(_texture_offset[0], _texture_offset[1], _texture_offset[2]); }
    [[nodiscard]] auto size() const noexcept { return uint3(_texture_size[0], _texture_size[1], _texture_size[2]); }
    VOX_MAKE_COMMAND_COMMON(StreamTag::COPY)
};

class TextureCopyCommand final : public Command {
private:
    PixelStorage _storage{};
    uint64_t _src_handle{};
    uint64_t _dst_handle{};
    uint _src_offset[3]{};
    uint _dst_offset[3]{};
    uint _size[3]{};
    uint _src_level{};
    uint _dst_level{};

private:
    TextureCopyCommand() noexcept
        : Command{Command::Tag::ETextureCopyCommand} {}

public:
    TextureCopyCommand(PixelStorage storage, uint64_t src_handle, uint64_t dst_handle,
                       uint src_level, uint dst_level, uint3 size, uint3 src_offset = uint3::zero(), uint3 dst_offset = uint3::zero()) noexcept
        : Command{Command::Tag::ETextureCopyCommand},
          _storage{storage}, _src_handle{src_handle}, _dst_handle{dst_handle},
          _src_offset{src_offset.x, src_offset.y, src_offset.z},
          _dst_offset{dst_offset.x, dst_offset.y, dst_offset.z},
          _size{size.x, size.y, size.z}, _src_level{src_level}, _dst_level{dst_level} {}
    [[nodiscard]] auto storage() const noexcept { return _storage; }
    [[nodiscard]] auto src_handle() const noexcept { return _src_handle; }
    [[nodiscard]] auto dst_handle() const noexcept { return _dst_handle; }
    [[nodiscard]] auto size() const noexcept { return uint3(_size[0], _size[1], _size[2]); }
    [[nodiscard]] auto src_level() const noexcept { return _src_level; }
    [[nodiscard]] auto src_offset() const noexcept { return _src_offset; }
    [[nodiscard]] auto dst_offset() const noexcept { return _dst_offset; }
    [[nodiscard]] auto dst_level() const noexcept { return _dst_level; }
    VOX_MAKE_COMMAND_COMMON(StreamTag::COPY)
};

class TextureUploadCommand final : public Command {
private:
    uint64_t _handle{};
    PixelStorage _storage{};
    uint _level{};
    // only for sparse texture
    uint _offset[3]{};
    uint _size[3]{};
    const void *_data{};

private:
    TextureUploadCommand() noexcept
        : Command{Command::Tag::ETextureUploadCommand} {}

public:
    TextureUploadCommand(uint64_t handle, PixelStorage storage,
                         uint level, uint3 size, const void *data, uint3 offset = uint3::zero()) noexcept
        : Command{Command::Tag::ETextureUploadCommand},
          _handle{handle}, _storage{storage}, _level{level},
          _offset{offset.x, offset.y, offset.z},
          _size{size.x, size.y, size.z},
          _data{data} {}
    [[nodiscard]] auto handle() const noexcept { return _handle; }
    [[nodiscard]] auto storage() const noexcept { return _storage; }
    [[nodiscard]] auto level() const noexcept { return _level; }
    [[nodiscard]] auto size() const noexcept { return uint3(_size[0], _size[1], _size[2]); }
    [[nodiscard]] auto offset() const noexcept { return uint3(_offset[0], _offset[1], _offset[2]); }
    [[nodiscard]] auto data() const noexcept { return _data; }
    VOX_MAKE_COMMAND_COMMON(StreamTag::COPY)
};

class TextureDownloadCommand final : public Command {
private:
    uint64_t _handle{};
    PixelStorage _storage{};
    uint _level{};
    // only for sparse texture
    uint _offset[3]{};
    uint _size[3]{};
    void *_data{};

private:
    TextureDownloadCommand() noexcept
        : Command{Command::Tag::ETextureDownloadCommand} {}

public:
    TextureDownloadCommand(uint64_t handle, PixelStorage storage,
                           uint level, uint3 size, void *data, uint3 offset = uint3::zero()) noexcept
        : Command{Command::Tag::ETextureDownloadCommand},
          _handle{handle}, _storage{storage}, _level{level},
          _offset{offset.x, offset.y, offset.z},
          _size{size.x, size.y, size.z},
          _data{data} {}
    [[nodiscard]] auto handle() const noexcept { return _handle; }
    [[nodiscard]] auto storage() const noexcept { return _storage; }
    [[nodiscard]] auto level() const noexcept { return _level; }
    [[nodiscard]] auto size() const noexcept { return uint3(_size[0], _size[1], _size[2]); }
    [[nodiscard]] auto offset() const noexcept { return uint3(_offset[0], _offset[1], _offset[2]); }
    [[nodiscard]] auto data() const noexcept { return _data; }
    VOX_MAKE_COMMAND_COMMON(StreamTag::COPY)
};

class CustomCommand : public Command {
public:
    explicit CustomCommand() noexcept
        : Command{Command::Tag::ECustomCommand} {}
    [[nodiscard]] virtual uint64_t uuid() const noexcept = 0;
    ~CustomCommand() noexcept override = default;
    VOX_MAKE_COMMAND_COMMON_ACCEPT()
};

// For custom shader-dispatch or pass
class CustomDispatchCommand : public CustomCommand {
public:
    using ResourceHandle = std::variant<
        Argument::Buffer,
        Argument::Texture>;

    class ArgumentVisitor {
    public:
        ~ArgumentVisitor() noexcept = default;
        virtual void visit(const ResourceHandle &resource, Usage usage) noexcept = 0;
    };

public:
    explicit CustomDispatchCommand() noexcept = default;
    ~CustomDispatchCommand() noexcept override = default;

    virtual void traverse_arguments(ArgumentVisitor &visitor) const noexcept = 0;

    template<typename F>
        requires(!std::derived_from<std::remove_cvref_t<F>, ArgumentVisitor>)
    void traverse_arguments(F &&f) const noexcept {
        class Adapter final : public ArgumentVisitor {
        private:
            F &_f;

        public:
            explicit Adapter(F &f) noexcept : _f{f} {}
            void visit(const CustomDispatchCommand::ResourceHandle &resource, Usage usage) noexcept override {
                _f(resource, usage);
            }
        };
        Adapter adapter{f};
        this->traverse_arguments(adapter);
    }
};

}// namespace vox::compute
