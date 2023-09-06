//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "shader.h"
#include <glog/logging.h>

namespace vox {
std::unordered_map<std::string, std::unique_ptr<Shader>> Shader::_shaderMap = {};
std::unordered_map<std::string, ShaderProperty> Shader::_propertyNameMap = {};

Shader::Shader(const std::string &name,
               const std::string &vertexSource,
               const std::string &fragmentSource) : name(name),
                                                    _vertexSource(vertexSource),
                                                    _fragmentSource(fragmentSource) {
}

const std::string &Shader::vertexSource() {
    return _vertexSource;
}

const std::string &Shader::fragmentSource() {
    return _fragmentSource;
}

Shader *Shader::create(const std::string &name,
                       const std::string &vertexSource,
                       const std::string &fragmentSource) {
    auto iter = Shader::_shaderMap.find(name);

    if (iter != Shader::_shaderMap.end()) {
        LOG(ERROR) << ("Shader named" + name + "already exists.") << std::endl;
    }
    auto shader = std::make_unique<Shader>(name, vertexSource, fragmentSource);
    auto shaderPtr = shader.get();
    Shader::_shaderMap.insert(std::make_pair(name, std::move(shader)));
    return shaderPtr;
}

Shader *Shader::find(const std::string &name) {
    auto iter = Shader::_shaderMap.find(name);
    if (iter != Shader::_shaderMap.end()) {
        return iter->second.get();
    } else {
        return nullptr;
    }
}

//MARK: - Property
std::optional<ShaderProperty> Shader::getPropertyByName(const std::string &name) {
    auto iter = Shader::_propertyNameMap.find(name);
    if (iter != Shader::_propertyNameMap.end()) {
        return iter->second;
    } else {
        return std::nullopt;
    }
}

ShaderProperty Shader::createProperty(const std::string &name, ShaderDataGroup group) {
    auto iter = Shader::_propertyNameMap.find(name);
    if (iter != Shader::_propertyNameMap.end()) {
        return iter->second;
    } else {
        auto property = ShaderProperty(name, group);
        Shader::_propertyNameMap.insert(std::make_pair(name, property));
        return property;
    }
}

std::optional<ShaderDataGroup> Shader::getShaderPropertyGroup(const std::string &propertyName) {
    auto iter = Shader::_propertyNameMap.find(propertyName);
    if (iter != Shader::_propertyNameMap.end()) {
        return iter->second.group;
    } else {
        return std::nullopt;
    }
}

}// namespace vox
