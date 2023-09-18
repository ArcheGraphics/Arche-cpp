//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "runtime/byte_buffer.h"
#include "runtime/device.h"
#include "common/logging.h"

namespace vox::compute {

namespace detail {
void error_buffer_size_not_aligned(size_t align) noexcept {
    ERROR_WITH_LOCATION("Buffer size must be aligned to {}.", align);
}
}// namespace detail

ByteBuffer::ByteBuffer(DeviceInterface *device, const BufferCreationInfo &info) noexcept
    : Resource{device, Tag::BUFFER, info},
      _size_bytes{info.total_size_bytes} {}

ByteBuffer::ByteBuffer(DeviceInterface *device, size_t size_bytes) noexcept
    : ByteBuffer{
          device,
          [&] {
              if (size_bytes == 0) [[unlikely]] {
                  detail::error_buffer_size_is_zero();
              }
              if ((size_bytes & 3) != 0) [[unlikely]] {
                  detail::error_buffer_size_not_aligned(4);
              }
              return device->create_buffer(
                  (size_bytes + sizeof(uint) - 1u) / sizeof(uint));
          }()} {}

ByteBuffer::~ByteBuffer() noexcept {
    if (*this) { device()->destroy_buffer(handle()); }
}

ByteBuffer Device::create_byte_buffer(size_t byte_size) noexcept {
    return ByteBuffer{impl(), byte_size};
}

}// namespace vox::compute
