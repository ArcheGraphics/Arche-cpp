//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <random>

#include "unit_tests_utils.h"
#include "math/color.h"

using namespace vox;

TEST(Color, Constructor) {
    const auto color1 = Color(1, 0.5, 0.5, 1);
    auto color2 = Color(0.0);
    color2.r = 1;
    color2.g = 0.5;
    color2.b = 0.5;
    color2.a = 1;

    EXPECT_EQ(color1, color2);
}

TEST(Color, scale) {
    auto color1 = Color(0.5, 0.5, 0.5, 0.5);
    auto color2 = Color(1, 1, 1, 1);

    color1 = color1 * 2.0;
    EXPECT_EQ(color1, color2);

    color2 = color1 * 0.5;
    EXPECT_COLOR_EQ(color2, Color(0.5, 0.5, 0.5, 0.5));
}

TEST(Color, add) {
    auto color1 = Color(1, 0, 0, 0);
    auto color2 = Color(0, 1, 0, 0);

    color1 = color1 + color2;
    EXPECT_COLOR_EQ(color1, Color(1, 1, 0, 0));

    color2 = color1 + Color(0, 0, 1, 0);
    EXPECT_COLOR_EQ(color2, Color(1, 1, 1, 0));
}

TEST(Color, LinearAndGamma) {
    const auto fixColor = [](Color &color) {
        color.r = std::floor(color.r * 1000) / 1000;
        color.g = std::floor(color.g * 1000) / 1000;
        color.b = std::floor(color.b * 1000) / 1000;
    };

    auto colorLinear = Color(0.0);
    auto colorGamma = Color(0.0);
    auto colorNewLinear = Color(0.0);

    std::default_random_engine e;
    std::uniform_real_distribution<float> u(0.0, 1.0);

    for (int i = 0; i < 100; ++i) {
        colorLinear.r = u(e);
        colorLinear.g = u(e);
        colorLinear.b = u(e);
        fixColor(colorLinear);

        colorGamma = colorLinear.toGamma();
        colorNewLinear = colorGamma.toLinear();

        fixColor(colorLinear);
        fixColor(colorNewLinear);

        EXPECT_COLOR_NEAR(colorLinear, Color(colorNewLinear.r, colorNewLinear.g, colorNewLinear.b, colorNewLinear.a),
                          1.0e-3);
    }
}
