//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "light.h"
#include "color.h"
#include "shader_common.h"

namespace vox {
/**
 * Spot light.
 */
class SpotLight : public Light {
public:
    /** Light color. */
    Color color = Color(1, 1, 1, 1);
    /** Light intensity. */
    float intensity = 1.0;
    /** Defines a distance cutoff at which the light's intensity must be considered zero. */
    float distance = 5;
    /** Angle, in radians, from centre of spotlight where falloff begins. */
    float angle = M_PI / 6;
    /** Angle, in radians, from falloff begins to ends. */
    float penumbra = M_PI / 12;

    SpotLight(Entity *entity);

public:
    Matrix4x4F shadowProjectionMatrix() override;

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

    void _updateShaderData(SpotLightData &shaderData);
};

}// namespace vox