//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <array>
#include <string_view>
#include "runtime/rhi/pixel.h"
#include "metal_api.h"

namespace vox::compute::metal {

class MetalTexture {

public:
    static constexpr auto max_level_count = 15u;

    struct Binding {
        MTL::ResourceID handle;
    };

private:
    std::array<MTL::Texture *, max_level_count> _maps{};
    PixelFormat _format{};

public:
    MetalTexture(MTL::Device *device, PixelFormat format, uint dimension,
                 uint width, uint height, uint depth, uint mipmap_levels,
                 bool allow_simultaneous_access) noexcept;
    ~MetalTexture() noexcept;
    [[nodiscard]] MTL::Texture *handle(uint level = 0u) const noexcept;
    [[nodiscard]] Binding binding(uint level = 0u) const noexcept;
    [[nodiscard]] auto format() const noexcept { return _format; }
    [[nodiscard]] auto storage() const noexcept { return pixel_format_to_storage(_format); }
    void set_name(std::string_view name) noexcept;
};

}// namespace vox::compute::metal

