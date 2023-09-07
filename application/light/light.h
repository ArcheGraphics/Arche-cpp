//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "math/matrix4x4.h"
#include "ecs/component.h"

namespace vox {
/**
 * Light base class.
 */
class Light : public Component {
public:
    /**
     * Each type of light source is at most 10, beyond which it will not take effect.
     * */
    static constexpr uint32_t max_light_ = 10;

    explicit Light(Entity *entity);

    /**
     * View matrix.
     */
    Matrix4x4F get_view_matrix();

    /**
     * Inverse view matrix.
     */
    Matrix4x4F get_inverse_view_matrix();

public:
    [[nodiscard]] bool shadow_enabled() const;

    void enable_shadow(bool enabled);

    /**
     * Shadow bias.
     */
    [[nodiscard]] float get_shadow_bias() const;

    void set_shadow_bias(float value);

    /**
     * Shadow intensity, the larger the value, the clearer and darker the shadow.
     */
    [[nodiscard]] float get_shadow_intensity() const;

    void set_shadow_intensity(float value);

    /**
     * Pixel range used for shadow PCF interpolation.
     */
    [[nodiscard]] float get_shadow_radius() const;

    void set_shadow_radius(float value);

    virtual Matrix4x4F get_shadow_projection_matrix() = 0;

private:
    bool enable_shadow_ = false;
    float shadow_bias_ = 0.005;
    float shadow_intensity_ = 0.2;
    float shadow_radius_ = 1;
};

}// namespace vox
