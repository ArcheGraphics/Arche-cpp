//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <pxr/usd/sdf/layer.h>
#include <pxr/usd/sdf/primSpec.h>

#include "Selection.h"

namespace vox {
void DrawLayerPrimHierarchy(SdfLayerRefPtr layer, const Selection &selectedPrim);
}// namespace vox