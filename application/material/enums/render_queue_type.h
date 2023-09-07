//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

namespace vox {
/**
 * Render queue type.
 */
struct RenderQueueType {
    enum Enum {
        /** Opaque queue. */
        OPAQUE = 1000,
        /** Opaque queue, alpha cutoff. */
        ALPHA_TEST = 2000,
        /** Transparent queue, rendering from back to front to ensure correct rendering of transparent objects. */
        TRANSPARENT = 3000
    };
};
}// namespace vox