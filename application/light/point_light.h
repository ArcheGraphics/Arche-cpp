//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "math/color.h"
#include "light/light.h"

namespace vox {
/**
 * Point light.
 */
class PointLight : public Light {
public:
    struct PointLightData {
        Vector3F color;
        float color_pad;// for align
        Vector3F position;
        float distance;
    };

    /** Light color. */
    Color color_ = Color(1, 1, 1, 1);
    /** Light intensity. */
    float intensity_ = 1.0;
    /** Defines a distance cutoff at which the light's intensity must be considered zero. */
    float distance_ = 5;

    explicit PointLight(Entity *entity);

public:
    Matrix4x4F get_shadow_projection_matrix() override;

private:
    /**
     * Mount to the current Scene.
     */
    void on_enable() override;

    /**
     * Unmount from the current Scene.
     */
    void on_disable() override;

    void update_shader_data(PointLightData &shader_data);

private:
    friend class LightManager;
};

}// namespace vox
