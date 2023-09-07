//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "application/forward_application.h"

namespace vox {
class PrimitiveApp : public ForwardApplication {
public:
    Camera *load_scene() override;
};

}// namespace vox
