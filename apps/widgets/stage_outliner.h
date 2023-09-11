//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <pxr/usd/usd/stage.h>
#include "Selection.h"// TODO: ideally we should have only pxr headers here

PXR_NAMESPACE_USING_DIRECTIVE

namespace vox {
// TODO: selected could be multiple Path, we should pass a HdSelection instead
void DrawStageOutliner(UsdStageRefPtr stage, Selection &selectedPaths);
}// namespace vox