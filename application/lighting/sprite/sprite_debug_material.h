//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef sprite_debug_material_hpp
#define sprite_debug_material_hpp

#include "material/base_material.h"

namespace vox {
class SpriteDebugMaterial : public BaseMaterial {
public:
    SpriteDebugMaterial(bool isSpotLight);
};

}// namespace vox
#endif /* sprite_debug_material_hpp */
