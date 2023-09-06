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

class Image {
public:
    Image(std::vector<uint8_t> &&data = {}, std::vector<Mipmap> &&mipmaps = {{}});

    static std::unique_ptr<Image> load(const std::string &uri, bool flipY = false);

    virtual ~Image() = default;

    const std::vector<uint8_t> &data() const;

    void clear();

    MTL::PixelFormat format() const;

    const MTL::Size &extent() const;

    const uint32_t layers() const;

    const std::vector<Mipmap> &mipmaps() const;

    const std::vector<std::vector<uint64_t>> &offsets() const;

    void generateMipmaps();

public:
    std::shared_ptr<SampledTexture2D>
    createSampledTexture(MTL::Device &device, MTL::CommandQueue &queue,
                         MTL::TextureUsage usage = MTL::TextureUsageShaderRead);

protected:
    std::vector<uint8_t> &data();

    void setData(const uint8_t *raw_data, size_t size);

    void setFormat(MTL::PixelFormat format);

    void setWidth(uint32_t width);

    void setHeight(uint32_t height);

    void setDepth(uint32_t depth);

    void setLayers(uint32_t layers);

    void setOffsets(const std::vector<std::vector<uint64_t>> &offsets);

    Mipmap &mipmap(size_t index);

    std::vector<Mipmap> &mipmaps();

private:
    std::vector<uint8_t> _data;

    MTL::PixelFormat _format{MTL::PixelFormatInvalid};

    std::vector<Mipmap> _mipmaps{{}};

    // Offsets stored like offsets[array_layer][mipmap_layer]
    std::vector<std::vector<uint64_t>> _offsets;
};

}// namespace vox