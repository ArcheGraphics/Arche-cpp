//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef texture_utils_hpp
#define texture_utils_hpp

#include "sampled_texturecube.h"
#include "sampled_texture2d.h"
#include <vector>

namespace vox {
class TextureUtils {
public:
    // n -> 1
    static void buildTextureArray(const std::vector<std::shared_ptr<MTL::Texture>>::iterator &texturesBegin,
                                  const std::vector<std::shared_ptr<MTL::Texture>>::iterator &texturesEnd,
                                  uint32_t width, uint32_t height,
                                  MTL::Texture &textureArray,
                                  MTL::CommandBuffer &commandBuffer);

    // 6n -> 1
    static void buildCubeTextureArray(const std::vector<std::shared_ptr<MTL::Texture>>::iterator &texturesBegin,
                                      const std::vector<std::shared_ptr<MTL::Texture>>::iterator &texturesEnd,
                                      uint32_t width, uint32_t height,
                                      MTL::Texture &textureArray,
                                      MTL::CommandBuffer &commandBuffer);

public:
    static std::shared_ptr<SampledTexture2D>
    createMetallicRoughnessTexture(const MTL::Texture &metallic, const MTL::Texture &roughness,
                                   MTL::Device &device, MTL::Library &library, MTL::CommandQueue &queue);

    static std::shared_ptr<SampledTexture2D>
    createBRDFLookupTable(MTL::Device &device, MTL::Library &library, MTL::CommandQueue &queue);

    static std::shared_ptr<SampledTextureCube>
    createSpecularTexture(const MTL::Texture &cube,
                          MTL::Device &device, MTL::Library &library, MTL::CommandQueue &queue);
};

}// namespace vox

#endif /* texture_utils_hpp */
