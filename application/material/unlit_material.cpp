//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "unlit_material.h"

namespace vox {
Color UnlitMaterial::baseColor() const {
    return std::any_cast<Color>(shaderData.getData(_baseColorProp).value());
}

void UnlitMaterial::setBaseColor(const Color &newValue) {
    shaderData.setData(UnlitMaterial::_baseColorProp, newValue);
}

SampledTexture2DPtr UnlitMaterial::baseTexture() const {
    return std::any_cast<SampledTexture2DPtr>(shaderData.getData(_baseTextureProp).value());
}

void UnlitMaterial::setBaseTexture(const SampledTexture2DPtr &newValue) {
    shaderData.setSampledTexure(UnlitMaterial::_baseTextureProp, newValue);

    if (newValue) {
        shaderData.enableMacro(HAS_BASE_TEXTURE);
    } else {
        shaderData.disableMacro(HAS_BASE_TEXTURE);
    }
}

Vector4F UnlitMaterial::tilingOffset() const {
    return std::any_cast<Vector4F>(shaderData.getData(_tilingOffsetProp).value());
}

void UnlitMaterial::setTilingOffset(const Vector4F &newValue) {
    shaderData.setData(UnlitMaterial::_tilingOffsetProp, newValue);
}

UnlitMaterial::UnlitMaterial() : BaseMaterial(Shader::find("unlit")),
                                 _baseColorProp(Shader::createProperty("u_baseColor", ShaderDataGroup::Material)),
                                 _baseTextureProp(Shader::createProperty("u_baseTexture", ShaderDataGroup::Material)),
                                 _tilingOffsetProp(Shader::createProperty("u_tilingOffset", ShaderDataGroup::Material)) {
    shaderData.enableMacro(OMIT_NORMAL);
    shaderData.enableMacro(NEED_TILINGOFFSET);

    shaderData.setData(_baseColorProp, Color(1, 1, 1, 1));
    shaderData.setData(_tilingOffsetProp, Vector4F(1, 1, 0, 0));
}

}// namespace vox
