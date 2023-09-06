//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "pbr_base_material.h"

namespace vox {
Color PBRBaseMaterial::baseColor() const {
    return std::any_cast<Color>(shaderData.getData(PBRBaseMaterial::_baseColorProp).value());
}

void PBRBaseMaterial::setBaseColor(const Color &newValue) {
    shaderData.setData(PBRBaseMaterial::_baseColorProp, newValue);
}

SampledTexture2DPtr PBRBaseMaterial::baseTexture() const {
    return std::any_cast<SampledTexture2DPtr>(shaderData.getData(PBRBaseMaterial::_baseTextureProp).value());
}

void PBRBaseMaterial::setBaseTexture(const SampledTexture2DPtr &newValue) {
    shaderData.setSampledTexure(PBRBaseMaterial::_baseTextureProp, newValue);
    if (newValue) {
        shaderData.enableMacro(HAS_BASE_COLORMAP);
    } else {
        shaderData.disableMacro(HAS_BASE_COLORMAP);
    }
}

SampledTexture2DPtr PBRBaseMaterial::normalTexture() const {
    return std::any_cast<SampledTexture2DPtr>(shaderData.getData(PBRBaseMaterial::_normalTextureProp).value());
}

void PBRBaseMaterial::setNormalTexture(const SampledTexture2DPtr &newValue) {
    shaderData.setSampledTexure(PBRBaseMaterial::_normalTextureProp, newValue);
    if (newValue) {
        shaderData.enableMacro(HAS_NORMAL_TEXTURE);
    } else {
        shaderData.disableMacro(HAS_NORMAL_TEXTURE);
    }
}

float PBRBaseMaterial::normalTextureIntensity() const {
    return std::any_cast<float>(shaderData.getData(PBRBaseMaterial::_normalTextureIntensityProp).value());
}

void PBRBaseMaterial::setNormalTextureIntensity(float newValue) {
    shaderData.setData(PBRBaseMaterial::_normalTextureIntensityProp, newValue);
}

Color PBRBaseMaterial::emissiveColor() const {
    return std::any_cast<Color>(shaderData.getData(PBRBaseMaterial::_emissiveColorProp).value());
}

void PBRBaseMaterial::setEmissiveColor(const Color &newValue) {
    shaderData.setData(PBRBaseMaterial::_emissiveColorProp, newValue);
}

SampledTexture2DPtr PBRBaseMaterial::emissiveTexture() const {
    return std::any_cast<SampledTexture2DPtr>(shaderData.getData(PBRBaseMaterial::_emissiveTextureProp).value());
}

void PBRBaseMaterial::setEmissiveTexture(const SampledTexture2DPtr &newValue) {
    shaderData.setSampledTexure(PBRBaseMaterial::_emissiveTextureProp, newValue);
    if (newValue) {
        shaderData.enableMacro(HAS_EMISSIVEMAP);
    } else {
        shaderData.disableMacro(HAS_EMISSIVEMAP);
    }
}

SampledTexture2DPtr PBRBaseMaterial::occlusionTexture() const {
    return std::any_cast<SampledTexture2DPtr>(shaderData.getData(PBRBaseMaterial::_occlusionTextureProp).value());
}

void PBRBaseMaterial::setOcclusionTexture(const SampledTexture2DPtr &newValue) {
    shaderData.setSampledTexure(PBRBaseMaterial::_occlusionTextureProp, newValue);
    if (newValue) {
        shaderData.enableMacro(HAS_OCCLUSIONMAP);
    } else {
        shaderData.disableMacro(HAS_OCCLUSIONMAP);
    }
}

float PBRBaseMaterial::occlusionTextureIntensity() const {
    return std::any_cast<float>(shaderData.getData(PBRBaseMaterial::_occlusionTextureIntensityProp).value());
}

void PBRBaseMaterial::setOcclusionTextureIntensity(float newValue) {
    shaderData.setData(PBRBaseMaterial::_occlusionTextureIntensityProp, newValue);
}

Vector4F PBRBaseMaterial::tilingOffset() const {
    return std::any_cast<Vector4F>(shaderData.getData(PBRBaseMaterial::_tilingOffsetProp).value());
}

void PBRBaseMaterial::setTilingOffset(const Vector4F &newValue) {
    shaderData.setData(PBRBaseMaterial::_tilingOffsetProp, newValue);
}

PBRBaseMaterial::PBRBaseMaterial() : BaseMaterial(Shader::find("pbr")),
                                     _tilingOffsetProp(Shader::createProperty("u_tilingOffset", ShaderDataGroup::Material)),
                                     _normalTextureIntensityProp(Shader::createProperty("u_normalIntensity", ShaderDataGroup::Material)),
                                     _occlusionTextureIntensityProp(Shader::createProperty("u_occlusionStrength", ShaderDataGroup::Material)),
                                     _baseColorProp(Shader::createProperty("u_baseColor", ShaderDataGroup::Material)),
                                     _emissiveColorProp(Shader::createProperty("u_emissiveColor", ShaderDataGroup::Material)),
                                     _baseTextureProp(Shader::createProperty("u_baseColorTexture", ShaderDataGroup::Material)),
                                     _normalTextureProp(Shader::createProperty("u_normalTexture", ShaderDataGroup::Material)),
                                     _emissiveTextureProp(Shader::createProperty("u_emissiveTexture", ShaderDataGroup::Material)),
                                     _occlusionTextureProp(Shader::createProperty("u_occlusionTexture", ShaderDataGroup::Material)) {
    shaderData.enableMacro(NEED_WORLDPOS);
    shaderData.enableMacro(NEED_TILINGOFFSET);

    shaderData.setData(PBRBaseMaterial::_baseColorProp, Color(1, 1, 1, 1));
    shaderData.setData(PBRBaseMaterial::_emissiveColorProp, Color(0, 0, 0, 1));
    shaderData.setData(PBRBaseMaterial::_tilingOffsetProp, Vector4F(1, 1, 0, 0));

    shaderData.setData(PBRBaseMaterial::_normalTextureIntensityProp, 1.f);
    shaderData.setData(PBRBaseMaterial::_occlusionTextureIntensityProp, 1.f);
}

}// namespace vox
