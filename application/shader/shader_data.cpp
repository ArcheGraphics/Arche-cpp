//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "shader_data.h"

namespace vox {
ShaderData::ShaderData(MTL::Device &device) : device_(device) {}

//MARK: - Macro
void ShaderData::enable_macro(MacroName macroName) {
    _macroCollection._value.insert(std::make_pair(macroName, std::make_pair(1, MTL::DataTypeBool)));
}

void ShaderData::enable_macro(MacroName macroName, std::pair<int, MTL::DataType> value) {
    _macroCollection._value.insert(std::make_pair(macroName, value));
}

void ShaderData::disable_macro(MacroName macroName) {
    auto iter = _macroCollection._value.find(macroName);
    if (iter != _macroCollection._value.end()) {
        _macroCollection._value.erase(iter);
    }
}

void ShaderData::merge_macro(const ShaderMacroCollection &macros,
                             ShaderMacroCollection &result) const {
    ShaderMacroCollection::unionCollection(macros, _macroCollection, result);
}

}// namespace vox
