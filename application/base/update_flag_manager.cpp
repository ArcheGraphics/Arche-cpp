//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "update_flag_manager.h"

namespace vox {
std::unique_ptr<UpdateFlag> UpdateFlagManager::registration() {
    return std::make_unique<UpdateFlag>(this);
}

void UpdateFlagManager::distribute() {
    for (size_t i = 0; i < _updateFlags.size(); i++) {
        _updateFlags[i]->flag = true;
    }
}

}// namespace vox
