//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "sampled_texture.h"
#include "image/image.h"

namespace vox {
class SampledTexture2D : public SampledTexture {
public:
    /**
     * Create Texture2D.
     * @param device - Define the engine to use to render this texture
     * @param width - Texture width
     * @param height - Texture height
     * @param format - Texture format. default  `TextureFormat.R8G8B8A8`
     * @param mipmap - Whether to use multi-level texture
     */
    SampledTexture2D(MTL::Device &device,
                     uint32_t width,
                     uint32_t height,
                     uint32_t depthOrArrayLayers = 1,
                     bool mipmap = true,
                     MTL::PixelFormat format = MTL::PixelFormatRGBA8Unorm,
                     MTL::TextureUsage usage = MTL::TextureUsageShaderRead,
                     MTL::StorageMode storage = MTL::StorageModePrivate);

    std::shared_ptr<MTL::Texture> textureView() override;

    void setPixelBuffer(MTL::CommandQueue &queue,
                        const std::vector<uint8_t> &data,
                        uint32_t width,
                        uint32_t height,
                        uint32_t mipLevel = 0, uint32_t offset = 0,
                        uint32_t x = 0, uint32_t y = 0);

    void setImageSource(MTL::CommandQueue &queue, const Image *data);

protected:
    SampledTexture2D(MTL::Device &device);
};

using SampledTexture2DPtr = std::shared_ptr<SampledTexture2D>;

}// namespace vox