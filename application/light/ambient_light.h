//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "math/matrix4x4.h"
#include "math/spherical_harmonics3.h"
#include "framework/core/sampler.h"
#include "texture/texture.h"

namespace vox {
class Scene;

/**
 * Diffuse mode.
 */
enum class DiffuseMode {
    /** Solid color mode. */
    SOLID_COLOR,

    /** Texture mode. */
    TEXTURE,

    /**
     * SH mode
     * @remarks
     * Use SH3 to represent irradiance environment maps efficiently, allowing for interactive rendering of diffuse
     * objects under distant illumination.
     */
    SPHERICAL_HARMONICS
};

/**
 * Ambient light.
 */
class AmbientLight {
public:
    struct alignas(16) EnvMapLight {
        Vector3F diffuse;
        uint32_t mip_map_level;
        float diffuse_intensity;
        float specular_intensity;
    };

    AmbientLight();

    void set_scene(Scene *value);

    /**
     * Diffuse mode of ambient light.
     */
    DiffuseMode get_diffuse_mode();

    void set_diffuse_mode(DiffuseMode value);

    /**
     * Diffuse reflection solid color.
     * @remarks Effective when diffuse reflection mode is `DiffuseMode.SolidColor`.
     */
    [[nodiscard]] Color get_diffuse_solid_color() const;

    void set_diffuse_solid_color(const Color &value);

    /**
     * Diffuse reflection spherical harmonics 3.
     * @remarks Effective when diffuse reflection mode is `DiffuseMode.SphericalHarmonics`.
     */
    const SphericalHarmonics3 &get_diffuse_spherical_harmonics();

    void set_diffuse_spherical_harmonics(const SphericalHarmonics3 &value);

    /**
     * Diffuse reflection intensity.
     */
    [[nodiscard]] float get_diffuse_intensity() const;

    void set_diffuse_intensity(float value);

public:
    /**
     * Whether to decode from SpecularTexture with RGBM format.
     */
    [[nodiscard]] bool get_specular_texture_decode_rgbm() const;

    void set_specular_texture_decode_rgbm(bool value);

    /**
     * Specular reflection texture.
     * @remarks This texture must be baked from MetalLoader::createSpecularTexture
     */
    std::shared_ptr<Texture> get_specular_texture();

    void set_specular_texture(const std::shared_ptr<Texture> &value);

    /**
     * Specular reflection intensity.
     */
    [[nodiscard]] float get_specular_intensity() const;

    void set_specular_intensity(float value);

private:
    static std::array<float, 27> pre_compute_sh(const SphericalHarmonics3 &sh);

    VkSamplerCreateInfo sampler_create_info_;
    std::unique_ptr<core::Sampler> sampler_{nullptr};

    EnvMapLight env_map_light_;
    const std::string env_map_property_;

    SphericalHarmonics3 diffuse_spherical_harmonics_;
    std::array<float, 27> sh_array_{};
    const std::string diffuse_sh_property_;

    bool specular_texture_decode_rgbm_{false};
    std::shared_ptr<Texture> specular_reflection_{nullptr};
    const std::string specular_texture_property_;

    Scene *scene_{nullptr};
    DiffuseMode diffuse_mode_ = DiffuseMode::SOLID_COLOR;
};

}// namespace vox
