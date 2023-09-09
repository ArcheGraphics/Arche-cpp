//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "buffer_utils.h"
#include "common/metal_helpers.h"

namespace vox::compute {
void set_device_buffer_via_staging_buffer(
    MTL::Device &device, MTL::Buffer &device_buffer,
    size_t buffer_size_in_bytes,
    const std::function<void(void *, size_t)> &staging_buffer_setter) {
    auto stage_buffer = make_shared(device.newBuffer(buffer_size_in_bytes, MTL::ResourceStorageModeShared));
    staging_buffer_setter(stage_buffer->contents(), buffer_size_in_bytes);

    auto queue = make_shared(device.newCommandQueue());
    auto commandBuffer = make_shared(queue->commandBuffer());
    auto blit = make_shared(commandBuffer->blitCommandEncoder());
    blit->copyFromBuffer(stage_buffer.get(), 0, &device_buffer, 0, buffer_size_in_bytes);
    blit->endEncoding();
    commandBuffer->commit();
    commandBuffer->waitUntilCompleted();
}

void get_device_buffer_via_staging_buffer(
    MTL::Device &device, MTL::Buffer &device_buffer,
    size_t buffer_size_in_bytes,
    const std::function<void(void *, size_t)> &staging_buffer_getter) {
    auto stage_buffer = make_shared(device.newBuffer(buffer_size_in_bytes, MTL::ResourceStorageModeShared));

    auto queue = make_shared(device.newCommandQueue());
    auto commandBuffer = make_shared(queue->commandBuffer());
    auto blit = make_shared(commandBuffer->blitCommandEncoder());
    blit->copyFromBuffer(&device_buffer, 0, stage_buffer.get(), 0, buffer_size_in_bytes);
    blit->endEncoding();
    commandBuffer->commit();
    commandBuffer->waitUntilCompleted();

    staging_buffer_getter(stage_buffer->contents(), buffer_size_in_bytes);
}

}// namespace vox::compute