//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "sampled_texture2d.h"
#include "framework/common/metal_helpers.h"

namespace vox {
SampledTexture2D::SampledTexture2D(MTL::Device &device,
                                   uint32_t width,
                                   uint32_t height,
                                   uint32_t depthOrArrayLayers,
                                   bool mipmap,
                                   MTL::PixelFormat format,
                                   MTL::TextureUsage usage,
                                   MTL::StorageMode storage) : SampledTexture(device) {
    _textureDesc = CLONE_METAL_CUSTOM_DELETER(MTL::TextureDescriptor, MTL::TextureDescriptor::alloc()->init());
    if (depthOrArrayLayers == 1) {
        _textureDesc->setTextureType(MTL::TextureType2D);
    } else {
        _textureDesc->setTextureType(MTL::TextureType2DArray);
    }
    _textureDesc->setWidth(width);
    _textureDesc->setHeight(height);
    _textureDesc->setArrayLength(depthOrArrayLayers);
    _textureDesc->setPixelFormat(format);
    _textureDesc->setUsage(usage);
    _textureDesc->setMipmapLevelCount(_getMipmapCount(mipmap));
    _textureDesc->setStorageMode(storage);

    _dimension = MTL::TextureType2D;
    _nativeTexture = CLONE_METAL_CUSTOM_DELETER(MTL::Texture, device.newTexture(_textureDesc.get()));
}

SampledTexture2D::SampledTexture2D(MTL::Device &device) : SampledTexture(device) {
}

std::shared_ptr<MTL::Texture> SampledTexture2D::textureView() {
    return CLONE_METAL_CUSTOM_DELETER(MTL::Texture,
                                      _nativeTexture->newTextureView(_textureDesc->pixelFormat(),
                                                                     _dimension,
                                                                     NS::Range(0, _textureDesc->mipmapLevelCount()),
                                                                     NS::Range(0, _textureDesc->arrayLength())));
}

void SampledTexture2D::setPixelBuffer(MTL::CommandQueue &queue,
                                      const std::vector<uint8_t> &data,
                                      uint32_t width, uint32_t height, uint32_t mipLevel,
                                      uint32_t offset, uint32_t x, uint32_t y) {
    auto stagingBuffer = CLONE_METAL_CUSTOM_DELETER(MTL::Buffer, _device.newBuffer(data.data(), data.size(),
                                                                                   MTL::ResourceOptionCPUCacheModeDefault));

    auto commandBuffer = CLONE_METAL_CUSTOM_DELETER(MTL::CommandBuffer, queue.commandBuffer());
    auto blit = CLONE_METAL_CUSTOM_DELETER(MTL::BlitCommandEncoder, commandBuffer->blitCommandEncoder());
    blit->copyFromBuffer(stagingBuffer.get(), offset,
                         bytesPerPixel(_textureDesc->pixelFormat()) * width,
                         bytesPerPixel(_textureDesc->pixelFormat()) * width * height, MTL::Size(width, height, 1),
                         _nativeTexture.get(), 0, mipLevel, MTL::Origin(x, y, 0));
    blit->endEncoding();
    commandBuffer->commit();
    commandBuffer->waitUntilCompleted();
}

void SampledTexture2D::setImageSource(MTL::CommandQueue &queue, const Image *image) {
    std::vector<std::shared_ptr<MTL::Buffer>> stagingBuffers;

    auto commandBuffer = CLONE_METAL_CUSTOM_DELETER(MTL::CommandBuffer, queue.commandBuffer());
    auto blit = CLONE_METAL_CUSTOM_DELETER(MTL::BlitCommandEncoder, commandBuffer->blitCommandEncoder());
    for (const auto &mipmap : image->mipmaps()) {
        auto stagingBuffer = CLONE_METAL_CUSTOM_DELETER(MTL::Buffer, _device.newBuffer(image->data().data(), image->data().size(),
                                                                                       MTL::ResourceOptionCPUCacheModeDefault));
        stagingBuffers.push_back(stagingBuffer);
        blit->copyFromBuffer(stagingBuffer.get(), mipmap.offset,
                             bytesPerPixel(_textureDesc->pixelFormat()) * mipmap.extent.width,
                             bytesPerPixel(_textureDesc->pixelFormat()) * mipmap.extent.width * mipmap.extent.height,
                             MTL::Size(mipmap.extent.width, mipmap.extent.height, 1),
                             _nativeTexture.get(), 0, mipmap.level, MTL::Origin(0, 0, 0));
    }
    blit->endEncoding();
    commandBuffer->commit();
    commandBuffer->waitUntilCompleted();
}

}// namespace vox
