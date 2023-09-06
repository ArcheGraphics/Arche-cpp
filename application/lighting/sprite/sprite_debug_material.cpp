//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "sprite_debug_material.h"

namespace vox {
SpriteDebugMaterial::SpriteDebugMaterial(bool isSpotLight) : BaseMaterial(Shader::find(isSpotLight ? "spotlight_sprite_debug" : "pointlight_sprite_debug")) {
    setIsTransparent(true);
    setBlendMode(BlendMode::Additive);
}

}// namespace vox
