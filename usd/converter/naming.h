//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <pxr/usd/usd/stage.h>

#include <unordered_set>

using namespace PXR_NS;

namespace vox {
enum class EntryPathType {
    Root = 0,
    Scenes,
    Nodes,
    Materials,
    PreviewMaterials,
    MaterialXMaterials,
    Meshes,
    Cameras,
    Lights,
    ENUM_SIZE
};
const SdfPath &getEntryPath(EntryPathType type);

const char *getMaterialVariantSetName();
std::string normalizeVariantName(const std::string &name);

std::string makeStSetName(int index);
std::string makeColorSetName(int index);
std::string makeOpacitySetName(int index);

std::string makeUniqueMaterialName(std::string baseName,
                                   const std::unordered_set<std::string> &existingNames);

std::string makeUniqueImageFileName(const char *nameHint,
                                    const std::string &fileName,
                                    const std::string &fileExt,
                                    const std::unordered_set<std::string> &existingNames);

SdfPath makeUniqueStageSubpath(const UsdStageRefPtr &stage,
                               const SdfPath &root,
                               const std::string &baseName,
                               const std::string &delimiter = "_");

SdfPath makeMtlxMaterialPath(const std::string &materialName);

SdfPath makeUsdPreviewSurfaceMaterialPath(const std::string &materialName);
}// namespace vox
