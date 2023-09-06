//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef direct_light_hpp
#define direct_light_hpp

#include "light.h"
#include "color.h"
#include "shader_common.h"

namespace vox {
/**
 * Directional light.
 */
class DirectLight : public Light {
public:
    /** Light color. */
    Color color = Color(1, 1, 1, 1);
    /** Light intensity. */
    float intensity = 1.0;

    DirectLight(Entity *entity);

public:
    Matrix4x4F shadowProjectionMatrix() override;

    Vector3F direction();

private:
    friend class LightManager;

    /**
     * Mount to the current Scene.
     */
    void _onEnable() override;

    /**
     * Unmount from the current Scene.
     */
    void _onDisable() override;

    void _updateShaderData(DirectLightData &shaderData);
};

}// namespace vox

#endif /* direct_light_hpp */
