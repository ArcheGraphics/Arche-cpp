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
 * Directional light.
 */
class DirectLight : public Light {
public:
    struct DirectLightData {
        Vector3F color;
        float color_pad;// for align
        Vector3F direction;
        float direction_pad;// for align
    };

    /** Light color. */
    Color color_ = Color(1, 1, 1, 1);
    /** Light intensity. */
    float intensity_ = 1.0;

    explicit DirectLight(Entity *entity);

public:
    Matrix4x4F get_shadow_projection_matrix() override;

    Vector3F get_direction();

private:
    friend class LightManager;

    /**
     * Mount to the current Scene.
     */
    void on_enable() override;

    /**
     * Unmount from the current Scene.
     */
    void on_disable() override;

    void update_shader_data(DirectLightData &shader_data);
};

}// namespace vox
