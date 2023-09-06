//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef light_hpp
#define light_hpp

#include "component.h"
#include "matrix4x4.h"

namespace vox {
/**
 * Light base class.
 */
class Light : public Component {
public:
    /**
     * Each type of light source is at most 10, beyond which it will not take effect.
     * */
    static constexpr uint32_t MAX_LIGHT = 10;

    Light(Entity *entity);

    /**
     * View matrix.
     */
    Matrix4x4F viewMatrix();

    /**
     * Inverse view matrix.
     */
    Matrix4x4F inverseViewMatrix();

public:
    bool enableShadow();

    void setEnableShadow(bool enabled);

    /**
     * Shadow bias.
     */
    float shadowBias();

    void setShadowBias(float value);

    /**
     * Shadow intensity, the larger the value, the clearer and darker the shadow.
     */
    float shadowIntensity();

    void setShadowIntensity(float value);

    /**
     * Pixel range used for shadow PCF interpolation.
     */
    float shadowRadius();

    void setShadowRadius(float value);

    virtual Matrix4x4F shadowProjectionMatrix() = 0;

private:
    bool _enableShadow = false;
    float _shadowBias = 0.005;
    float _shadowIntensity = 0.2;
    float _shadowRadius = 1;
};

}// namespace vox
#endif /* light_hpp */
