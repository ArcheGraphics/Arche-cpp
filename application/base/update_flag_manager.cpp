//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "base/update_flag_manager.h"

namespace vox {
std::unique_ptr<UpdateFlag> UpdateFlagManager::registration() { return std::make_unique<UpdateFlag>(this); }

void UpdateFlagManager::distribute() {
    for (auto &update_flag : update_flags_) {
        update_flag->flag_ = true;
    }
}

}// namespace vox
