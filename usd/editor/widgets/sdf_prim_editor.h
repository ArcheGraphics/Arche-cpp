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
void DrawSdfPrimEditor(const SdfPrimSpecHandle &primSpec, const Selection &);

void DrawPrimKind(const SdfPrimSpecHandle &primSpec);
void DrawPrimType(const SdfPrimSpecHandle &primSpec, ImGuiComboFlags comboFlags = 0);
void DrawPrimSpecifier(const SdfPrimSpecHandle &primSpec, ImGuiComboFlags comboFlags = 0);
void DrawPrimInstanceable(const SdfPrimSpecHandle &primSpec);
void DrawPrimHidden(const SdfPrimSpecHandle &primSpec);
void DrawPrimActive(const SdfPrimSpecHandle &primSpec);
void DrawPrimName(const SdfPrimSpecHandle &primSpec);
//void DrawPrimDocumentation(const SdfPrimSpecHandle &primSpec);
//void DrawPrimComment(const SdfPrimSpecHandle &primSpec);
void DrawSdfPrimEditorMenuBar(const SdfPrimSpecHandle &primSpec);
void DrawPrimCreateCompositionMenu(const SdfPrimSpecHandle &primSpec);

}// namespace vox