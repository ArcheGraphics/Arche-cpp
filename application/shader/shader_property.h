//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "shader_data_group.h"
#include <string>

namespace vox {
/**
 * Shader property.
 */
struct ShaderProperty {
    /** Shader property name. */
    const std::string name;

    const ShaderDataGroup group;

    const int uniqueId;

    ShaderProperty(const std::string &name, ShaderDataGroup group);

private:
    static int _propertyNameCounter;
};

}// namespace vox