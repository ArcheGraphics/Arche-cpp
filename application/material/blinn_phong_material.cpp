//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "blinn_phong_material.h"

namespace vox {
Color BlinnPhongMaterial::baseColor() const {
    return std::any_cast<Color>(shaderData.getData(BlinnPhongMaterial::_diffuseColorProp).value());
}

void BlinnPhongMaterial::setBaseColor(const Color &newValue) {
    shaderData.setData(BlinnPhongMaterial::_diffuseColorProp, newValue);
}

SampledTexture2DPtr BlinnPhongMaterial::baseTexture() const {
    return std::any_cast<SampledTexture2DPtr>(shaderData.getData(BlinnPhongMaterial::_baseTextureProp).value());
}

void BlinnPhongMaterial::setBaseTexture(const SampledTexture2DPtr &newValue) {
    shaderData.setSampledTexure(BlinnPhongMaterial::_baseTextureProp, newValue);
    if (newValue) {
        shaderData.enableMacro(HAS_DIFFUSE_TEXTURE);
    } else {
        shaderData.disableMacro(HAS_DIFFUSE_TEXTURE);
    }
}

Color BlinnPhongMaterial::specularColor() const {
    return std::any_cast<Color>(shaderData.getData(BlinnPhongMaterial::_specularColorProp).value());
}

void BlinnPhongMaterial::setSpecularColor(const Color &newValue) {
    shaderData.setData(BlinnPhongMaterial::_specularColorProp, newValue);
}

SampledTexture2DPtr BlinnPhongMaterial::specularTexture() const {
    return std::any_cast<SampledTexture2DPtr>(shaderData.getData(BlinnPhongMaterial::_specularTextureProp).value());
}

void BlinnPhongMaterial::setSpecularTexture(const SampledTexture2DPtr &newValue) {
    shaderData.setSampledTexure(BlinnPhongMaterial::_specularTextureProp, newValue);
    if (newValue) {
        shaderData.enableMacro(HAS_SPECULAR_TEXTURE);
    } else {
        shaderData.disableMacro(HAS_SPECULAR_TEXTURE);
    }
}

Color BlinnPhongMaterial::emissiveColor() const {
    return std::any_cast<Color>(shaderData.getData(BlinnPhongMaterial::_emissiveColorProp).value());
}

void BlinnPhongMaterial::setEmissiveColor(const Color &newValue) {
    shaderData.setData(BlinnPhongMaterial::_emissiveColorProp, newValue);
}

SampledTexture2DPtr BlinnPhongMaterial::emissiveTexture() const {
    return std::any_cast<SampledTexture2DPtr>(shaderData.getData(BlinnPhongMaterial::_emissiveTextureProp).value());
}

void BlinnPhongMaterial::BlinnPhongMaterial::setEmissiveTexture(const SampledTexture2DPtr &newValue) {
    shaderData.setSampledTexure(BlinnPhongMaterial::_emissiveTextureProp, newValue);
    if (newValue) {
        shaderData.enableMacro(HAS_EMISSIVE_TEXTURE);
    } else {
        shaderData.disableMacro(HAS_EMISSIVE_TEXTURE);
    }
}

SampledTexture2DPtr BlinnPhongMaterial::normalTexture() const {
    return std::any_cast<SampledTexture2DPtr>(shaderData.getData(BlinnPhongMaterial::_normalTextureProp).value());
}

void BlinnPhongMaterial::setNormalTexture(const SampledTexture2DPtr &newValue) {
    shaderData.setSampledTexure(BlinnPhongMaterial::_normalTextureProp, newValue);
    if (newValue) {
        shaderData.enableMacro(HAS_NORMAL_TEXTURE);
    } else {
        shaderData.disableMacro(HAS_NORMAL_TEXTURE);
    }
}

float BlinnPhongMaterial::normalIntensity() const {
    return std::any_cast<float>(shaderData.getData(BlinnPhongMaterial::_normalIntensityProp).value());
}

void BlinnPhongMaterial::setNormalIntensity(float newValue) {
    shaderData.setData(BlinnPhongMaterial::_normalIntensityProp, newValue);
}

float BlinnPhongMaterial::shininess() const {
    return std::any_cast<float>(shaderData.getData(BlinnPhongMaterial::_shininessProp).value());
}

void BlinnPhongMaterial::setShininess(float newValue) {
    shaderData.setData(BlinnPhongMaterial::_shininessProp, newValue);
}

Vector4F BlinnPhongMaterial::tilingOffset() const {
    return std::any_cast<Vector4F>(shaderData.getData(BlinnPhongMaterial::_tilingOffsetProp).value());
}

void BlinnPhongMaterial::setTilingOffset(const Vector4F &newValue) {
    shaderData.setData(BlinnPhongMaterial::_tilingOffsetProp, newValue);
}

BlinnPhongMaterial::BlinnPhongMaterial() : BaseMaterial(Shader::find("blinn-phong")),
                                           _diffuseColorProp(Shader::createProperty("u_diffuseColor", ShaderDataGroup::Material)),
                                           _specularColorProp(Shader::createProperty("u_specularColor", ShaderDataGroup::Material)),
                                           _emissiveColorProp(Shader::createProperty("u_emissiveColor", ShaderDataGroup::Material)),
                                           _tilingOffsetProp(Shader::createProperty("u_tilingOffset", ShaderDataGroup::Material)),
                                           _shininessProp(Shader::createProperty("u_shininess", ShaderDataGroup::Material)),
                                           _normalIntensityProp(Shader::createProperty("u_normalIntensity", ShaderDataGroup::Material)),
                                           _baseTextureProp(Shader::createProperty("u_diffuseTexture", ShaderDataGroup::Material)),
                                           _specularTextureProp(Shader::createProperty("u_specularTexture", ShaderDataGroup::Material)),
                                           _emissiveTextureProp(Shader::createProperty("u_emissiveTexture", ShaderDataGroup::Material)),
                                           _normalTextureProp(Shader::createProperty("u_normalTexture", ShaderDataGroup::Material)) {
    shaderData.enableMacro(NEED_WORLDPOS);
    shaderData.enableMacro(NEED_TILINGOFFSET);

    shaderData.setData(_diffuseColorProp, Color(1, 1, 1, 1));
    shaderData.setData(_specularColorProp, Color(1, 1, 1, 1));
    shaderData.setData(_emissiveColorProp, Color(0, 0, 0, 1));
    shaderData.setData(_tilingOffsetProp, Vector4F(1, 1, 0, 0));
    shaderData.setData(_shininessProp, 16.f);
    shaderData.setData(_normalIntensityProp, 1.f);
}

}// namespace vox
