//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "update_flag.h"
#include "update_flag_manager.h"

namespace vox {
UpdateFlag::UpdateFlag(UpdateFlagManager *_flags) : _flags(_flags) {
    _flags->_updateFlags.push_back(this);
}

void UpdateFlag::destroy() {
    _flags->_updateFlags.erase(std::remove(_flags->_updateFlags.begin(),
                                           _flags->_updateFlags.end(), this),
                               _flags->_updateFlags.end());
}

}// namespace vox
