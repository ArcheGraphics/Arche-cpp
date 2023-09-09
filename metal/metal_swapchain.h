//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <QuartzCore/QuartzCore.hpp>
#include <Metal/Metal.hpp>
#include <string_view>

extern "C" CA::MetalLayer *metal_backend_create_layer(
    MTL::Device *device, uint64_t window_handle,
    uint32_t width, uint32_t height,
    bool hdr, bool vsync,
    uint32_t back_buffer_count) noexcept;

namespace vox::metal {
class MetalSwapchain {
public:
    MetalSwapchain(MTL::Device &device, uint64_t window_handle,
                   uint width, uint height, bool allow_hdr,
                   bool vsync, uint back_buffer_size) noexcept;

    ~MetalSwapchain() noexcept;

    [[nodiscard]] auto layer() const noexcept { return _layer; }

    void present(MTL::CommandBuffer *commandBuffer, MTL::Texture *image) noexcept;

    void set_name(std::string_view name) noexcept;

private:
    void create_pso(MTL::Device &device);

private:
    CA::MetalLayer *_layer{};
    std::shared_ptr<MTL::RenderPipelineState> _pipeline{};
    std::shared_ptr<MTL::RenderPassDescriptor> _render_pass_desc{};
    NS::String *_command_label{};
};

}// namespace vox::metal
