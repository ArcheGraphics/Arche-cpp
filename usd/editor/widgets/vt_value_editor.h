//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <string>
#include <array>
#include <pxr/base/vt/value.h>
#include <pxr/usd/sdf/valueTypeName.h>

PXR_NAMESPACE_USING_DIRECTIVE

namespace vox {
/// Draw an editor for a VtValue.
/// Returns the new edited value if there was an edition else VtValue()
VtValue draw_vt_value(const std::string &label, const VtValue &value);

///
/// Specialized VtValue editors
///

/// TfToken editor
VtValue draw_tf_token(const std::string &label, const TfToken &token, const VtValue &allowedTokens = VtValue());
VtValue draw_tf_token(const std::string &label, const VtValue &value, const VtValue &allowedTokens = VtValue());

/// Color editor. It supports vec3f and array with one vec3f value
VtValue draw_color_value(const std::string &label, const VtValue &value);

/// Helper function to return all the value type names. I couldn't find a function in usd doing that.
const std::array<SdfValueTypeName, 106> &get_all_value_type_names();

/// Helper function to return all spec type names. The function is not exposed in the usd api
const std::vector<std::string> &get_all_spec_type_names();
}// namespace vox