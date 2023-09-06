//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "sampled_texture3d.h"
#include "metal_helpers.h"

namespace vox {
SampledTexture3D::SampledTexture3D(MTL::Device &device,
                                   uint32_t width,
                                   uint32_t height,
                                   uint32_t depth,
                                   bool mipmap,
                                   MTL::PixelFormat format,
                                   MTL::TextureUsage usage,
                                   MTL::StorageMode storage) : SampledTexture(device) {
    _textureDesc = CLONE_METAL_CUSTOM_DELETER(MTL::TextureDescriptor, MTL::TextureDescriptor::alloc()->init());
    _textureDesc->setTextureType(MTL::TextureType3D);
    _textureDesc->setWidth(width);
    _textureDesc->setHeight(height);
    _textureDesc->setDepth(depth);
    _textureDesc->setArrayLength(1);
    _textureDesc->setPixelFormat(format);
    _textureDesc->setUsage(usage);
    _textureDesc->setMipmapLevelCount(_getMipmapCount(mipmap));
    _textureDesc->setStorageMode(storage);

    _dimension = MTL::TextureType3D;
    _nativeTexture = CLONE_METAL_CUSTOM_DELETER(MTL::Texture, device.newTexture(_textureDesc.get()));
}

}// namespace vox
