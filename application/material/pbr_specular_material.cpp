//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.
#include "pbr_specular_material.h"

namespace vox {
Color PBRSpecularMaterial::specularColor() const {
    return std::any_cast<Color>(shaderData.getData(PBRSpecularMaterial::_specularColorProp).value());
}

void PBRSpecularMaterial::setSpecularColor(const Color &newValue) {
    shaderData.setData(PBRSpecularMaterial::_specularColorProp, newValue);
}

float PBRSpecularMaterial::glossiness() const {
    return std::any_cast<float>(shaderData.getData(PBRSpecularMaterial::_glossinessProp).value());
}

void PBRSpecularMaterial::setGlossiness(float newValue) {
    shaderData.setData(PBRSpecularMaterial::_glossinessProp, newValue);
}

SampledTexture2DPtr PBRSpecularMaterial::specularGlossinessTexture() const {
    return std::any_cast<SampledTexture2DPtr>(shaderData.getData(PBRSpecularMaterial::_specularGlossinessTextureProp).value());
}

void PBRSpecularMaterial::setSpecularGlossinessTexture(const SampledTexture2DPtr &newValue) {
    shaderData.setSampledTexure(PBRSpecularMaterial::_specularGlossinessTextureProp, newValue);
    if (newValue) {
        shaderData.enableMacro(HAS_SPECULARGLOSSINESSMAP);
    } else {
        shaderData.disableMacro(HAS_SPECULARGLOSSINESSMAP);
    }
}

PBRSpecularMaterial::PBRSpecularMaterial() : PBRBaseMaterial(),
                                             _glossinessProp(Shader::createProperty("u_glossiness", ShaderDataGroup::Material)),
                                             _specularColorProp(Shader::createProperty("u_specularColor", ShaderDataGroup::Material)),
                                             _specularGlossinessTextureProp(Shader::createProperty("_specularGlossinessTexture", ShaderDataGroup::Material)) {
    shaderData.setData(PBRSpecularMaterial::_specularColorProp, Color(1, 1, 1, 1));
    shaderData.setData(PBRSpecularMaterial::_glossinessProp, 1.f);
}

}// namespace vox
