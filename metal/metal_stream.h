//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "runtime/rhi/stream_tag.h"
#include "runtime/command_list.h"
#include "metal_api.h"
#include "metal_stage_buffer_pool.h"


extern void luisa_compute_metal_stream_print_function_logs(MTL::LogContainer *logs);

namespace vox::compute::metal {

class MetalEvent;
class MetalTexture;
class MetalSwapchain;
class MetalCommandEncoder;

class MetalStream {
private:
    MTL::CommandQueue *_queue;
    spin_mutex _upload_pool_creation_mutex;
    spin_mutex _download_pool_creation_mutex;
    spin_mutex _callback_mutex;
    spin_mutex _dispatch_mutex;
    std::unique_ptr<MetalStageBufferPool> _upload_pool;
    std::unique_ptr<MetalStageBufferPool> _download_pool;

protected:
    void _do_dispatch(MetalCommandEncoder &encoder, CommandList &&list) noexcept;
    virtual void _encode(MetalCommandEncoder &encoder, Command *command) noexcept;

public:
    MetalStream(MTL::Device *device, size_t max_commands) noexcept;
    virtual ~MetalStream() noexcept;
    virtual void signal(MetalEvent *event, uint64_t value) noexcept;
    virtual void wait(MetalEvent *event, uint64_t value) noexcept;
    virtual void synchronize() noexcept;
    virtual void dispatch(CommandList &&list) noexcept;
    void present(MetalSwapchain *swapchain, MetalTexture *image) noexcept;
    virtual void set_name(std::string_view name) noexcept;
    [[nodiscard]] auto device() const noexcept { return _queue->device(); }
    [[nodiscard]] auto queue() const noexcept { return _queue; }
    [[nodiscard]] MetalStageBufferPool *upload_pool() noexcept;
    [[nodiscard]] MetalStageBufferPool *download_pool() noexcept;
    virtual void submit(MTL::CommandBuffer *command_buffer) noexcept;
};

}// namespace vox::compute::metal

