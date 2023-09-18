//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "metal_api.h"

namespace vox::compute::metal {

class MetalEvent {

private:
    MTL::SharedEvent *_handle;

public:
    explicit MetalEvent(MTL::Device *device) noexcept;
    ~MetalEvent() noexcept;
    [[nodiscard]] auto handle() const noexcept { return _handle; }
    [[nodiscard]] bool is_completed(uint64_t value) const noexcept;
    void signal(MTL::CommandBuffer *command_buffer, uint64_t value) noexcept;
    void wait(MTL::CommandBuffer *command_buffer, uint64_t value) noexcept;
    void synchronize(uint64_t value) noexcept;
    void set_name(std::string_view name) noexcept;
};

}// namespace vox::compute::metal

