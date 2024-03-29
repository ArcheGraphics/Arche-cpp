//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <pxr/usd/sdf/path.h>
#include <pxr/usd/sdf/layer.h>
#include "selection.h"

PXR_NAMESPACE_USING_DIRECTIVE

namespace vox {
VtValue draw_vt_array_value(const VtValue &value);
}// namespace vox