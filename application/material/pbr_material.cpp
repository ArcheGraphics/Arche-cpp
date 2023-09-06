//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "pbr_material.h"

namespace vox {
float PBRMaterial::metallic() const {
    return std::any_cast<float>(shaderData.getData(PBRMaterial::_metallicProp).value());
}

void PBRMaterial::setMetallic(float newValue) {
    shaderData.setData(PBRMaterial::_metallicProp, newValue);
}

float PBRMaterial::roughness() const {
    return std::any_cast<float>(shaderData.getData(PBRMaterial::_roughnessProp).value());
}

void PBRMaterial::setRoughness(float newValue) {
    shaderData.setData(PBRMaterial::_roughnessProp, newValue);
}

SampledTexture2DPtr PBRMaterial::metallicRoughnessTexture() {
    return std::any_cast<SampledTexture2DPtr>(shaderData.getData(PBRMaterial::_metallicRoughnessTextureProp).value());
}

void PBRMaterial::setMetallicRoughnessTexture(const SampledTexture2DPtr &newValue) {
    shaderData.setSampledTexure(PBRMaterial::_metallicRoughnessTextureProp, newValue);
    if (newValue) {
        shaderData.enableMacro(HAS_METALROUGHNESSMAP);
    } else {
        shaderData.disableMacro(HAS_METALROUGHNESSMAP);
    }
}

PBRMaterial::PBRMaterial() : PBRBaseMaterial(),
                             _metallicProp(Shader::createProperty("u_metal", ShaderDataGroup::Material)),
                             _roughnessProp(Shader::createProperty("u_roughness", ShaderDataGroup::Material)),
                             _metallicRoughnessTextureProp(Shader::createProperty("u_metallicRoughnessTexture", ShaderDataGroup::Material)) {
    shaderData.enableMacro(IS_METALLIC_WORKFLOW);
    shaderData.setData(PBRMaterial::_metallicProp, 1.f);
    shaderData.setData(PBRMaterial::_roughnessProp, 1.f);
}

}// namespace vox
