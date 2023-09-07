//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "material.h"

namespace vox {
Material::Material(MTL::Device &device, std::string name) : device_(device), shader_data(device), name{std::move(name)} {}

}// namespace vox
