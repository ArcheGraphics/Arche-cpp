//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef blinn_phong_material_hpp
#define blinn_phong_material_hpp

#include "base_material.h"
#include "color.h"
#include "vector4.h"
#include "texture/sampled_texture2d.h"

namespace vox {
/**
 * Blinn-phong Material.
 */
class BlinnPhongMaterial : public BaseMaterial {
public:
    /**
     * Base color.
     */
    Color baseColor() const;

    void setBaseColor(const Color &newValue);

    /**
     * Base texture.
     */
    SampledTexture2DPtr baseTexture() const;

    void setBaseTexture(const SampledTexture2DPtr &newValue);

    /**
     * Specular color.
     */
    Color specularColor() const;

    void setSpecularColor(const Color &newValue);

    /**
     * Specular texture.
     */
    SampledTexture2DPtr specularTexture() const;

    void setSpecularTexture(const SampledTexture2DPtr &newValue);

    /**
     * Emissive color.
     */
    Color emissiveColor() const;

    void setEmissiveColor(const Color &newValue);

    /**
     * Emissive texture.
     */
    SampledTexture2DPtr emissiveTexture() const;

    void setEmissiveTexture(const SampledTexture2DPtr &newValue);

    /**
     * Normal texture.
     */
    SampledTexture2DPtr normalTexture() const;

    void setNormalTexture(const SampledTexture2DPtr &newValue);

    /**
     * Normal texture intensity.
     */
    float normalIntensity() const;

    void setNormalIntensity(float newValue);

    /**
     * Set the specular reflection coefficient, the larger the value, the more convergent the specular reflection effect.
     */
    float shininess() const;

    void setShininess(float newValue);

    /**
     * Tiling and offset of main textures.
     */
    Vector4F tilingOffset() const;

    void setTilingOffset(const Vector4F &newValue);

    explicit BlinnPhongMaterial();

private:
    ShaderProperty _diffuseColorProp;
    ShaderProperty _specularColorProp;
    ShaderProperty _emissiveColorProp;
    ShaderProperty _tilingOffsetProp;
    ShaderProperty _shininessProp;
    ShaderProperty _normalIntensityProp;

    ShaderProperty _baseTextureProp;
    ShaderProperty _specularTextureProp;
    ShaderProperty _emissiveTextureProp;
    ShaderProperty _normalTextureProp;
};

}// namespace vox

#endif /* blinn_phong_material_hpp */
