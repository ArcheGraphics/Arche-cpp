//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "math/color.h"

namespace vox {
const Color Color::red = {1.f, 0.f, 0.f};
const Color Color::green = {0.f, 1.f, 0.f};
const Color Color::blue = {0.f, 0.f, 1.f};
const Color Color::white = {1.f, 1.f, 1.f};
const Color Color::black = {0.f, 0.f, 0.f};
const Color Color::grey = {0.5f, 0.5f, 0.5f};
const Color Color::yellow = {1.f, 1.f, 0.f};
const Color Color::cyan = {0.f, 1.f, 1.f};
const Color Color::magenta = {1.f, 0.f, 1.f};

Color Color::lightened(float amount) const {
    amount = std::max(0.0f, std::min(1.0f, amount));
    return {(1.0f - amount) * r + amount * 1.0f, (1.0f - amount) * g + amount * 1.0f, (1.0f - amount) * +amount * 1.0f,
            a};
}

unsigned int Color::toABGR32() const {
    auto new_a = (unsigned int)std::round(a * 255.0f);
    auto new_b = (unsigned int)std::round(b * 255.0f);
    auto new_g = (unsigned int)std::round(g * 255.0f);
    auto new_r = (unsigned int)std::round(r * 255.0f);
    return ((new_a << 24) | (new_b << 16) | (new_g << 8) | new_r);
}

}  // namespace vox
