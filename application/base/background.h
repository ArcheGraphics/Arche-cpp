//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "math/color.h"

namespace vox {
/**
 * The Background mode enumeration.
 */
struct BackgroundMode {
    enum Enum {
        /* Solid color. */
        SOLID_COLOR,
        /** Texture */
        TEXTURE
    };
};

/**
 * Background of scene.
 */
struct Background {
public:
    /**
     * Background mode.
     * @defaultValue `BackgroundMode.SolidColor`
     * @remarks If using `BackgroundMode.Sky` mode and material or mesh of the `sky` is not defined, it will downgrade
     * to `BackgroundMode.SolidColor`.
     */
    BackgroundMode::Enum mode = BackgroundMode::Enum::SOLID_COLOR;

    /**
     * Background solid color.
     * @defaultValue `new Color(0.25, 0.25, 0.25, 1.0)`
     * @remarks When `mode` is `BackgroundMode.SolidColor`, the property will take effects.
     */
    Color solid_color = Color(0.25, 0.25, 0.25, 1.0);

    Background() = default;
};

}// namespace vox