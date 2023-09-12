//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <pxr/usd/sdf/primSpec.h>
#include "selection.h"

PXR_NAMESPACE_USING_DIRECTIVE

namespace vox {
/// Draw the full fledged primspec editor
void draw_sdf_prim_editor(const SdfPrimSpecHandle &primSpec, const Selection &);

void draw_prim_kind(const SdfPrimSpecHandle &primSpec);
void draw_prim_type(const SdfPrimSpecHandle &primSpec, ImGuiComboFlags comboFlags = 0);
void draw_prim_specifier(const SdfPrimSpecHandle &primSpec, ImGuiComboFlags comboFlags = 0);
void draw_prim_instanceable(const SdfPrimSpecHandle &primSpec);
void draw_prim_hidden(const SdfPrimSpecHandle &primSpec);
void draw_prim_active(const SdfPrimSpecHandle &primSpec);
void draw_prim_name(const SdfPrimSpecHandle &primSpec);
//void DrawPrimDocumentation(const SdfPrimSpecHandle &primSpec);
//void DrawPrimComment(const SdfPrimSpecHandle &primSpec);
void draw_sdf_prim_editor_menu_bar(const SdfPrimSpecHandle &primSpec);
void draw_prim_create_composition_menu(const SdfPrimSpecHandle &primSpec);

}// namespace vox