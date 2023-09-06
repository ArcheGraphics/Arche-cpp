//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef sprite_debug_hpp
#define sprite_debug_hpp

#include "script.h"
#include "mesh/buffer_mesh.h"

namespace vox {
class SpriteDebug : public Script {
public:
    SpriteDebug(Entity *entity);

    void onUpdate(float deltaTime) override;

private:
    EntityPtr _spotEntity{nullptr};
    std::shared_ptr<BufferMesh> _spotLightMesh{nullptr};

    EntityPtr _pointEntity{nullptr};
    std::shared_ptr<BufferMesh> _pointLightMesh{nullptr};
};

}// namespace vox
#endif /* sprite_debug_hpp */
