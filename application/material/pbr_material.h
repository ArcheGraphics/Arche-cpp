//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef pbr_material_hpp
#define pbr_material_hpp

#include "pbr_base_material.h"

namespace vox {
/**
 * PBR (Metallic-Roughness Workflow) Material.
 */
class PBRMaterial : public PBRBaseMaterial {
public:
    /**
     * Metallic.
     */
    float metallic() const;

    void setMetallic(float newValue);

    /**
     * Roughness.
     */
    float roughness() const;

    void setRoughness(float newValue);

    /**
     * Roughness metallic texture.
     * @remarks G channel is roughness, B channel is metallic
     */
    SampledTexture2DPtr metallicRoughnessTexture();

    void setMetallicRoughnessTexture(const SampledTexture2DPtr &newValue);

    /**
     * Create a pbr metallic-roughness workflow material instance.
     */
    explicit PBRMaterial();

private:
    ShaderProperty _metallicProp;
    ShaderProperty _roughnessProp;
    ShaderProperty _metallicRoughnessTextureProp;
};

}// namespace vox

#endif /* pbr_material_hpp */
