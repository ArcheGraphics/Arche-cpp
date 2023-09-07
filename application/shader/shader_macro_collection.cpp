//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "shader_macro_collection.h"
#include "framework/common/helpers.h"
#include "framework/common/metal_helpers.h"

namespace vox {
std::unordered_map<MacroName, std::pair<int, MTL::DataType>> ShaderMacroCollection::defaultValue = {
    {HAS_UV, {0, MTL::DataTypeBool}},
    {HAS_NORMAL, {0, MTL::DataTypeBool}},
    {HAS_TANGENT, {0, MTL::DataTypeBool}},
    {HAS_VERTEXCOLOR, {0, MTL::DataTypeBool}},

    // Blend Shape
    {HAS_BLENDSHAPE, {0, MTL::DataTypeBool}},
    {HAS_BLENDSHAPE_NORMAL, {0, MTL::DataTypeBool}},
    {HAS_BLENDSHAPE_TANGENT, {0, MTL::DataTypeBool}},

    // Skin
    {HAS_SKIN, {0, MTL::DataTypeBool}},
    {HAS_JOINT_TEXTURE, {0, MTL::DataTypeBool}},
    {JOINTS_COUNT, {0, MTL::DataTypeInt}},

    // Material
    {NEED_ALPHA_CUTOFF, {0, MTL::DataTypeBool}},
    {NEED_WORLDPOS, {0, MTL::DataTypeBool}},
    {NEED_TILINGOFFSET, {0, MTL::DataTypeBool}},
    {HAS_DIFFUSE_TEXTURE, {0, MTL::DataTypeBool}},
    {HAS_SPECULAR_TEXTURE, {0, MTL::DataTypeBool}},
    {HAS_EMISSIVE_TEXTURE, {0, MTL::DataTypeBool}},
    {HAS_NORMAL_TEXTURE, {0, MTL::DataTypeBool}},
    {OMIT_NORMAL, {0, MTL::DataTypeBool}},
    {HAS_BASE_TEXTURE, {0, MTL::DataTypeBool}},
    {HAS_BASE_COLORMAP, {0, MTL::DataTypeBool}},
    {HAS_EMISSIVEMAP, {0, MTL::DataTypeBool}},
    {HAS_OCCLUSIONMAP, {0, MTL::DataTypeBool}},
    {HAS_SPECULARGLOSSINESSMAP, {0, MTL::DataTypeBool}},
    {HAS_METALROUGHNESSMAP, {0, MTL::DataTypeBool}},
    {IS_METALLIC_WORKFLOW, {0, MTL::DataTypeBool}},

    // Light
    {DIRECT_LIGHT_COUNT, {0, MTL::DataTypeInt}},
    {POINT_LIGHT_COUNT, {0, MTL::DataTypeInt}},
    {SPOT_LIGHT_COUNT, {0, MTL::DataTypeInt}},
    {NEED_FORWARD_PLUS, {0, MTL::DataTypeBool}},

    // Enviroment
    {HAS_SH, {0, MTL::DataTypeBool}},
    {HAS_SPECULAR_ENV, {0, MTL::DataTypeBool}},
    {HAS_DIFFUSE_ENV, {0, MTL::DataTypeBool}},

    // Particle Render
    {NEED_PARTICLE_SCATTERING, {0, MTL::DataTypeBool}},
    {NEED_PARTICLE_VECTOR_FIELD, {0, MTL::DataTypeBool}},
    {NEED_PARTICLE_CURL_NOISE, {0, MTL::DataTypeBool}},
    {NEED_PARTICLE_VELOCITY_CONTROL, {0, MTL::DataTypeBool}},

    // Shadow
    {SHADOW_MAP_COUNT, {0, MTL::DataTypeInt}},
    {CUBE_SHADOW_MAP_COUNT, {0, MTL::DataTypeInt}},
};

std::shared_ptr<MTL::FunctionConstantValues> ShaderMacroCollection::createDefaultFunction() {
    std::shared_ptr<MTL::FunctionConstantValues> functionConstants = CLONE_METAL_CUSTOM_DELETER(MTL::FunctionConstantValues, MTL::FunctionConstantValues::alloc()->init());
    for (size_t i = 0; i < TOTAL_COUNT; i++) {
        const auto macro = ShaderMacroCollection::defaultValue[MacroName(i)];

        int value = macro.first;
        auto type = macro.second;
        if (type == MTL::DataTypeBool) {
            bool property;
            if (value == 1) {
                property = true;
            } else {
                property = false;
            }
            functionConstants->setConstantValue(&property, MTL::DataTypeBool, i);
        } else {
            functionConstants->setConstantValue(&value, type, i);
        }
    }
    return functionConstants;
}

void ShaderMacroCollection::unionCollection(const ShaderMacroCollection &left, const ShaderMacroCollection &right,
                                            ShaderMacroCollection &result) {
    result._value.insert(left._value.begin(), left._value.end());
    result._value.insert(right._value.begin(), right._value.end());
}

size_t ShaderMacroCollection::hash() const {
    std::size_t hash{0U};
    for (int i = 0; i < MacroName::TOTAL_COUNT; i++) {
        auto iter = _value.find(MacroName(i));
        if (iter != _value.end()) {
            hash_combine(hash, iter->first);
        }
    }
    return hash;
}

}// namespace vox
