//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <pxr/usd/usd/prim.h>

namespace vox {
//
// Connection Editor prototype
//
PXR_NAMESPACE_USING_DIRECTIVE

void draw_connection_editor(const UsdPrim &prim);

}// namespace vox