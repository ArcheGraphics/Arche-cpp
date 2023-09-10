//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <cgltf.h>

#include <filesystem>
#include <string>
#include <unordered_map>

namespace fs = std::filesystem;

namespace vox {
struct ImageMetadata {
    std::string filePath;
    std::string refPath;
    // Needed to determine the type of MaterialX <image> nodes
    int channelCount;
    // USD makes an incorrect assumption that we have to work around by undoing an incorrect sRGB-to-linear transform in our MaterialX network gen:
    // https://github.com/PixarAnimationStudios/USD/blob/857ffda41f4f1553fe1019ac7c7b4f08c233a7bb/pxr/imaging/plugin/hioOiio/oiioImage.cpp#L470-L471
    // The stb_image sRGB detection logic is slightly different, but since guc requires OIIO, we don't have to care about it.
    // Our UsdPreviewSurface generator is fine too, since there we set explicit sourceColorSpace inputs.
    bool isSrgbInUSD;
};

using ImageMetadataMap = std::unordered_map<const cgltf_image *, ImageMetadata>;

void processImages(const cgltf_image *images,
                   size_t imageCount,
                   const fs::path &srcDir,
                   const fs::path &dstDir,
                   bool copyExistingFiles,
                   bool genRelativePaths,
                   ImageMetadataMap &metadata);
}// namespace vox
