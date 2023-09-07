//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "shader_pool.h"
#include "shader.h"

namespace vox {
void ShaderPool::initialization() {
    Shader::create("unlit", "vertex_unlit", "fragment_unlit");
    Shader::create("blinn-phong", "vertex_blinn_phong", "fragment_blinn_phong");
    Shader::create("particle-shader", "vertex_particle", "fragment_particle");
    Shader::create("pbr", "vertex_blinn_phong", "fragment_pbr");
    Shader::create("pbr-specular", "vertex_blinn_phong", "fragment_pbr");
    Shader::create("skybox", "vertex_skybox", "fragment_skybox");

    // MARK: - experimental
    Shader::create("shadow-map", "vertex_shadow_map", "fragment_shadow_map");
    Shader::create("shadow", "vertex_shadow_map", "fragment_shadow");
    Shader::create("background-texture", "vertex_background_texture", "fragment_background_texture");

    Shader::create("experimental", "vertex_experimental", "fragment_experimental");
}

}// namespace vox
