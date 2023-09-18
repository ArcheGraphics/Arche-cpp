//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "common/logging.h"
#include "metal_buffer.h"

namespace vox::compute::metal {

metal::MetalBuffer::MetalBuffer(MTL::Device *device, size_t size) noexcept
    : _handle{device->newBuffer(size, MTL::ResourceStorageModePrivate |
                                          MTL::ResourceHazardTrackingModeTracked)} {}

metal::MetalBuffer::~MetalBuffer() noexcept {
    _handle->release();
}

MetalBuffer::Binding MetalBuffer::binding(size_t offset, size_t size) const noexcept {
    ASSERT(offset + size <= _handle->length(), "Offset out of range.");
    return {_handle->gpuAddress() + offset, size};
}

void MetalBuffer::set_name(std::string_view name) noexcept {
    if (name.empty()) {
        _handle->setLabel(nullptr);
    } else {
        auto mtl_name = NS::String::alloc()->init(
            const_cast<char *>(name.data()), name.size(),
            NS::UTF8StringEncoding, false);
        _handle->setLabel(mtl_name);
        mtl_name->release();
    }
}

MetalIndirectDispatchBuffer::MetalIndirectDispatchBuffer(
    MTL::Device *device, size_t capacity) noexcept
    : _dispatch_buffer{nullptr},
      _command_buffer{nullptr},
      _capacity{capacity} {

    auto dispatch_buffer_size = sizeof(Header) + sizeof(Dispatch) * capacity;
    _dispatch_buffer = device->newBuffer(dispatch_buffer_size,
                                         MTL::ResourceStorageModePrivate |
                                             MTL::ResourceHazardTrackingModeTracked);
    ASSERT(_dispatch_buffer != nullptr,
           "Failed to create indirect dispatch buffer.");

    auto command_buffer_desc = MTL::IndirectCommandBufferDescriptor::alloc()->init();
    command_buffer_desc->setCommandTypes(MTL::IndirectCommandTypeConcurrentDispatch);
    command_buffer_desc->setInheritPipelineState(false);
    command_buffer_desc->setInheritBuffers(false);
    command_buffer_desc->setMaxVertexBufferBindCount(0);
    command_buffer_desc->setMaxFragmentBufferBindCount(0);
    command_buffer_desc->setMaxKernelBufferBindCount(2);
    command_buffer_desc->setSupportRayTracing(true);
    _command_buffer = device->newIndirectCommandBuffer(command_buffer_desc, capacity,
                                                       MTL::ResourceStorageModePrivate |
                                                           MTL::ResourceHazardTrackingModeTracked);
    ASSERT(_command_buffer != nullptr,
           "Failed to create indirect command buffer.");
    command_buffer_desc->release();
}

MetalIndirectDispatchBuffer::~MetalIndirectDispatchBuffer() noexcept {
    _dispatch_buffer->release();
    _command_buffer->release();
}

[[nodiscard]] MetalIndirectDispatchBuffer::Binding
MetalIndirectDispatchBuffer::binding(size_t offset, size_t count) const noexcept {
    count = std::min(count, std::numeric_limits<size_t>::max() - offset);// prevent overflow
    return {_dispatch_buffer->gpuAddress(),
            static_cast<uint>(offset),
            static_cast<uint>(std::min(offset + count, _capacity))};
}

void MetalIndirectDispatchBuffer::set_name(std::string_view name) noexcept {
    auto do_set_name = [base_name = name](auto handle, auto postfix) noexcept {
        auto name = fmt::format("{} ({})", base_name, postfix);
        if (name.empty()) {
            handle->setLabel(nullptr);
        } else {
            auto mtl_name = NS::String::alloc()->init(
                const_cast<char *>(name.data()), name.size(),
                NS::UTF8StringEncoding, false);
            handle->setLabel(mtl_name);
            mtl_name->release();
        }
    };
    do_set_name(_dispatch_buffer, "dispatch");
    do_set_name(_command_buffer, "command");
}

}// namespace vox::compute::metal
