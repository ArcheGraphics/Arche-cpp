//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "texture/texture.h"
#include "framework/common/helpers.h"
#include "framework/common/filesystem.h"
#include "framework/common/utils.h"
#include "framework/common/metal_helpers.h"

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb_image_resize.h>

#include "texture/stb.h"

namespace vox {
Texture::Texture(std::vector<uint8_t> &&d, std::vector<Mipmap> &&m)
    : _data{std::move(d)},
      _format{MTL::PixelFormatRGBA8Unorm},
      _mipmaps{std::move(m)} {
}

const std::vector<uint8_t> &Texture::get_data() const {
    return _data;
}

void Texture::clear_data() {
    _data.clear();
    _data.shrink_to_fit();
}

MTL::PixelFormat Texture::get_format() const {
    return _format;
}

const MTL::Size &Texture::get_extent() const {
    return _mipmaps.at(0).extent;
}

const uint32_t Texture::get_layers() const {
    return static_cast<uint32_t>(_mipmaps.at(0).extent.depth);
}

const std::vector<Mipmap> &Texture::get_mipmaps() const {
    return _mipmaps;
}

const std::vector<std::vector<uint64_t>> &Texture::get_offsets() const {
    return _offsets;
}

void Texture::create_image(MTL::Device &device,
                           MTL::TextureType image_type) {
    auto desc = CLONE_METAL_CUSTOM_DELETER(MTL::TextureDescriptor, MTL::TextureDescriptor::alloc()->init());
    desc->setTextureType(image_type);
    desc->setPixelFormat(_format);
    desc->setMipmapLevelCount(_mipmaps.size());
    desc->setArrayLength(get_layers());
    desc->setStorageMode(MTL::StorageModeManaged);
    desc->setSampleCount(1);
    auto extent = get_extent();
    desc->setWidth(extent.width);
    desc->setHeight(extent.height);
    desc->setDepth(extent.depth);
    metal_image = CLONE_METAL_CUSTOM_DELETER(MTL::Texture, device.newTexture(desc.get()));
}

const MTL::Texture &Texture::get_image() const {
    return *metal_image;
}

const MTL::Texture &Texture::get_image_view(MTL::TextureType view_type,
                                            uint32_t base_mip_level,
                                            uint32_t base_array_layer,
                                            uint32_t n_mip_levels,
                                            uint32_t n_array_layers) {
    std::size_t key = 0;
    vox::hash_combine(key, view_type);
    vox::hash_combine(key, base_mip_level);
    vox::hash_combine(key, base_array_layer);
    vox::hash_combine(key, n_mip_levels);
    vox::hash_combine(key, n_array_layers);
    auto iter = metal_image_views.find(key);

    if (iter == metal_image_views.end()) {
        auto view = CLONE_METAL_CUSTOM_DELETER(MTL::Texture,
                                               metal_image->newTextureView(get_format(), view_type,
                                                                           {base_mip_level, n_mip_levels},
                                                                           {base_array_layer, n_array_layers}));
        metal_image_views.insert(std::make_pair(key, view));
    }

    return *metal_image_views.find(key)->second.get();
}

Mipmap &Texture::get_mipmap(const size_t index) {
    return _mipmaps.at(index);
}

void Texture::generate_mipmaps() {
    assert(_mipmaps.size() == 1 && "Mipmaps already generated");

    if (_mipmaps.size() > 1) {
        return;// Do not generate again
    }

    const MTL::Size &extent = get_extent();
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

std::vector<Mipmap> &Texture::get_mut_mipmaps() {
    return _mipmaps;
}

std::vector<uint8_t> &Texture::get_mut_data() {
    return _data;
}

void Texture::set_data(const uint8_t *raw_data, size_t size) {
    assert(_data.empty() && "Texture data already set");
    _data = {raw_data, raw_data + size};
}

void Texture::set_format(const MTL::PixelFormat f) {
    _format = f;
}

void Texture::set_width(const uint32_t width) {
    _mipmaps.at(0).extent.width = width;
}

void Texture::set_height(const uint32_t height) {
    _mipmaps.at(0).extent.height = height;
}

void Texture::set_depth(const uint32_t depth) {
    _mipmaps.at(0).extent.depth = depth;
}

void Texture::set_layers(uint32_t l) {
    _mipmaps.at(0).extent.depth = l;
}

void Texture::set_offsets(const std::vector<std::vector<uint64_t>> &o) {
    _offsets = o;
}

// When the color-space of a loaded image is unknown (from KTX1 for example) we
// may want to assume that the loaded data is in sRGB format (since it usually is).
// In those cases, this helper will get called which will force an existing unorm
// format to become an srgb format where one exists. If none exist, the format will
// remain unmodified.
static MTL::PixelFormat maybe_coerce_to_srgb(MTL::PixelFormat fmt) {
    switch (fmt) {
        case MTL::PixelFormatR8Unorm:
            return MTL::PixelFormatR8Unorm_sRGB;
        case MTL::PixelFormatRG8Unorm:
            return MTL::PixelFormatRG8Unorm_sRGB;
        case MTL::PixelFormatRGBA8Unorm:
            return MTL::PixelFormatRGBA8Unorm_sRGB;
        case MTL::PixelFormatBGRA8Unorm:
            return MTL::PixelFormatBGRA8Unorm_sRGB;
        case MTL::PixelFormatBC1_RGBA:
            return MTL::PixelFormatBC1_RGBA_sRGB;
        case MTL::PixelFormatBC2_RGBA:
            return MTL::PixelFormatBC2_RGBA_sRGB;
        case MTL::PixelFormatBC3_RGBA:
            return MTL::PixelFormatBC3_RGBA_sRGB;
        case MTL::PixelFormatBC7_RGBAUnorm:
            return MTL::PixelFormatBC7_RGBAUnorm_sRGB;
        case MTL::PixelFormatPVRTC_RGB_2BPP:
            return MTL::PixelFormatPVRTC_RGB_2BPP_sRGB;
        case MTL::PixelFormatPVRTC_RGB_4BPP:
            return MTL::PixelFormatPVRTC_RGB_4BPP_sRGB;
        case MTL::PixelFormatPVRTC_RGBA_2BPP:
            return MTL::PixelFormatPVRTC_RGBA_2BPP_sRGB;
        case MTL::PixelFormatPVRTC_RGBA_4BPP:
            return MTL::PixelFormatPVRTC_RGBA_4BPP_sRGB;
        case MTL::PixelFormatEAC_RGBA8:
            return MTL::PixelFormatEAC_RGBA8_sRGB;
        case MTL::PixelFormatETC2_RGB8:
            return MTL::PixelFormatETC2_RGB8_sRGB;
        case MTL::PixelFormatETC2_RGB8A1:
            return MTL::PixelFormatETC2_RGB8A1_sRGB;
        case MTL::PixelFormatASTC_4x4_LDR:
        case MTL::PixelFormatASTC_4x4_HDR:
            return MTL::PixelFormatASTC_4x4_sRGB;
        case MTL::PixelFormatASTC_5x4_LDR:
        case MTL::PixelFormatASTC_5x4_HDR:
            return MTL::PixelFormatASTC_5x4_sRGB;
        case MTL::PixelFormatASTC_5x5_LDR:
        case MTL::PixelFormatASTC_5x5_HDR:
            return MTL::PixelFormatASTC_5x5_sRGB;
        case MTL::PixelFormatASTC_6x5_LDR:
        case MTL::PixelFormatASTC_6x5_HDR:
            return MTL::PixelFormatASTC_6x5_sRGB;
        case MTL::PixelFormatASTC_6x6_LDR:
        case MTL::PixelFormatASTC_6x6_HDR:
            return MTL::PixelFormatASTC_6x6_sRGB;
        case MTL::PixelFormatASTC_8x5_LDR:
        case MTL::PixelFormatASTC_8x5_HDR:
            return MTL::PixelFormatASTC_8x5_sRGB;
        case MTL::PixelFormatASTC_8x6_LDR:
        case MTL::PixelFormatASTC_8x6_HDR:
            return MTL::PixelFormatASTC_8x6_sRGB;
        case MTL::PixelFormatASTC_8x8_LDR:
        case MTL::PixelFormatASTC_8x8_HDR:
            return MTL::PixelFormatASTC_8x8_sRGB;
        case MTL::PixelFormatASTC_10x5_LDR:
        case MTL::PixelFormatASTC_10x5_HDR:
            return MTL::PixelFormatASTC_10x5_sRGB;
        case MTL::PixelFormatASTC_10x6_LDR:
        case MTL::PixelFormatASTC_10x6_HDR:
            return MTL::PixelFormatASTC_10x6_sRGB;
        case MTL::PixelFormatASTC_10x8_LDR:
        case MTL::PixelFormatASTC_10x8_HDR:
            return MTL::PixelFormatASTC_10x8_sRGB;
        case MTL::PixelFormatASTC_10x10_LDR:
        case MTL::PixelFormatASTC_10x10_HDR:
            return MTL::PixelFormatASTC_10x10_sRGB;
        case MTL::PixelFormatASTC_12x10_LDR:
        case MTL::PixelFormatASTC_12x10_HDR:
            return MTL::PixelFormatASTC_12x10_sRGB;
        case MTL::PixelFormatASTC_12x12_LDR:
        case MTL::PixelFormatASTC_12x12_HDR:
            return MTL::PixelFormatASTC_12x12_sRGB;
        case MTL::PixelFormatBGRA10_XR:
            return MTL::PixelFormatBGRA10_XR_sRGB;
        case MTL::PixelFormatBGR10_XR:
            return MTL::PixelFormatBGR10_XR_sRGB;
        default:
            return fmt;
    }
}

void Texture::coerce_format_to_srgb() {
    _format = maybe_coerce_to_srgb(_format);
}

std::shared_ptr<Texture> Texture::load(const std::string &uri, bool flipY) {
    std::shared_ptr<Texture> image{nullptr};

    auto data = fs::read_asset(uri);

    // Get extension
    auto extension = get_extension(uri);
    if (extension == "png" || extension == "jpg") {
        image = std::make_shared<Stb>(data, flipY);
    }
    return image;
}

}// namespace vox
