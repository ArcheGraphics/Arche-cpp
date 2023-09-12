//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <pxr/usd/usd/stage.h>

PXR_NAMESPACE_USING_DIRECTIVE

namespace vox {
void draw_attribute_value_at_time(UsdAttribute &attribute, UsdTimeCode currentTime = UsdTimeCode::Default());
void draw_usd_prim_properties(UsdPrim &obj, UsdTimeCode currentTime = UsdTimeCode::Default());

/// Should go in TransformEditor.h
bool draw_xforms_common(UsdPrim &prim, UsdTimeCode currentTime = UsdTimeCode::Default());

/// Should go in a UsdPrimEditors file instead
void draw_usd_prim_edit_target(const UsdPrim &prim);
}// namespace vox