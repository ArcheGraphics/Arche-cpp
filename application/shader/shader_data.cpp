//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "shader_data.h"
#include "shader.h"

namespace vox {
std::optional<std::any> ShaderData::getData(const std::string &property_name) const {
    auto property = Shader::getPropertyByName(property_name);
    if (property.has_value()) {
        return getData(property.value());
    } else {
        return std::nullopt;
    }
}

std::optional<std::any> ShaderData::getData(const ShaderProperty &property) const {
    return getData(property.uniqueId);
}

std::optional<std::any> ShaderData::getData(uint32_t uniqueID) const {
    auto iter = _properties.find(uniqueID);
    if (iter != _properties.end()) {
        return iter->second;
    }

    auto functorIter = _shaderBufferFunctors.find(uniqueID);
    if (functorIter != _shaderBufferFunctors.end()) {
        return functorIter->second();
    }

    return std::nullopt;
}

void ShaderData::setBufferFunctor(const std::string &property_name,
                                  std::function<std::shared_ptr<MTL::Buffer>()> functor) {
    auto property = Shader::getPropertyByName(property_name);
    if (property.has_value()) {
        setBufferFunctor(property.value(), functor);
    } else {
        assert(false && "can't find property");
    }
}

void ShaderData::setBufferFunctor(ShaderProperty property,
                                  std::function<std::shared_ptr<MTL::Buffer>()> functor) {
    _shaderBufferFunctors.insert(std::make_pair(property.uniqueId, functor));
}

void ShaderData::setData(const std::string &property_name, std::any value) {
    auto property = Shader::getPropertyByName(property_name);
    if (property.has_value()) {
        setData(property.value(), value);
    } else {
        assert(false && "can't find property");
    }
}

void ShaderData::setData(ShaderProperty property, std::any value) {
    _properties[property.uniqueId] = value;
}

void ShaderData::setSampledTexure(const std::string &property, const SampledTexturePtr &value) {
    setData(property, value);
}

void ShaderData::setSampledTexure(ShaderProperty property, const SampledTexturePtr &value) {
    setData(property, value);
}

//MARK: - Macro
void ShaderData::enableMacro(MacroName macroName) {
    _macroCollection._value.insert(std::make_pair(macroName, std::make_pair(1, MTL::DataTypeBool)));
}

void ShaderData::enableMacro(MacroName macroName, std::pair<int, MTL::DataType> value) {
    _macroCollection._value.insert(std::make_pair(macroName, value));
}

void ShaderData::disableMacro(MacroName macroName) {
    auto iter = _macroCollection._value.find(macroName);
    if (iter != _macroCollection._value.end()) {
        _macroCollection._value.erase(iter);
    }
}

void ShaderData::mergeMacro(const ShaderMacroCollection &macros,
                            ShaderMacroCollection &result) const {
    ShaderMacroCollection::unionCollection(macros, _macroCollection, result);
}

}// namespace vox
