//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "common/logging.h"
#include "common/magic_enum.h"
#include "runtime/mipmap.h"

namespace vox::compute::detail {

void MipmapView::_error_mipmap_copy_buffer_size_mismatch(size_t mip_size, size_t buffer_size) noexcept {
    ERROR_WITH_LOCATION(
        "No enough data (required = {} bytes) in buffer (size = {} bytes).",
        mip_size, buffer_size);
}

MipmapView::MipmapView(uint64_t handle, uint3 size, uint32_t level, PixelStorage storage) noexcept
    : _handle{handle},
      _size{size},
      _level{level},
      _storage{storage} {
    VERBOSE_WITH_LOCATION(
        "Mipmap: size = [{}, {}, {}], storage = {}, level = {}.",
        size.x, size.y, size.z, vox::to_string(storage), level);
}

[[nodiscard]] std::unique_ptr<TextureCopyCommand> MipmapView::copy_from(MipmapView src) const noexcept {
    if (!all(_size == src._size)) [[unlikely]] {
        ERROR_WITH_LOCATION(
            "MipmapView sizes mismatch in copy command "
            "(src: [{}, {}], dest: [{}, {}]).",
            src._size.x, src._size.y, _size.x, _size.y);
    }
    if (src._storage != _storage) [[unlikely]] {
        ERROR_WITH_LOCATION(
            "MipmapView storages mismatch "
            "(src = {}, dst = {})",
            to_underlying(src._storage),
            to_underlying(_storage));
    }
    return std::make_unique<TextureCopyCommand>(
        _storage, src._handle, _handle, src._level, _level, _size);
}

}// namespace vox::compute::detail
