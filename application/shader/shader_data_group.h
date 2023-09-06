//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

namespace vox {
/**
 * Shader data grouping.
 */
enum class ShaderDataGroup {
    /** Scene group. */
    Scene,
    /** Camera group. */
    Camera,
    /** Renderer group. */
    Renderer,
    /** material group. */
    Material,

    /** compute group. */
    Compute
};

}// namespace vox