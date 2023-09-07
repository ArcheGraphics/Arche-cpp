//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

namespace vox {
/**
 * Set which face for render.
 */
enum class RenderFace {
    /** Render front face. */
    FRONT,
    /** Render back face. */
    BACK,
    /** Render double face. */
    DOUBLE
};
}// namespace vox