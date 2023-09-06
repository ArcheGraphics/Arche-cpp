//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "sampled_texture.h"
#include "metal_helpers.h"

namespace vox {
SampledTexture::SampledTexture(MTL::Device &device) : _device(device) {
    _samplerDesc = CLONE_METAL_CUSTOM_DELETER(MTL::SamplerDescriptor, MTL::SamplerDescriptor::alloc()->init());
    setMinFilterMode(MTL::SamplerMinMagFilterLinear);
    setMagFilterMode(MTL::SamplerMinMagFilterLinear);
    setMipmapFilter(MTL::SamplerMipFilterLinear);
    setAddressModeU(MTL::SamplerAddressModeRepeat);
    setAddressModeV(MTL::SamplerAddressModeRepeat);
}

uint32_t SampledTexture::width() {
    return static_cast<uint32_t>(_textureDesc->width());
}

uint32_t SampledTexture::height() {
    return static_cast<uint32_t>(_textureDesc->height());
}

uint32_t SampledTexture::depth() {
    return static_cast<uint32_t>(_textureDesc->depth());
}

uint32_t SampledTexture::arrayLength() {
    return static_cast<uint32_t>(_textureDesc->arrayLength());
}

uint32_t SampledTexture::mipmapCount() {
    return static_cast<uint32_t>(_textureDesc->mipmapLevelCount());
}

MTL::PixelFormat SampledTexture::format() {
    return _textureDesc->pixelFormat();
}

MTL::TextureType SampledTexture::textureViewDimension() {
    return _dimension;
}

void SampledTexture::setTextureViewDimension(MTL::TextureType dim) {
    _dimension = dim;
}

uint32_t SampledTexture::_getMipmapCount(bool mipmap) {
    return mipmap ? log2(fmax(_textureDesc->width(), _textureDesc->height())) + 1 : 1;
}

MTL::Texture &SampledTexture::texture() {
    return *_nativeTexture;
}

MTL::SamplerState &SampledTexture::sampler() {
    if (_isDirty) {
        _nativeSampler = CLONE_METAL_CUSTOM_DELETER(MTL::SamplerState, _device.newSamplerState(_samplerDesc.get()));
        _isDirty = false;
    }
    return *_nativeSampler;
}

//MARK: - Sampler
MTL::SamplerAddressMode SampledTexture::addressModeU() {
    return _samplerDesc->sAddressMode();
}

void SampledTexture::setAddressModeU(MTL::SamplerAddressMode value) {
    _samplerDesc->setSAddressMode(value);
    _isDirty = true;
}

MTL::SamplerAddressMode SampledTexture::addressModeV() {
    return _samplerDesc->tAddressMode();
}

void SampledTexture::setAddressModeV(MTL::SamplerAddressMode value) {
    _samplerDesc->setTAddressMode(value);
    _isDirty = true;
}

MTL::SamplerMinMagFilter SampledTexture::minFilterMode() {
    return _samplerDesc->minFilter();
}

void SampledTexture::setMinFilterMode(MTL::SamplerMinMagFilter value) {
    _samplerDesc->setMinFilter(value);
    _isDirty = true;
}

MTL::SamplerMinMagFilter SampledTexture::magFilterMode() {
    return _samplerDesc->magFilter();
}

void SampledTexture::setMagFilterMode(MTL::SamplerMinMagFilter value) {
    _samplerDesc->setMagFilter(value);
    _isDirty = true;
}

MTL::SamplerMipFilter SampledTexture::mipmapFilter() {
    return _samplerDesc->mipFilter();
}

void SampledTexture::setMipmapFilter(MTL::SamplerMipFilter value) {
    _samplerDesc->setMipFilter(value);
    _isDirty = true;
}

uint16_t SampledTexture::anisoLevel() {
    return _samplerDesc->maxAnisotropy();
}

void SampledTexture::setAnisoLevel(uint16_t value) {
    _samplerDesc->setMaxAnisotropy(value);
    _isDirty = true;
}

MTL::CompareFunction SampledTexture::compareFunction() {
    return _samplerDesc->compareFunction();
}

void SampledTexture::setCompareFunction(MTL::CompareFunction function) {
    _samplerDesc->setCompareFunction(function);
    _isDirty = true;
}

uint32_t bytesPerPixel(MTL::PixelFormat format) {
    switch (format) {
        case MTL::PixelFormatR8Unorm:
        case MTL::PixelFormatR8Snorm:
        case MTL::PixelFormatR8Uint:
        case MTL::PixelFormatR8Sint:
            return 1;
            break;

        case MTL::PixelFormatR16Uint:
        case MTL::PixelFormatR16Sint:
        case MTL::PixelFormatR16Float:
        case MTL::PixelFormatRG8Unorm:
        case MTL::PixelFormatRG8Snorm:
            return 2;
            break;

        case MTL::PixelFormatR32Float:
        case MTL::PixelFormatR32Uint:
        case MTL::PixelFormatR32Sint:
        case MTL::PixelFormatRG16Uint:
        case MTL::PixelFormatRG16Sint:
        case MTL::PixelFormatRG16Float:
        case MTL::PixelFormatRGBA8Unorm:
        case MTL::PixelFormatRGBA8Unorm_sRGB:
        case MTL::PixelFormatRGBA8Snorm:
        case MTL::PixelFormatRGBA8Uint:
        case MTL::PixelFormatRGBA8Sint:
        case MTL::PixelFormatBGRA8Unorm:
        case MTL::PixelFormatBGRA8Unorm_sRGB:
            return 4;
            break;

        case MTL::PixelFormatRG32Float:
        case MTL::PixelFormatRG32Uint:
        case MTL::PixelFormatRG32Sint:
        case MTL::PixelFormatRGBA16Uint:
        case MTL::PixelFormatRGBA16Sint:
            return 8;
            break;

        case MTL::PixelFormatRGBA32Float:
        case MTL::PixelFormatRGBA32Sint:
            return 16;
            break;

        default:
            assert(false && "undefined");
            throw std::exception();
            break;
    }
}

}// namespace vox
