//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <pxr/usd/sdf/layer.h>
#include <pxr/usd/sdf/primSpec.h>

PXR_NAMESPACE_USING_DIRECTIVE

namespace vox {
//
void draw_sdf_layer_identity(const SdfLayerRefPtr &layer, const SdfPath &);

//
void draw_sdf_layer_metadata(const SdfLayerRefPtr &layer);

///
void draw_layer_action_popup_menu(const SdfLayerHandle& layer, bool isStage = false);

void draw_layer_navigation(const SdfLayerRefPtr& layer);

void draw_layer_sublayer_stack(const SdfLayerRefPtr& layer);

void draw_sdf_layer_editor_menu_bar(const SdfLayerRefPtr& layer);

void draw_sublayer_path_edit_dialog(const SdfLayerRefPtr &layer, const std::string &path);

}// namespace vox