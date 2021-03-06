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
    struct PBRData {
        float metallic = 1.f;
        float roughness = 1.f;
        float _pad1, _pad2;
    };
    /**
     * Metallic.
     */
    float metallic();
    
    void setMetallic(float newValue);
    
    /**
     * Roughness.
     */
    float roughness();
    
    void setRoughness(float newValue);
    
    /**
     * Roughness metallic texture.
     * @remarks G channel is roughness, B channel is metallic
     */
    SampledTexture2DPtr metallicRoughnessTexture();
    
    void setMetallicRoughnessTexture(SampledTexture2DPtr newValue);
    
    /**
     * Create a pbr metallic-roughness workflow material instance.
     */
    explicit PBRMaterial(wgpu::Device& device);
    
private:
    PBRData _pbrData;
    ShaderProperty _pbrProp;
    
    SampledTexture2DPtr _metallicRoughnessTexture{nullptr};
    ShaderProperty _metallicRoughnessTextureProp;
    ShaderProperty _metallicRoughnessSamplerProp;
};

}

#endif /* pbr_material_hpp */
