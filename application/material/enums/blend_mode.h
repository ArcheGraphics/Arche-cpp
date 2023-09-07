//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

namespace vox {
/**
 * Alpha blend mode.
 */
enum class BlendMode {
    /** SRC ALPHA * SRC + (1 - SRC ALPHA) * DEST */
    NORMAL,
    /** SRC ALPHA * SRC + ONE * DEST */
    ADDITIVE
};
}// namespace vox