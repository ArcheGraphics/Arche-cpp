//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef updateFlag_hpp
#define updateFlag_hpp

#include <stdio.h>

namespace vox {
class UpdateFlagManager;

/**
 * Used to update tags.
 */
class UpdateFlag {
public:
    bool flag = true;

    UpdateFlag(UpdateFlagManager *_flags);

    /**
     * Destroy.
     */
    void destroy();

private:
    UpdateFlagManager *_flags;
};

}// namespace vox

#endif /* updateFlag_hpp */
