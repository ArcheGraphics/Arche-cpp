//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <cgltf.h>
#include <pxr/usd/usd/prim.h>
#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usdShade/shader.h>
#include <MaterialXCore/Document.h>

#include <unordered_map>
#include <filesystem>
#include <string_view>

#include "materialx.h"
#include "usd_preview_surface.h"

namespace fs = std::filesystem;
using namespace PXR_NS;

namespace vox {
class Converter {
public:
    enum class GltfPbrImpl {
        Runtime,
        File,
        Flattened
    };

    struct Params {
        fs::path srcDir;
        fs::path dstDir;
        fs::path mtlxFileName;
        bool copyExistingFiles;
        bool genRelativePaths;
        bool emitMtlx;
        bool mtlxAsUsdShade;
        bool explicitColorspaceTransforms;
        GltfPbrImpl gltfPbrImpl;
        bool hdStormCompat;
        int defaultMaterialVariant;
    };

public:
    Converter(const cgltf_data *data, UsdStageRefPtr stage, const Params &params);

public:
    struct FileExport {
        std::string filePath;
        std::string refPath;
    };
    using FileExports = std::vector<FileExport>;

    void convert(FileExports &fileExports);

private:
    void createMaterials(FileExports &fileExports, bool createDefaultMaterial);
    void createNodesRecursively(const cgltf_node *nodeData, SdfPath path);
    void createOrOverCamera(const cgltf_camera *cameraData, SdfPath path);
    void createOrOverLight(const cgltf_light *lightData, SdfPath path);
    void createOrOverMesh(const cgltf_mesh *meshData, SdfPath path);
    void createMaterialBinding(UsdPrim &prim, const std::string &materialName);
    bool createPrimitive(const cgltf_primitive *primitiveData, SdfPath path, UsdPrim &prim);

private:
    bool overridePrimInPathMap(void *dataPtr, const SdfPath &path, UsdPrim &prim);
    bool isValidTexture(const cgltf_texture_view &textureView);

private:
    const cgltf_data *m_data;
    UsdStageRefPtr m_stage;
    const Params &m_params;

private:
    ImageMetadataMap m_imgMetadata;
    MaterialX::DocumentPtr m_mtlxDoc;
    MaterialXMaterialConverter m_mtlxConverter;
    UsdPreviewSurfaceMaterialConverter m_usdPreviewSurfaceConverter;
    std::unordered_map<void *, SdfPath> m_uniquePaths;
    std::vector<std::string> m_materialNames;
};
}// namespace vox
