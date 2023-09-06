//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef render_queue_type_h
#define render_queue_type_h

/**
 * Render queue type.
 */
struct RenderQueueType {
    enum Enum {
        /** Opaque queue. */
        Opaque = 1000,
        /** Opaque queue, alpha cutoff. */
        AlphaTest = 2000,
        /** Transparent queue, rendering from back to front to ensure correct rendering of transparent objects. */
        Transparent = 3000
    };
};

#endif /* render_queue_type_h */
