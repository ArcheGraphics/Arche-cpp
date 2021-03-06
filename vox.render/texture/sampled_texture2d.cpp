//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "sampled_texture2d.h"
#include <vector>

namespace vox {
SampledTexture2D::SampledTexture2D(wgpu::Device& device,
                                   uint32_t width,
                                   uint32_t height,
                                   uint32_t depthOrArrayLayers,
                                   wgpu::TextureFormat format,
                                   wgpu::TextureUsage usage,
                                   bool mipmap):
SampledTexture(device) {
    _textureDesc.size.width = width;
    _textureDesc.size.height = height;
    _textureDesc.size.depthOrArrayLayers = depthOrArrayLayers;
    _textureDesc.format = format;
    _textureDesc.usage = usage;
    _textureDesc.mipLevelCount = _getMipmapCount(mipmap);
    
    _dimension = wgpu::TextureViewDimension::e2D;
    _nativeTexture = device.CreateTexture(&_textureDesc);
}

SampledTexture2D::SampledTexture2D(wgpu::Device& device):
SampledTexture(device) {
}

wgpu::TextureView SampledTexture2D::textureView() {
    wgpu::TextureViewDescriptor desc;
    desc.format = _textureDesc.format;
    desc.dimension = _dimension;
    desc.mipLevelCount = _textureDesc.mipLevelCount;
    desc.arrayLayerCount = _textureDesc.size.depthOrArrayLayers;
    desc.aspect = wgpu::TextureAspect::All;
    return _nativeTexture.CreateView(&desc);
}

void SampledTexture2D::setPixelBuffer(const std::vector<uint8_t>& data,
                                      uint32_t width, uint32_t height, uint32_t mipLevel,
                                      uint32_t offset, uint32_t x, uint32_t y) {
    wgpu::BufferDescriptor descriptor;
    descriptor.size = data.size();
    descriptor.usage = wgpu::BufferUsage::CopySrc | wgpu::BufferUsage::CopyDst;
    wgpu::Buffer stagingBuffer = _device.CreateBuffer(&descriptor);
    _device.GetQueue().WriteBuffer(stagingBuffer, 0, data.data(), data.size());
    
    wgpu::ImageCopyBuffer imageCopyBuffer = _createImageCopyBuffer(stagingBuffer, offset, bytesPerPixel(_textureDesc.format) * width);
    wgpu::ImageCopyTexture imageCopyTexture = _createImageCopyTexture(mipLevel, {x, y, 0});
    wgpu::Extent3D copySize = {width, height, 1};
    
    wgpu::CommandEncoder encoder = _device.CreateCommandEncoder();
    encoder.CopyBufferToTexture(&imageCopyBuffer, &imageCopyTexture, &copySize);
    
    wgpu::CommandBuffer copy = encoder.Finish();
    _device.GetQueue().Submit(1, &copy);
}

void SampledTexture2D::setImageSource(const Image* image) {
    for (const auto& mipmap : image->mipmaps()) {
        wgpu::ImageCopyTexture imageCopyTexture = _createImageCopyTexture(mipmap.level, {0, 0, 0});
        wgpu::TextureDataLayout textureLayout = _createTextureDataLayout(mipmap.offset, bytesPerPixel(image->format()) * mipmap.extent.width, wgpu::kCopyStrideUndefined);
        wgpu::Extent3D copySize = {mipmap.extent.width, mipmap.extent.height, 1};
        _device.GetQueue().WriteTexture(&imageCopyTexture, image->data().data(), image->data().size(), &textureLayout, &copySize);
    }
}


}
