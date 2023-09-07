//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

namespace vox {
/**
 * Layer, used for bit operations.
 */
enum Layer {
    /** Layer 0. */
    LAYER_0 = 0x1,
    /** Layer 1. */
    LAYER_1 = 0x2,
    /** Layer 2. */
    LAYER_2 = 0x4,
    /** Layer 3. */
    LAYER_3 = 0x8,
    /** Layer 4. */
    LAYER_4 = 0x10,
    /** Layer 5. */
    LAYER_5 = 0x20,
    /** Layer 6. */
    LAYER_6 = 0x40,
    /** Layer 7. */
    LAYER_7 = 0x80,
    /** Layer 8. */
    LAYER_8 = 0x100,
    /** Layer 9. */
    LAYER_9 = 0x200,
    /** Layer 10. */
    LAYER_10 = 0x400,
    /** Layer 11. */
    LAYER_11 = 0x800,
    /** Layer 12. */
    LAYER_12 = 0x1000,
    /** Layer 13. */
    LAYER_13 = 0x2000,
    /** Layer 14. */
    LAYER_14 = 0x4000,
    /** Layer 15. */
    LAYER_15 = 0x8000,
    /** Layer 16. */
    LAYER_16 = 0x10000,
    /** Layer 17. */
    LAYER_17 = 0x20000,
    /** Layer 18. */
    LAYER_18 = 0x40000,
    /** Layer 19. */
    LAYER_19 = 0x80000,
    /** Layer 20. */
    LAYER_20 = 0x100000,
    /** Layer 21. */
    LAYER_21 = 0x200000,
    /** Layer 22. */
    LAYER_22 = 0x400000,
    /** Layer 23. */
    LAYER_23 = 0x800000,
    /** Layer 24. */
    LAYER_24 = 0x1000000,
    /** Layer 25. */
    LAYER_25 = 0x2000000,
    /** Layer 26. */
    LAYER_26 = 0x4000000,
    /** Layer 27. */
    LAYER_27 = 0x8000000,
    /** Layer 28. */
    LAYER_28 = 0x10000000,
    /** Layer 29. */
    LAYER_29 = 0x20000000,
    /** Layer 30. */
    LAYER_30 = 0x40000000,
    /** Layer 31. */
    LAYER_31 = 0x80000000,
    /** All layers. */
    EVERYTHING = 0xffffffff,
    /** None layer. */
    NOTHING = 0x0
};

}// namespace vox