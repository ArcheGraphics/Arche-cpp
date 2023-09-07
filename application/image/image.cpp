//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "image.h"
#include "framework/common/helpers.h"
#include "filesystem.h"

#define STB_IMAGE_RESIZE_IMPLEMENTATION

#include <stb_image_resize.h>

#include "image/stb.h"

#include "texture/sampled_texture2d.h"

namespace vox {
Image::Image(std::vector<uint8_t> &&d, std::vector<Mipmap> &&m) : _data{std::move(d)},
                                                                  _format{MTL::PixelFormatRGBA8Unorm},
                                                                  _mipmaps{std::move(m)} {
}

const std::vector<uint8_t> &Image::data() const {
    return _data;
}

void Image::clear() {
    _data.clear();
    _data.shrink_to_fit();
}

MTL::PixelFormat Image::format() const {
    return _format;
}

const MTL::Size &Image::extent() const {
    return _mipmaps.at(0).extent;
}

const uint32_t Image::layers() const {
    return static_cast<uint32_t>(_mipmaps.at(0).extent.depth);
}

const std::vector<Mipmap> &Image::mipmaps() const {
    return _mipmaps;
}

const std::vector<std::vector<uint64_t>> &Image::offsets() const {
    return _offsets;
}

std::shared_ptr<SampledTexture2D> Image::createSampledTexture(MTL::Device &device, MTL::CommandQueue &queue, MTL::TextureUsage usage) {
    auto sampledTex = std::make_shared<SampledTexture2D>(device,
                                                         _mipmaps.at(0).extent.width,
                                                         _mipmaps.at(0).extent.height,
                                                         _mipmaps.at(0).extent.depth,
                                                         _mipmaps.size() > 1 ? true : false,
                                                         _format, usage);
    sampledTex->setImageSource(queue, this);
    return sampledTex;
}

Mipmap &Image::mipmap(const size_t index) {
    return _mipmaps.at(index);
}

void Image::generateMipmaps() {
    assert(_mipmaps.size() == 1 && "Mipmaps already generated");

    if (_mipmaps.size() > 1) {
        return;// Do not generate again
    }

    const MTL::Size &extent = this->extent();
    auto next_width = std::max<uint32_t>(1u, static_cast<uint32_t>(extent.width) / 2);
    auto next_height = std::max<uint32_t>(1u, static_cast<uint32_t>(extent.height) / 2);
    auto channels = 4;
    auto next_size = next_width * next_height * channels;

    while (true) {
        // Make space for next mipmap
        auto old_size = to_u32(_data.size());
        _data.resize(old_size + next_size);

        auto &prev_mipmap = _mipmaps.back();
        // Update mipmaps
        Mipmap next_mipmap{};
        next_mipmap.level = prev_mipmap.level + 1;
        next_mipmap.offset = old_size;
        next_mipmap.extent = {next_width, next_height, 1u};

        // Fill next mipmap memory
        stbir_resize_uint8(_data.data() + prev_mipmap.offset,
                           static_cast<uint32_t>(prev_mipmap.extent.width),
                           static_cast<uint32_t>(prev_mipmap.extent.height), 0,
                           _data.data() + next_mipmap.offset,
                           static_cast<uint32_t>(next_mipmap.extent.width),
                           static_cast<uint32_t>(next_mipmap.extent.height), 0, channels);

        _mipmaps.emplace_back(std::move(next_mipmap));

        // Next mipmap values
        next_width = std::max<uint32_t>(1u, next_width / 2);
        next_height = std::max<uint32_t>(1u, next_height / 2);
        next_size = next_width * next_height * channels;

        if (next_width == 1 && next_height == 1) {
            break;
        }
    }
}

std::vector<Mipmap> &Image::mipmaps() {
    return _mipmaps;
}

std::vector<uint8_t> &Image::data() {
    return _data;
}

void Image::setData(const uint8_t *raw_data, size_t size) {
    assert(_data.empty() && "Image data already set");
    _data = {raw_data, raw_data + size};
}

void Image::setFormat(const MTL::PixelFormat f) {
    _format = f;
}

void Image::setWidth(const uint32_t width) {
    _mipmaps.at(0).extent.width = width;
}

void Image::setHeight(const uint32_t height) {
    _mipmaps.at(0).extent.height = height;
}

void Image::setDepth(const uint32_t depth) {
    _mipmaps.at(0).extent.depth = depth;
}

void Image::setLayers(uint32_t l) {
    _mipmaps.at(0).extent.depth = l;
}

void Image::setOffsets(const std::vector<std::vector<uint64_t>> &o) {
    _offsets = o;
}

std::unique_ptr<Image> Image::load(const std::string &uri, bool flipY) {
    std::unique_ptr<Image> image{nullptr};

    auto data = fs::readAsset(uri);

    // Get extension
    auto extension = fs::extraExtension(uri);
    if (extension == "png" || extension == "jpg") {
        image = std::make_unique<Stb>(data, flipY);
    }
    return image;
}

}// namespace vox
