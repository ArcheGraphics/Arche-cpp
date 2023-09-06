//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "sampled_texture.h"

namespace vox {
class SampledTexture3D : public SampledTexture {
public:
    /**
     * Create Texture3D.
     * @param device - Define the engine to use to render this texture
     * @param width - Texture width
     * @param height - Texture height
     * @param format - Texture format. default  `TextureFormat.R8G8B8A8`
     * @param mipmap - Whether to use multi-level texture
     */
    SampledTexture3D(MTL::Device &device,
                     uint32_t width,
                     uint32_t height,
                     uint32_t depth,
                     bool mipmap = true,
                     MTL::PixelFormat format = MTL::PixelFormatRGBA8Unorm,
                     MTL::TextureUsage usage = MTL::TextureUsageShaderRead,
                     MTL::StorageMode storage = MTL::StorageModePrivate);
};

}// namespace vox