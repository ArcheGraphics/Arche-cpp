//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <pxr/usd/sdf/primSpec.h>

PXR_NAMESPACE_USING_DIRECTIVE

namespace vox {
/// Draw modal dialogs to add composition on primspec (reference, payload, inherit, specialize)
void draw_prim_create_reference(const SdfPrimSpecHandle &primSpec);
void draw_prim_create_payload(const SdfPrimSpecHandle &primSpec);
void draw_prim_create_inherit(const SdfPrimSpecHandle &primSpec);
void draw_prim_create_specialize(const SdfPrimSpecHandle &primSpec);

/// Draw multiple tables with the compositions (Reference, Payload, Inherit, Specialize)
void draw_prim_compositions(const SdfPrimSpecHandle &primSpec);

// Draw a text summary of the composition
void draw_prim_composition_summary(const SdfPrimSpecHandle &primSpec);
}// namespace vox