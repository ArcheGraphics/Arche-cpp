//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "stb.h"

#define STB_IMAGE_IMPLEMENTATION

#include <stb_image.h>
#include "framework/common/helpers.h"

namespace vox {
Stb::Stb(const std::vector<uint8_t> &data, bool flipY) : Image{} {
    int width;
    int height;
    int comp;
    int req_comp = 4;

    auto data_buffer = reinterpret_cast<const stbi_uc *>(data.data());
    auto data_size = static_cast<int>(data.size());

    if (flipY) {
        stbi_set_flip_vertically_on_load(flipY);
    }
    auto raw_data = stbi_load_from_memory(data_buffer, data_size, &width, &height, &comp, req_comp);

    if (!raw_data) {
        throw std::runtime_error{std::string("Failed to load: ") + stbi_failure_reason()};
    }

    setData(raw_data, width * height * req_comp);
    stbi_image_free(raw_data);

    setFormat(MTL::PixelFormatRGBA8Unorm);
    setWidth(to_u32(width));
    setHeight(to_u32(height));
    setDepth(1u);
}

}// namespace vox
