//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "sampled_texturecube.h"
#include "metal_helpers.h"
#include <array>

namespace vox {
SampledTextureCube::SampledTextureCube(MTL::Device &device,
                                       uint32_t width,
                                       uint32_t height,
                                       uint32_t depthOrArrayLayers,
                                       bool mipmap,
                                       MTL::PixelFormat format,
                                       MTL::TextureUsage usage,
                                       MTL::StorageMode storage) : SampledTexture(device) {
    _textureDesc = CLONE_METAL_CUSTOM_DELETER(MTL::TextureDescriptor, MTL::TextureDescriptor::alloc()->init());
    if (depthOrArrayLayers == 1) {
        _textureDesc->setTextureType(MTL::TextureTypeCube);
    } else {
        _textureDesc->setTextureType(MTL::TextureTypeCubeArray);
    }
    _textureDesc->setWidth(width);
    _textureDesc->setHeight(height);
    _textureDesc->setArrayLength(depthOrArrayLayers);
    _textureDesc->setPixelFormat(format);
    _textureDesc->setUsage(usage);
    _textureDesc->setMipmapLevelCount(_getMipmapCount(mipmap));
    _textureDesc->setStorageMode(storage);

    _dimension = MTL::TextureTypeCube;
    _nativeTexture = CLONE_METAL_CUSTOM_DELETER(MTL::Texture, device.newTexture(_textureDesc.get()));
}

std::shared_ptr<MTL::Texture> SampledTextureCube::textureView() {
    return CLONE_METAL_CUSTOM_DELETER(MTL::Texture,
                                      _nativeTexture->newTextureView(_textureDesc->pixelFormat(),
                                                                     _dimension,
                                                                     NS::Range(0, _textureDesc->mipmapLevelCount()),
                                                                     NS::Range(0, _textureDesc->arrayLength() * 6)));
}

SampledTexture2DViewPtr SampledTextureCube::textureView2D(uint32_t mipmapLevel, uint32_t layer) {
    return std::make_shared<SampledTexture2DView>(_device, [mipmapLevel, layer, this]() -> auto {
        return CLONE_METAL_CUSTOM_DELETER(MTL::Texture,
                                          _nativeTexture->newTextureView(_textureDesc->pixelFormat(),
                                                                         MTL::TextureType2D,
                                                                         NS::Range(mipmapLevel, 1),
                                                                         NS::Range(layer, 1)));
    });
}

void SampledTextureCube::setPixelBuffer(MTL::CommandQueue &queue, std::array<Image *, 6> images) {
    std::vector<std::shared_ptr<MTL::Buffer>> stagingBuffers;

    auto commandBuffer = CLONE_METAL_CUSTOM_DELETER(MTL::CommandBuffer, queue.commandBuffer());
    auto blit = CLONE_METAL_CUSTOM_DELETER(MTL::BlitCommandEncoder, commandBuffer->blitCommandEncoder());
    for (uint32_t i = 0; i < 6; i++) {
        const Image *image = images[i];
        const std::vector<uint8_t> &data = image->data();

        auto stagingBuffer = CLONE_METAL_CUSTOM_DELETER(MTL::Buffer, _device.newBuffer(data.data(), data.size(),
                                                                                       MTL::ResourceOptionCPUCacheModeDefault));
        stagingBuffers.push_back(stagingBuffer);

        auto width = image->mipmaps().at(0).extent.width;
        auto height = image->mipmaps().at(0).extent.height;
        auto offset = image->mipmaps().at(0).offset;
        blit->copyFromBuffer(stagingBuffer.get(), offset,
                             bytesPerPixel(_textureDesc->pixelFormat()) * width,
                             bytesPerPixel(_textureDesc->pixelFormat()) * width * height,
                             MTL::Size(width, height, 1),
                             _nativeTexture.get(), i, 0, MTL::Origin(0, 0, 0));
    }
    blit->endEncoding();
    commandBuffer->commit();
    commandBuffer->waitUntilCompleted();
}

}// namespace vox
