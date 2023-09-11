//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <pxr/usd/usd/stage.h>

PXR_NAMESPACE_USING_DIRECTIVE

namespace vox {
void DrawAttributeValueAtTime(UsdAttribute &attribute, UsdTimeCode currentTime = UsdTimeCode::Default());
void DrawUsdPrimProperties(UsdPrim &obj, UsdTimeCode currentTime = UsdTimeCode::Default());

/// Should go in TransformEditor.h
bool DrawXformsCommon(UsdPrim &prim, UsdTimeCode currentTime = UsdTimeCode::Default());

/// Should go in a UsdPrimEditors file instead
void DrawUsdPrimEditTarget(const UsdPrim &prim);
}// namespace vox