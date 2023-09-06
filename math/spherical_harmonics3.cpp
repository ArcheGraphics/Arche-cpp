//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "math/spherical_harmonics3.h"

namespace vox {

SphericalHarmonics3::SphericalHarmonics3() = default;

SphericalHarmonics3::SphericalHarmonics3(std::array<float, 27> coefficients) : coefficients_(coefficients) {}

const std::array<float, 27> &SphericalHarmonics3::coefficients() const { return coefficients_; }

void SphericalHarmonics3::addLight(const Vector3F &direction, const Color &c, float delta_solid_angle) {
    /**
     * Implements `EvalSHBasis` from [Projection from Cube maps] in http://www.ppsloan.org/publications/StupidSH36.pdf.
     *
     * Basis constants
     * 0: Math.sqrt(1/(4 * Math.PI))
     *
     * 1: -Math.sqrt(3 / (4 * Math.PI))
     * 2: Math.sqrt(3 / (4 * Math.PI))
     * 3: -Math.sqrt(3 / (4 * Math.PI))
     *
     * 4: Math.sqrt(15 / (4 * Math.PI))
     * 5: -Math.sqrt(15 / (4 * Math.PI))
     * 6: Math.sqrt(5 / (16 * Math.PI))
     * 7: -Math.sqrt(15 / (4 * Math.PI)）
     * 8: Math.sqrt(15 / (16 * Math.PI))
     */

    auto color = c * delta_solid_angle;
    auto &coe = coefficients_;

    const float kBv0 = 0.282095f;                                                            // basis0 = 0.886227
    const float kBv1 = -0.488603f * direction.y;                                             // basis1 = -0.488603
    const float kBv2 = 0.488603f * direction.z;                                              // basis2 = 0.488603
    const float kBv3 = -0.488603f * direction.x;                                             // basis3 = -0.488603
    const float kBv4 = 1.092548f * (direction.x * direction.y);                              // basis4 = 1.092548
    const float kBv5 = -1.092548f * (direction.y * direction.z);                             // basis5 = -1.092548
    const float kBv6 = 0.315392f * (3 * direction.z * direction.z - 1);                      // basis6 = 0.315392
    const float kBv7 = -1.092548f * (direction.x * direction.z);                             // basis7 = -1.092548
    const float kBv8 = 0.546274f * (direction.x * direction.x - direction.y * direction.y);  // basis8 = 0.546274

    coe[0] += color.r * kBv0;
    coe[1] += color.g * kBv0;
    coe[2] += color.b * kBv0;

    coe[3] += color.r * kBv1;
    coe[4] += color.g * kBv1;
    coe[5] += color.b * kBv1;
    coe[6] += color.r * kBv2;
    coe[7] += color.g * kBv2;
    coe[8] += color.b * kBv2;
    coe[9] += color.r * kBv3;
    coe[10] += color.g * kBv3;
    coe[11] += color.b * kBv3;

    coe[12] += color.r * kBv4;
    coe[13] += color.g * kBv4;
    coe[14] += color.b * kBv4;
    coe[15] += color.r * kBv5;
    coe[16] += color.g * kBv5;
    coe[17] += color.b * kBv5;
    coe[18] += color.r * kBv6;
    coe[19] += color.g * kBv6;
    coe[20] += color.b * kBv6;
    coe[21] += color.r * kBv7;
    coe[22] += color.g * kBv7;
    coe[23] += color.b * kBv7;
    coe[24] += color.r * kBv8;
    coe[25] += color.g * kBv8;
    coe[26] += color.b * kBv8;
}

Color SphericalHarmonics3::operator()(const Vector3F &direction) {
    /**
     * Equations based on data from: http://ppsloan.org/publications/StupidSH36.pdf
     *
     *
     * Basis constants
     * 0: Math.sqrt(1/(4 * Math.PI))
     *
     * 1: -Math.sqrt(3 / (4 * Math.PI))
     * 2: Math.sqrt(3 / (4 * Math.PI))
     * 3: -Math.sqrt(3 / (4 * Math.PI))
     *
     * 4: Math.sqrt(15 / (4 * Math.PI)）
     * 5: -Math.sqrt(15 / (4 * Math.PI))
     * 6: Math.sqrt(5 / (16 * Math.PI)）
     * 7: -Math.sqrt(15 / (4 * Math.PI)）
     * 8: Math.sqrt(15 / (16 * Math.PI)）
     *
     *
     * Convolution kernel
     * 0: Math.PI
     * 1: (2 * Math.PI) / 3
     * 2: Math.PI / 4
     */

    const auto &coe = coefficients_;

    const float kBv0 = 0.886227f;                                          // kernel0 * basis0 = 0.886227
    const float kBv1 = -1.023327f * direction.y;                           // kernel1 * basis1 = -1.023327
    const float kBv2 = 1.023327f * direction.z;                            // kernel1 * basis2 = 1.023327
    const float kBv3 = -1.023327f * direction.x;                           // kernel1 * basis3 = -1.023327
    const float kBv4 = 0.858086f * direction.y * direction.x;              // kernel2 * basis4 = 0.858086
    const float kBv5 = -0.858086f * direction.y * direction.z;             // kernel2 * basis5 = -0.858086
    const float kBv6 = 0.247708f * (3.f * direction.z * direction.z - 1);  // kernel2 * basis6 = 0.247708
    const float kBv7 = -0.858086f * direction.z * direction.x;             // kernel2 * basis7 = -0.858086
    const float kBv8 =
            0.429042f * (direction.x * direction.x - direction.y * direction.y);  // kernel2 * basis8 = 0.429042

    // l0
    float r = coe[0] * kBv0;
    float g = coe[1] * kBv0;
    float b = coe[2] * kBv0;

    // l1
    r += coe[3] * kBv1 + coe[6] * kBv2 + coe[9] * kBv3;
    g += coe[4] * kBv1 + coe[7] * kBv2 + coe[10] * kBv3;
    b += coe[5] * kBv1 + coe[8] * kBv2 + coe[11] * kBv3;

    // l2
    r += coe[12] * kBv4 + coe[15] * kBv5 + coe[18] * kBv6 + coe[21] * kBv7 + coe[24] * kBv8;
    g += coe[13] * kBv4 + coe[16] * kBv5 + coe[19] * kBv6 + coe[22] * kBv7 + coe[25] * kBv8;
    b += coe[14] * kBv4 + coe[17] * kBv5 + coe[20] * kBv6 + coe[23] * kBv7 + coe[26] * kBv8;

    return {r, g, b, 1.0};
}

SphericalHarmonics3 SphericalHarmonics3::operator*(float s) {
    auto src = coefficients_;

    src[0] *= s;
    src[1] *= s;
    src[2] *= s;
    src[3] *= s;
    src[4] *= s;
    src[5] *= s;
    src[6] *= s;
    src[7] *= s;
    src[8] *= s;
    src[9] *= s;
    src[10] *= s;
    src[11] *= s;
    src[12] *= s;
    src[13] *= s;
    src[14] *= s;
    src[15] *= s;
    src[16] *= s;
    src[17] *= s;
    src[18] *= s;
    src[19] *= s;
    src[20] *= s;
    src[21] *= s;
    src[22] *= s;
    src[23] *= s;
    src[24] *= s;
    src[25] *= s;
    src[26] *= s;
    return {src};
}

}  // namespace vox
