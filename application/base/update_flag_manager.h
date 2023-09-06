//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef updateFlag_manager_hpp
#define updateFlag_manager_hpp

#include <memory>
#include <vector>
#include "update_flag.h"

namespace vox {
class UpdateFlagManager {
public:
    std::unique_ptr<UpdateFlag> registration();

    void distribute();

private:
    friend class UpdateFlag;

    std::vector<UpdateFlag *> _updateFlags;
};

}// namespace vox
#endif /* updateFlag_manager_hpp */
