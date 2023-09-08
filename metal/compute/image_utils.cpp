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
    auto stage_buffer = CLONE_METAL_CUSTOM_DELETER(MTL::Buffer, device.newBuffer(buffer_size_in_bytes, MTL::ResourceStorageModeShared));
    staging_buffer_setter(stage_buffer->contents(), buffer_size_in_bytes);

    auto queue = CLONE_METAL_CUSTOM_DELETER(MTL::CommandQueue, device.newCommandQueue());
    auto commandBuffer = CLONE_METAL_CUSTOM_DELETER(MTL::CommandBuffer, queue->commandBuffer());
    auto blit = CLONE_METAL_CUSTOM_DELETER(MTL::BlitCommandEncoder, commandBuffer->blitCommandEncoder());
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
    auto stage_buffer = CLONE_METAL_CUSTOM_DELETER(MTL::Buffer, device.newBuffer(buffer_size_in_bytes, MTL::ResourceStorageModeShared));

    auto queue = CLONE_METAL_CUSTOM_DELETER(MTL::CommandQueue, device.newCommandQueue());
    auto commandBuffer = CLONE_METAL_CUSTOM_DELETER(MTL::CommandBuffer, queue->commandBuffer());
    auto blit = CLONE_METAL_CUSTOM_DELETER(MTL::BlitCommandEncoder, commandBuffer->blitCommandEncoder());
    blit->copyFromTexture(&device_image, 0, 0, MTL::Origin(), image_dimensions,
                          stage_buffer.get(), 0, destination_bytes_per_row, buffer_size_in_bytes);
    blit->endEncoding();
    commandBuffer->commit();
    commandBuffer->waitUntilCompleted();

    staging_buffer_getter(stage_buffer->contents(), buffer_size_in_bytes);
}

}// namespace vox::compute