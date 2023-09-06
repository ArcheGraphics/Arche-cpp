//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <unordered_map>
#include <string>
#include <optional>
#include "shader_property.h"
#include "shader_macro_collection.h"

namespace vox {
/**
 * Shader containing vertex and fragment source.
 */
class Shader {
public:
    /** The name of shader. */
    std::string name;

    Shader(const std::string &name, const std::string &vertexSource, const std::string &fragmentSource);

    const std::string &vertexSource();

    const std::string &fragmentSource();

    /**
     * Create a shader.
     * @param name - Name of the shader
     * @param vertexSource - Vertex source code
     * @param fragmentSource - Fragment source code
     */
    static Shader *create(const std::string &name, const std::string &vertexSource, const std::string &fragmentSource);

    /**
     * Find a shader by name.
     * @param name - Name of the shader
     */
    static Shader *find(const std::string &name);

public:
    /**
     * Get shader property by name.
     * @param name - Name of the shader property
     * @returns Shader property
     */
    static std::optional<ShaderProperty> getPropertyByName(const std::string &name);

    static std::optional<ShaderDataGroup> getShaderPropertyGroup(const std::string &propertyName);

    /**
     * Create shader property by name.
     * @param name - Name of the shader property
     * @param group - Group of shader data
     * @returns Shader property
     */
    static ShaderProperty createProperty(const std::string &name, ShaderDataGroup group);

private:
    // common shader map
    static std::unordered_map<std::string, std::unique_ptr<Shader>> _shaderMap;
    static std::unordered_map<std::string, ShaderProperty> _propertyNameMap;

    // object shader data
    int _shaderId = 0;
    std::string _vertexSource;
    std::string _fragmentSource;
};

}// namespace vox