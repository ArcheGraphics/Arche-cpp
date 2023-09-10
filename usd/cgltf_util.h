//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <cgltf.h>

namespace vox {
bool load_gltf(const char *gltfPath, cgltf_data **data);

const char *cgltf_error_string(cgltf_result result);

const cgltf_accessor *cgltf_find_accessor(const cgltf_primitive *primitive,
                                          const char *name);

cgltf_size cgltf_calc_size(cgltf_type type, cgltf_component_type component_type);

cgltf_size cgltf_decode_uri(char *uri);

bool cgltf_transform_required(const cgltf_texture_transform &transform);
}// namespace vox
