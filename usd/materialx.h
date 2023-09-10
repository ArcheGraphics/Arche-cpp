//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <cgltf.h>

#include <MaterialXCore/Document.h>

#include "image.h"

namespace mx = MaterialX;

namespace vox {
class MaterialXMaterialConverter {
public:
    MaterialXMaterialConverter(mx::DocumentPtr doc,
                               const ImageMetadataMap &imageMetadataMap,
                               bool flattenNodes,
                               bool explicitColorspaceTransforms,
                               bool hdstormCompat);

    void convert(const cgltf_material *material, const std::string &materialName);

private:
    MaterialX::DocumentPtr m_doc;
    const ImageMetadataMap &m_imageMetadataMap;
    std::string m_defaultColorSetName;
    std::string m_defaultOpacitySetName;

    bool m_flattenNodes;
    bool m_explicitColorSpaceTransforms;
    bool m_hdstormCompat;

private:
    void createGltfPbrNodes(const cgltf_material *material,
                            const std::string &materialName);

    void setGltfPbrInputs(const cgltf_material *material,
                          const mx::NodeGraphPtr &nodeGraph,
                          const mx::NodePtr &shaderNode);

private:
    void setDiffuseTextureInput(const mx::NodeGraphPtr &nodeGraph,
                                const mx::InputPtr &shaderInput,
                                const cgltf_texture_view *textureView,
                                const mx::Color3 &factor);

    void setAlphaTextureInput(const mx::NodeGraphPtr &nodeGraph,
                              const mx::InputPtr &shaderInput,
                              const cgltf_texture_view *textureView,
                              float factor);

    bool setNormalTextureInput(const mx::NodeGraphPtr &nodeGraph,
                               const mx::InputPtr &shaderInput,
                               const cgltf_texture_view &textureView);

    void setOcclusionTextureInput(const mx::NodeGraphPtr &nodeGraph,
                                  const mx::InputPtr &shaderInput,
                                  const cgltf_texture_view &textureView);

    void setIridescenceThicknessInput(const mx::NodeGraphPtr &nodeGraph,
                                      const mx::InputPtr &shaderInput,
                                      const cgltf_iridescence *iridescence);

private:
    void setSrgbTextureInput(const mx::NodeGraphPtr &nodeGraph,
                             const mx::InputPtr &input,
                             const cgltf_texture_view &textureView,
                             mx::Color3 factor,
                             mx::Color3 fallback);

    void setFloatTextureInput(const mx::NodeGraphPtr &nodeGraph,
                              const mx::InputPtr &input,
                              const cgltf_texture_view &textureView,
                              int channelIndex,
                              float factor,
                              float fallback);

private:
    // These two functions not only set up the image nodes with the correct value
    // types and sampling properties, but also resolve mismatches between the desired and
    // given component types. Resolution is handled according to this table:
    //
    //             texture type
    //              (#channels)
    //           +---------------+---------------+--------------------+
    //  desired  |               |               | color3             |
    //   type    |               | float         | (/vector3)         |
    //           +---------------+---------------+--------------------+
    //           |               |               | img +              |
    //           | greyscale (1) | img           | convert_color3     |
    //           +---------------+---------------+--------------------+
    //           |               |               | img +              |
    //           | greyscale +   | img +         | extract_float(0) + |
    //           | alpha (2)     | extract_float | convert_color3     |
    //           +---------------+---------------+--------------------+
    //           |               | img +         |                    |
    //           | RGB (3)       | extract_float | img                |
    //           +---------------+---------------+--------------------+
    //           |               | img +         | img +              |
    //           | RGBA (4)      | extract_float | convert_color3     |
    //           +---------------+---------------+--------------------+
    //
    mx::NodePtr addFloatTextureNodes(const mx::NodeGraphPtr &nodeGraph,
                                     const cgltf_texture_view &textureView,
                                     std::string &filePath,
                                     int channelIndex,
                                     float defaultValue);

    mx::NodePtr addFloat3TextureNodes(const mx::NodeGraphPtr &nodeGraph,
                                      const cgltf_texture_view &textureView,
                                      std::string &filePath,
                                      bool color,
                                      mx::ValuePtr defaultValue);

    static mx::NodePtr addTextureTransformNode(const mx::NodeGraphPtr &nodeGraph,
                                               const mx::NodePtr &texcoordNode,
                                               const cgltf_texture_transform &transform);

    mx::NodePtr addTextureNode(const mx::NodeGraphPtr &nodeGraph,
                               const std::string &filePath,
                               const std::string &textureType,
                               bool isSrgb,
                               const cgltf_texture_view &textureView,
                               const mx::ValuePtr &defaultValue);

    mx::NodePtr makeGeompropValueNode(const mx::NodeGraphPtr &nodeGraph,
                                      const std::string &geompropName,
                                      const std::string &geompropValueTypeName,
                                      const mx::ValuePtr &defaultValue = nullptr);

    void connectNodeGraphNodeToShaderInput(const mx::NodeGraphPtr &nodeGraph, const mx::InputPtr &input, const mx::NodePtr &node) const;

private:
    bool getTextureMetadata(const cgltf_texture_view &textureView, ImageMetadata &metadata) const;
    bool getTextureFilePath(const cgltf_texture_view &textureView, std::string &filePath) const;
    [[nodiscard]] bool isTextureSrgbInUsd(const cgltf_texture_view &textureView) const;
    [[nodiscard]] int getTextureChannelCount(const cgltf_texture_view &textureView) const;

    [[nodiscard]] std::string getTextureValueType(const cgltf_texture_view &textureView, bool color) const;
};
}// namespace vox
