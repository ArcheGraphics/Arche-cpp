//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <cstdio>

namespace vox {
class UpdateFlagManager;

/**
 * Used to update tags.
 */
class UpdateFlag {
public:
    bool flag_ = true;

    explicit UpdateFlag(UpdateFlagManager *flags);

    ~UpdateFlag();

private:
    UpdateFlagManager *flags_{nullptr};
};

}// namespace vox