//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "sprite_debug.h"
#include "sprite_debug_material.h"
#include "mesh/mesh_renderer.h"
#include "lighting/light_manager.h"

namespace vox {
SpriteDebug::SpriteDebug(Entity *entity) : Script(entity) {
    Shader::create("spotlight_sprite_debug", "vertex_point_light_sprite", "fragment_light_sprite");
    Shader::create("pointlight_sprite_debug", "vertex_spot_light_sprite", "fragment_light_sprite");

    _spotLightMesh = std::make_shared<BufferMesh>();
    _spotLightMesh->addSubMesh(MTL::PrimitiveType::PrimitiveTypeTriangleStrip,
                               MTL::IndexType::IndexTypeUInt16, 4, nullptr);
    _spotEntity = entity->createChild();
    auto spotRenderer = _spotEntity->addComponent<MeshRenderer>();
    spotRenderer->setMaterial(std::make_shared<SpriteDebugMaterial>(true));
    spotRenderer->setMesh(_spotLightMesh);

    _pointLightMesh = std::make_shared<BufferMesh>();
    _pointLightMesh->addSubMesh(MTL::PrimitiveType::PrimitiveTypeTriangleStrip,
                                MTL::IndexType::IndexTypeUInt16, 4, nullptr);
    _pointEntity = entity->createChild();
    auto pointRenderer = _pointEntity->addComponent<MeshRenderer>();
    pointRenderer->setMaterial(std::make_shared<SpriteDebugMaterial>(false));
    pointRenderer->setMesh(_pointLightMesh);
}

void SpriteDebug::onUpdate(float deltaTime) {
    auto spotLightCount = LightManager::getSingleton().spotLights().size();
    if (spotLightCount > 0) {
        _spotLightMesh->setInstanceCount(static_cast<uint32_t>(spotLightCount));
        _spotEntity->setIsActive(true);
    } else {
        _spotEntity->setIsActive(false);
    }

    auto pointLightCount = LightManager::getSingleton().pointLights().size();
    if (pointLightCount > 0) {
        _pointLightMesh->setInstanceCount(static_cast<uint32_t>(pointLightCount));
        _pointEntity->setIsActive(true);
    } else {
        _pointEntity->setIsActive(false);
    }
}

}// namespace vox
