//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <memory>
#include <vector>

#include "base/update_flag.h"

namespace vox {
class UpdateFlagManager {
public:
    std::unique_ptr<UpdateFlag> registration();

    void distribute();

private:
    friend class UpdateFlag;

    std::vector<UpdateFlag *> update_flags_;
};

}// namespace vox