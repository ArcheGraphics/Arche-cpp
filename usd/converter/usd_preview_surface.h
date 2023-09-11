//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <cgltf.h>

#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usdShade/shader.h>

#include "image.h"

using namespace PXR_NS;

namespace vox {
class UsdPreviewSurfaceMaterialConverter {
public:
    UsdPreviewSurfaceMaterialConverter(UsdStageRefPtr stage,
                                       const ImageMetadataMap &imageMetadataMap);

    void convert(const cgltf_material *material, const SdfPath &path);

private:
    UsdStageRefPtr m_stage;
    const ImageMetadataMap &m_imageMetadataMap;

private:
    void setNormalTextureInput(const SdfPath &basePath,
                               UsdShadeInput &shaderInput,
                               const cgltf_texture_view &textureView);

    void setOcclusionTextureInput(const SdfPath &basePath,
                                  UsdShadeInput &shaderInput,
                                  const cgltf_texture_view &textureView);

    void setSrgbTextureInput(const SdfPath &basePath,
                             UsdShadeInput &shaderInput,
                             const cgltf_texture_view &textureView,
                             const GfVec4f &factor,
                             const GfVec4f *fallback = nullptr);

    void setFloatTextureInput(const SdfPath &basePath,
                              UsdShadeInput &shaderInput,
                              const cgltf_texture_view &textureView,
                              const TfToken &channel,
                              const GfVec4f &factor,
                              const GfVec4f *fallback = nullptr);

private:
    void setTextureInput(const SdfPath &basePath,
                         UsdShadeInput &shaderInput,
                         const cgltf_texture_view &textureView,
                         const TfToken &channels,
                         const TfToken &colorSpace,
                         const GfVec4f *scale,
                         const GfVec4f *bias,
                         const GfVec4f *fallback);

    void addTextureTransformNode(const SdfPath &basePath,
                                 const cgltf_texture_transform &transform,
                                 int stIndex,
                                 UsdShadeInput &textureStInput);

    bool addTextureNode(const SdfPath &basePath,
                        const cgltf_texture_view &textureView,
                        const TfToken &colorSpace,
                        const GfVec4f *scale,
                        const GfVec4f *bias,
                        const GfVec4f *fallback,
                        UsdShadeShader &node);

    void setStPrimvarInput(UsdShadeInput &input, const SdfPath &nodeBasePath, int stIndex);

private:
    bool getTextureMetadata(const cgltf_texture_view &textureView, ImageMetadata &metadata) const;
    bool getTextureFilePath(const cgltf_texture_view &textureView, std::string &filePath) const;
    [[nodiscard]] int getTextureChannelCount(const cgltf_texture_view &textureView) const;
};
}// namespace vox
