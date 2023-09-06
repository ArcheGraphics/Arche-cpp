//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <array>

#include "math/color.h"
#include "math/vector3.h"

namespace vox {
/**
 * Use SH3 to represent irradiance environment maps efficiently, allowing for interactive rendering of diffuse objects
 * under distant illumination.
 * @remarks
 * https://graphics.stanford.edu/papers/envmap/envmap.pdf
 * http://www.ppsloan.org/publications/StupidSH36.pdf
 * https://google.github.io/filament/Filament.md.html#annex/sphericalharmonics
 */
class SphericalHarmonics3 {
public:
    SphericalHarmonics3();

    SphericalHarmonics3(std::array<float, 27> coefficients);

    [[nodiscard]] const std::array<float, 27> &coefficients() const;

    /**
     * Add light to SphericalHarmonics3.
     * @param direction - Light direction
     * @param color - Light color
     * @param delta_solid_angle - The delta solid angle of the light
     */
    void addLight(const Vector3F &direction, const Color &color, float delta_solid_angle);

    /**
     * Evaluates the color for the specified direction.
     * @param direction - Specified direction
     * @return out - Out color
     */
    Color operator()(const Vector3F &direction);

    /**
     * Scale the coefficients.
     * @param s - The amount by which to scale the SphericalHarmonics3
     */
    SphericalHarmonics3 operator*(float s);

private:
    /** The coefficients of SphericalHarmonics3. */
    std::array<float, 27> coefficients_{};
};

}  // namespace vox
