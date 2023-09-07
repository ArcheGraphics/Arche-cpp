//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <Metal/Metal.hpp>
#include <string>
#include <vector>

namespace vox {
class SampledTexture2D;

/**
 * @brief Mipmap information
 */
struct Mipmap {
    /// Mipmap level
    uint32_t level = 0;

    /// Byte offset used for uploading
    uint32_t offset = 0;

    /// Width depth and height of the mipmap
    MTL::Size extent = {0, 0, 0};
};

class Texture {
public:
    Texture(std::vector<uint8_t> &&data = {}, std::vector<Mipmap> &&mipmaps = {{}});

    static std::shared_ptr<Texture> load(const std::string &uri, bool flipY = false);

    virtual ~Texture() = default;

    const std::vector<uint8_t> &get_data() const;

    void clear_data();

    MTL::PixelFormat get_format() const;

    const MTL::Size &get_extent() const;

    const uint32_t get_layers() const;

    const std::vector<Mipmap> &get_mipmaps() const;

    const std::vector<std::vector<uint64_t>> &get_offsets() const;

    void generate_mipmaps();

    void create_image(MTL::Device &device,
                      MTL::TextureType image_usage = MTL::TextureType2D);

    [[nodiscard]] const MTL::Texture &get_image() const;

    [[nodiscard]] const MTL::Texture &get_image_view(MTL::TextureType view_type = MTL::TextureType2D,
                                                     uint32_t base_mip_level = 0,
                                                     uint32_t base_array_layer = 0,
                                                     uint32_t n_mip_levels = 0,
                                                     uint32_t n_array_layers = 0);

    void coerce_format_to_srgb();

protected:
    friend class TextureManager;

    std::vector<uint8_t> &get_mut_data();

    void set_data(const uint8_t *raw_data, size_t size);

    void set_format(MTL::PixelFormat format);

    void set_width(uint32_t width);

    void set_height(uint32_t height);

    void set_depth(uint32_t depth);

    void set_layers(uint32_t layers);

    void set_offsets(const std::vector<std::vector<uint64_t>> &offsets);

    Mipmap &get_mipmap(size_t index);

    std::vector<Mipmap> &get_mut_mipmaps();

private:
    std::vector<uint8_t> _data;

    MTL::PixelFormat _format{MTL::PixelFormatInvalid};

    std::vector<Mipmap> _mipmaps{{}};

    // Offsets stored like offsets[array_layer][mipmap_layer]
    std::vector<std::vector<uint64_t>> _offsets;

    std::shared_ptr<MTL::Texture> metal_image;

    std::unordered_map<size_t, std::shared_ptr<MTL::Texture>> metal_image_views;
};

}// namespace vox