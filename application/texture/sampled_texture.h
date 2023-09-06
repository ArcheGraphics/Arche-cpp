//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <Metal/Metal.hpp>

namespace vox {
uint32_t bytesPerPixel(MTL::PixelFormat format);

class SampledTexture {
public:
    SampledTexture(MTL::Device &device);

    /**
     * The width of the texture.
     */
    uint32_t width();

    /**
     * The height of the texture.
     */
    uint32_t height();

    /**
     * The depth of the texture.
     */
    uint32_t depth();

    /**
     * The arrayLength of the texture.
     */
    uint32_t arrayLength();

    /**
     * Texture mipmapping count.
     */
    uint32_t mipmapCount();

    /**
     * Texture format.
     */
    MTL::PixelFormat format();

    MTL::Texture &texture();

public:
    MTL::TextureType textureViewDimension();

    void setTextureViewDimension(MTL::TextureType dim);

    virtual std::shared_ptr<MTL::Texture> textureView() = 0;

public:
    /**
     * Wrapping mode for texture coordinate S.
     */
    MTL::SamplerAddressMode addressModeU();

    void setAddressModeU(MTL::SamplerAddressMode value);

    /**
     * Wrapping mode for texture coordinate T.
     */
    MTL::SamplerAddressMode addressModeV();

    void setAddressModeV(MTL::SamplerAddressMode value);

    /**
     * Filter mode for texture.
     */
    MTL::SamplerMinMagFilter minFilterMode();

    void setMinFilterMode(MTL::SamplerMinMagFilter value);

    /**
     * Filter mode for texture.
     */
    MTL::SamplerMinMagFilter magFilterMode();

    void setMagFilterMode(MTL::SamplerMinMagFilter value);

    /**
     * Filter mode for texture.
     */
    MTL::SamplerMipFilter mipmapFilter();

    void setMipmapFilter(MTL::SamplerMipFilter value);

    /**
     * Anisotropic level for texture.
     */
    uint16_t anisoLevel();

    void setAnisoLevel(uint16_t value);

    MTL::CompareFunction compareFunction();

    void setCompareFunction(MTL::CompareFunction function);

    MTL::SamplerState &sampler();

protected:
    uint32_t _getMipmapCount(bool mipmap);

    MTL::Device &_device;
    std::shared_ptr<MTL::Texture> _nativeTexture;
    std::shared_ptr<MTL::SamplerState> _nativeSampler;
    std::shared_ptr<MTL::TextureDescriptor> _textureDesc;
    std::shared_ptr<MTL::SamplerDescriptor> _samplerDesc;
    MTL::TextureType _dimension;
    bool _isDirty{false};
};

using SampledTexturePtr = std::shared_ptr<SampledTexture>;

}// namespace vox