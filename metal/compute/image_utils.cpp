//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "image_utils.h"
#include "common/metal_helpers.h"

namespace vox::compute {
void set_device_image_via_staging_buffer(
    MTL::Device &device, MTL::Texture &device_image,
    MTL::Size image_dimensions,
    size_t source_bytes_per_row,
    size_t buffer_size_in_bytes,
    const std::function<void(void *, size_t)> &staging_buffer_setter) {
    auto stage_buffer = make_shared(device.newBuffer(buffer_size_in_bytes, MTL::ResourceStorageModeShared));
    staging_buffer_setter(stage_buffer->contents(), buffer_size_in_bytes);

    auto queue = make_shared(device.newCommandQueue());
    auto commandBuffer = make_shared(queue->commandBuffer());
    auto blit = make_shared(commandBuffer->blitCommandEncoder());
    blit->copyFromBuffer(stage_buffer.get(), 0, source_bytes_per_row, buffer_size_in_bytes, image_dimensions,
                         &device_image, 0, 0, MTL::Origin());
    blit->endEncoding();
    commandBuffer->commit();
    commandBuffer->waitUntilCompleted();
}

void get_device_image_via_staging_buffer(
    MTL::Device &device, MTL::Texture &device_image,
    MTL::Size image_dimensions,
    size_t destination_bytes_per_row,
    size_t buffer_size_in_bytes,
    const std::function<void(void *, size_t)> &staging_buffer_getter) {
    auto stage_buffer = make_shared(device.newBuffer(buffer_size_in_bytes, MTL::ResourceStorageModeShared));

    auto queue = make_shared(device.newCommandQueue());
    auto commandBuffer = make_shared(queue->commandBuffer());
    auto blit = make_shared(commandBuffer->blitCommandEncoder());
    blit->copyFromTexture(&device_image, 0, 0, MTL::Origin(), image_dimensions,
                          stage_buffer.get(), 0, destination_bytes_per_row, buffer_size_in_bytes);
    blit->endEncoding();
    commandBuffer->commit();
    commandBuffer->waitUntilCompleted();

    staging_buffer_getter(stage_buffer->contents(), buffer_size_in_bytes);
}

}// namespace vox::compute