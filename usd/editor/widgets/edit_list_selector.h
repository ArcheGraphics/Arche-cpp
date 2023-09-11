//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <pxr/usd/sdf/listOp.h>

PXR_NAMESPACE_USING_DIRECTIVE

namespace vox {
constexpr const char *const EditListChoiceID = "EditListChoice";

inline ImGuiStorage *GetStorage() {
    ImGuiContext &g = *GImGui;
    ImGuiWindow *window = g.CurrentWindow;
    return window->DC.StateStorage;
}

template<typename ListEditorT>
inline SdfListOpType GetEditListChoice(const ListEditorT &proxy) {
    const auto key = ImGui::GetID(EditListChoiceID);
    ImGuiStorage *storage = GetStorage();
    int opList = storage->GetInt(key, -1);
    if (opList == -1) {// select the non empty list or explicit
        opList = 0;    // explicit
        for (int i = 0; i < get_list_editor_operation_size(); i++) {
            if (!GetSdfListOpItems(proxy, i).empty()) {
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
inline ScopedStyleColor GetItemColor(SdfListOpType selectedList, const EditListT &arcList) {
    return ScopedStyleColor(ImGuiCol_Text, GetSdfListOpItems(arcList, selectedList).empty() ? ImVec4(ColorAttributeUnauthored) : ImVec4(ColorAttributeAuthored));
}

template<typename EditListT>
inline void DrawEditListSmallButtonSelector(SdfListOpType &currentSelection, const EditListT &arcList) {
    ImGuiStorage *storage = GetStorage();
    const auto key = ImGui::GetID(EditListChoiceID);
    auto outerScopedColorStyle = GetItemColor(currentSelection, arcList);
    ImGui::SmallButton(get_list_editor_operation_abbreviation(currentSelection));
    if (ImGui::BeginPopupContextItem(nullptr, ImGuiPopupFlags_MouseButtonLeft)) {
        for (int i = 0; i < get_list_editor_operation_size(); ++i) {
            // Changing the color to show the empty lists
            auto innerScopedColorStyle = GetItemColor(static_cast<SdfListOpType>(i), arcList);
            if (ImGui::MenuItem(get_list_editor_operation_name(i))) {
                currentSelection = static_cast<SdfListOpType>(i);
                storage->SetInt(key, i);
            }
        }
        ImGui::EndPopup();
    }
}

template<typename EditListT>
inline void DrawEditListComboSelector(SdfListOpType &currentSelection, const EditListT &arcList) {
    ImGuiStorage *storage = GetStorage();
    const auto key = ImGui::GetID(EditListChoiceID);
    const auto outerScopedColorStyle = GetItemColor(currentSelection, arcList);
    if (ImGui::BeginCombo("##Edit list", get_list_editor_operation_name(currentSelection))) {
        for (int i = 0; i < get_list_editor_operation_size(); ++i) {
            const auto innerScopedColorStyle = GetItemColor(static_cast<SdfListOpType>(i), arcList);
            if (ImGui::Selectable(get_list_editor_operation_name(i))) {
                currentSelection = static_cast<SdfListOpType>(i);
                storage->SetInt(key, i);
            }
        }
        ImGui::EndCombo();
    }
}

}// namespace vox