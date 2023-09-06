//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "shader_property.h"

namespace vox {
int ShaderProperty::_propertyNameCounter = 0;

ShaderProperty::ShaderProperty(const std::string &name, ShaderDataGroup group) : name(name),
                                                                                 group(group),
                                                                                 uniqueId(ShaderProperty::_propertyNameCounter) {
    ShaderProperty::_propertyNameCounter += 1;
}

}// namespace vox
