//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef pbr_base_material_hpp
#define pbr_base_material_hpp

#include "base_material.h"
#include "color.h"
#include "vector4.h"
#include "texture/sampled_texture2d.h"

namespace vox {
/**
 * PBR (Physically-Based Rendering) Material.
 */
class PBRBaseMaterial : public BaseMaterial {
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
     * Normal texture.
     */
    SampledTexture2DPtr normalTexture() const;

    void setNormalTexture(const SampledTexture2DPtr &newValue);

    /**
     * Normal texture intensity.
     */
    float normalTextureIntensity() const;

    void setNormalTextureIntensity(float newValue);

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
     * Occlusion texture.
     */
    SampledTexture2DPtr occlusionTexture() const;

    void setOcclusionTexture(const SampledTexture2DPtr &newValue);

    /**
     * Occlusion texture intensity.
     */
    float occlusionTextureIntensity() const;

    void setOcclusionTextureIntensity(float newValue);

    /**
     * Tiling and offset of main textures.
     */
    Vector4F tilingOffset() const;

    void setTilingOffset(const Vector4F &newValue);

protected:
    /**
     * Create a pbr base material instance.
     */
    explicit PBRBaseMaterial();

private:
    ShaderProperty _tilingOffsetProp;
    ShaderProperty _normalTextureIntensityProp;
    ShaderProperty _occlusionTextureIntensityProp;

    ShaderProperty _baseColorProp;
    ShaderProperty _emissiveColorProp;

    ShaderProperty _baseTextureProp;
    ShaderProperty _normalTextureProp;
    ShaderProperty _emissiveTextureProp;
    ShaderProperty _occlusionTextureProp;
};

}// namespace vox

#endif /* pbr_base_material_hpp */
