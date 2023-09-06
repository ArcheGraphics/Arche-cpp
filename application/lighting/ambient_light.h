//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef ambient_light_hpp
#define ambient_light_hpp

#include "scene_forward.h"
#include <Metal/Metal.hpp>

#include "spherical_harmonics3.h"
#include "shader/shader_property.h"
#include "shader_common.h"
#include "texture/sampled_texturecube.h"

namespace vox {
/**
 * Diffuse mode.
 */
enum class DiffuseMode {
    /** Solid color mode. */
    SolidColor,

    /** Texture mode. */
    Texture,

    /**
     * SH mode
     * @remarks
     * Use SH3 to represent irradiance environment maps efficiently,
     * allowing for interactive rendering of diffuse objects under distant illumination.
     */
    SphericalHarmonics
};

/**
 * Ambient light.
 */
class AmbientLight {
public:
    AmbientLight(Scene *value);

    void registerUploader(Scene *scene);

    /**
     * Diffuse mode of ambient light.
     */
    DiffuseMode diffuseMode() const;

    void setDiffuseMode(DiffuseMode value);

    /**
     * Diffuse reflection solid color.
     * @remarks Effective when diffuse reflection mode is `DiffuseMode.SolidColor`.
     */
    Color diffuseSolidColor() const;

    void setDiffuseSolidColor(const Color &value);

    /**
     * Diffuse reflection spherical harmonics 3.
     * @remarks Effective when diffuse reflection mode is `DiffuseMode.SphericalHarmonics`.
     */
    const SphericalHarmonics3 &diffuseSphericalHarmonics() const;

    void setDiffuseSphericalHarmonics(const SphericalHarmonics3 &value);

    /**
     * Diffuse reflection texture.
     * @remarks This texture must be baked from MetalLoader::createIrradianceTexture
     */
    SampledTextureCubePtr diffuseTexture() const;

    void setDiffuseTexture(const SampledTextureCubePtr &value);

    /**
     * Diffuse reflection intensity.
     */
    float diffuseIntensity();

    void setDiffuseIntensity(float value);

public:
    /**
     * Whether to decode from specularTexture with RGBM format.
     */
    bool specularTextureDecodeRGBM() const;

    void setSpecularTextureDecodeRGBM(bool value);

    /**
     * Specular reflection texture.
     * @remarks This texture must be baked from MetalLoader::createSpecularTexture
     */
    SampledTextureCubePtr specularTexture() const;

    void setSpecularTexture(const SampledTextureCubePtr &value);

    /**
     * Specular reflection intensity.
     */
    float specularIntensity() const;

    void setSpecularIntensity(float value);

public:
    /**
     * brdf loopup texture.
     * @remarks This texture must be baked from MetalLoader::createBRDFLookupTable
     */
    SampledTexture2DPtr brdfTexture() const;

    void setBRDFTexture(const SampledTexture2DPtr &value);

private:
    std::array<float, 27> _preComputeSH(const SphericalHarmonics3 &sh);

    ShaderProperty _envMapProperty;
    ShaderProperty _diffuseSHProperty;
    ShaderProperty _diffuseTextureProperty;
    ShaderProperty _specularTextureProperty;
    ShaderProperty _brdfTextureProperty;

    Scene *_scene;
    EnvMapLight _envMapLight;

    DiffuseMode _diffuseMode = DiffuseMode::SolidColor;
    SphericalHarmonics3 _diffuseSphericalHarmonics;
    std::array<float, 27> _shArray{};
    SampledTextureCubePtr _diffuseTexture{nullptr};

    bool _specularTextureDecodeRGBM{false};
    SampledTextureCubePtr _specularReflection{nullptr};

    SampledTexture2DPtr _brdfLutTexture{nullptr};
};

}// namespace vox

#endif /* ambient_light_hpp */
