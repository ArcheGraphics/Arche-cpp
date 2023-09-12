//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <pxr/usd/sdf/listOp.h>
#include "base/constants.h"

PXR_NAMESPACE_USING_DIRECTIVE

namespace vox {
constexpr const char *const EditListChoiceID = "EditListChoice";

inline ImGuiStorage *get_storage() {
    ImGuiContext &g = *GImGui;
    ImGuiWindow *window = g.CurrentWindow;
    return window->DC.StateStorage;
}

template<typename ListEditorT>
inline SdfListOpType get_edit_list_choice(const ListEditorT &proxy) {
    const auto key = ImGui::GetID(EditListChoiceID);
    ImGuiStorage *storage = get_storage();
    int opList = storage->GetInt(key, -1);
    if (opList == -1) {// select the non empty list or explicit
        opList = 0;    // explicit
        for (int i = 0; i < get_list_editor_operation_size(); i++) {
            if (!get_sdf_list_op_items(proxy, i).empty()) {
                opList = i;
                break;
            }
        }
        storage->SetInt(key, opList);
    }
    return static_cast<SdfListOpType>(opList);
}

// Returns a the color style for the list name, depending on if they are empty or not
template<typename EditListT>
inline ScopedStyleColor get_item_color(SdfListOpType selectedList, const EditListT &arcList) {
    return ScopedStyleColor(ImGuiCol_Text, get_sdf_list_op_items(arcList, selectedList).empty() ? ImVec4(ColorAttributeUnauthored) : ImVec4(ColorAttributeAuthored));
}

template<typename EditListT>
inline void draw_edit_list_small_button_selector(SdfListOpType &currentSelection, const EditListT &arcList) {
    ImGuiStorage *storage = get_storage();
    const auto key = ImGui::GetID(EditListChoiceID);
    auto outerScopedColorStyle = get_item_color(currentSelection, arcList);
    ImGui::SmallButton(get_list_editor_operation_abbreviation(currentSelection));
    if (ImGui::BeginPopupContextItem(nullptr, ImGuiPopupFlags_MouseButtonLeft)) {
        for (int i = 0; i < get_list_editor_operation_size(); ++i) {
            // Changing the color to show the empty lists
            auto innerScopedColorStyle = get_item_color(static_cast<SdfListOpType>(i), arcList);
            if (ImGui::MenuItem(get_list_editor_operation_name(i))) {
                currentSelection = static_cast<SdfListOpType>(i);
                storage->SetInt(key, i);
            }
        }
        ImGui::EndPopup();
    }
}

template<typename EditListT>
inline void draw_edit_list_combo_selector(SdfListOpType &currentSelection, const EditListT &arcList) {
    ImGuiStorage *storage = get_storage();
    const auto key = ImGui::GetID(EditListChoiceID);
    const auto outerScopedColorStyle = get_item_color(currentSelection, arcList);
    if (ImGui::BeginCombo("##Edit list", get_list_editor_operation_name(currentSelection))) {
        for (int i = 0; i < get_list_editor_operation_size(); ++i) {
            const auto innerScopedColorStyle = get_item_color(static_cast<SdfListOpType>(i), arcList);
            if (ImGui::Selectable(get_list_editor_operation_name(i))) {
                currentSelection = static_cast<SdfListOpType>(i);
                storage->SetInt(key, i);
            }
        }
        ImGui::EndCombo();
    }
}

}// namespace vox