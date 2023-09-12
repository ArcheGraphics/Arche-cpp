//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "commands/commands.h"
#include "vt_dictionary_editor.h"
#include "vt_value_editor.h"
#include "base/constants.h"
#include <string>
#include <imgui.h>
#include <imgui_stdlib.h>
#include <pxr/base/vt/dictionary.h>

#include "base/imgui_helpers.h"
#include "modal_dialogs.h"
#include "table_layouts.h"
#include "vt_value_editor.h"

namespace vox {
static VtValue draw_editable_key_name(const std::string &keyNameSrc, const std::string &typeName, int depth, bool &unfolded) {
    constexpr float indentSize = 10;// TODO move in Constants ??
    VtValue returnValue;
    ImGui::PushID(keyNameSrc.c_str());
    ImGuiContext &g = *GImGui;
    ImGuiWindow *window = g.CurrentWindow;
    ImVec2 cursor = ImGui::GetCursorPos();

    std::string keyName = keyNameSrc + " (" + typeName + ")";

    if (depth) {// The root element is treated differently, it doesn't have a fold
        ImGui::Indent((depth)*indentSize);
        ImGuiTreeNodeFlags nodeFlags = typeName == "dict" ? ImGuiTreeNodeFlags_OpenOnArrow : ImGuiTreeNodeFlags_Leaf;
        nodeFlags |= ImGuiTreeNodeFlags_AllowItemOverlap;// For adding an inputText editor on top
        nodeFlags |= ImGuiTreeNodeFlags_NoTreePushOnOpen;// We handle the indentation ourselves
        unfolded = ImGui::TreeNodeEx(keyName.c_str(), nodeFlags, "%s", keyName.c_str());
        ImGui::Unindent((depth)*indentSize);
    } else {
        ImGui::Text("%s", keyNameSrc.c_str());
        unfolded = true;
    }

    static std::string editKeyName;
    static ImGuiID editingKey = 0;// Assuming there will be no collisions with 0
    if (!editingKey && ImGui::IsItemClicked() && ImGui::IsMouseDoubleClicked(0)) {
        editKeyName = keyNameSrc;
        editingKey = window->GetID(keyName.c_str());
    }

    if (editingKey == window->GetID(keyName.c_str())) {
        ImGui::SetCursorPos(cursor);
        ImGui::PushItemWidth(-FLT_MIN);
        ImGui::InputText("##Keyname", &editKeyName);
        if (ImGui::IsItemDeactivatedAfterEdit() || !ImGui::IsItemFocused()) {
            if (editKeyName != keyNameSrc) {
                returnValue = editKeyName;
            }
            editingKey = 0;
        }
    }
    ImGui::PopID();

    return returnValue;
}

static void create_new_entry(VtDictionary &dict) {
    int postfix = 1;
    VtDictionary::iterator found;
    std::string keyName;
    do {
        keyName = std::string("New Item " + std::to_string(postfix++));
        found = dict.find(keyName);
    } while (found != dict.end());

    dict[keyName] = VtValue();
}

template<>
inline bool has_edits<VtDictionary>(const VtValue &dict) { return dict != VtValue(); }

// TODO : show the types like "token" instead of TfToken
VtValue draw_dictionary_rows(const VtValue &dictValue, const std::string &dictName, int &rowId, int depth) {

    const VtDictionary &dictSource = dictValue.IsHolding<VtDictionary>() ? dictValue.Get<VtDictionary>() : VtDictionary();

    ImGui::PushID(dictName.c_str());

    std::unique_ptr<VtDictionary> modifiedDict;
    std::string modifiedDictName;
    bool clearItem = false;

    ScopedStyleColor style = get_row_style<VtDictionary>(0, dictValue);

    ImGui::TableNextRow(ImGuiTableRowFlags_None, TableRowDefaultHeight);
    ImGui::TableSetColumnIndex(0);
    ImGui::Button(ICON_FA_BOOK_OPEN);
    {
        ScopedStyleColor menuStyle(ImGuiCol_Text, ImVec4(ColorAttributeAuthored), ImGuiCol_Button,
                                   ImVec4(ColorAttributeAuthored));
        if (ImGui::BeginPopupContextItem(nullptr, ImGuiPopupFlags_MouseButtonLeft)) {
            if (ImGui::MenuItem(ICON_FA_PLUS " Add entry")) {
                modifiedDict.reset(new VtDictionary(dictSource));
                create_new_entry(*modifiedDict);
            }
            if (ImGui::MenuItem(ICON_FA_TRASH " Clear")) {
                clearItem = true;
            }
            ImGui::EndPopup();
        }
    }

    ImGui::TableSetColumnIndex(1);
    bool unfolded = false;
    VtValue editedDictName = draw_editable_key_name(dictName, "dict", depth, unfolded);
    if (editedDictName != VtValue()) {
        modifiedDictName = editedDictName.Get<std::string>();
    }
    if (unfolded) {
        for (auto &item : dictSource) {
            // Is it a dictionary ?
            // TODO test with list/vect of vtvalue or dict
            if (item.second.IsHolding<VtDictionary>()) {
                VtValue dictResult = draw_dictionary_rows(item.second, item.first, rowId, depth + 1);
                if (dictResult.IsHolding<VtDictionary>()) {
                    modifiedDict.reset(new VtDictionary(dictSource));
                    (*modifiedDict)[item.first] = dictResult.Get<VtDictionary>();
                } else if (dictResult.IsHolding<std::string>()) {
                    // name is modified, change the key, keep the values
                    modifiedDict.reset(new VtDictionary(dictSource));
                    (*modifiedDict)[dictResult.Get<std::string>()] = item.second;
                    modifiedDict->erase(item.first);
                } else if (dictResult.IsHolding<bool>()) {
                    modifiedDict.reset(new VtDictionary(dictSource));
                    modifiedDict->erase(item.first);
                }
            } else {
                // TODO: drawTableRow here ?
                ImGui::TableNextRow(ImGuiTableRowFlags_None, TableRowDefaultHeight);
                ImGui::TableSetColumnIndex(0);

                ImGui::PushID(item.first.c_str());// FOR THE BUTTON
                ImGui::Button(ICON_FA_EDIT);
                {
                    ScopedStyleColor menuStyle(ImGuiCol_Text, ImVec4(ColorAttributeAuthored), ImGuiCol_Button,
                                               ImVec4(ColorAttributeAuthored));
                    if (ImGui::BeginPopupContextItem(nullptr, ImGuiPopupFlags_MouseButtonLeft)) {
                        if (ImGui::BeginMenu(ICON_FA_USER_FRIENDS " Change type")) {
                            if (ImGui::Selectable("dict")) {
                                modifiedDict.reset(new VtDictionary(dictSource));
                                (*modifiedDict)[item.first] = VtDictionary();
                            }
                            for (int i = 0; i < get_all_value_type_names().size(); i++) {
                                if (ImGui::Selectable(get_all_value_type_names()[i].GetAsToken().GetString().c_str(), false)) {
                                    modifiedDict.reset(new VtDictionary(dictSource));
                                    (*modifiedDict)[item.first] = get_all_value_type_names()[i].GetDefaultValue();
                                }
                            }
                            ImGui::EndMenu();
                        }
                        if (ImGui::MenuItem(ICON_FA_TRASH " Clear")) {
                            modifiedDict.reset(new VtDictionary(dictSource));
                            modifiedDict->erase(item.first);
                        }
                        ImGui::EndPopup();
                    }
                }
                ImGui::PopID();

                ImGui::TableSetColumnIndex(1);
                bool valueUnfolded = false;
                VtValue editedKeyName = draw_editable_key_name(item.first, item.second.GetTypeName(), depth + 1, valueUnfolded);
                if (editedKeyName != VtValue()) {
                    modifiedDict.reset(new VtDictionary(dictSource));
                    modifiedDict->insert({editedKeyName.Get<std::string>(), item.second});
                    modifiedDict->erase(item.first);
                }

                ImGui::TableSetColumnIndex(2);
                ImGui::PushItemWidth(-FLT_MIN);
                VtValue result = draw_vt_value(item.first, item.second);
                if (result != VtValue()) {
                    modifiedDict.reset(new VtDictionary(dictSource));
                    (*modifiedDict)[item.first] = result;
                }
            }
        }
    }
    ImGui::PopID();
    // We return either a modified dict or a modified dict name or a bool -> clear item
    if (modifiedDict) {
        return VtValue(*modifiedDict);
    } else if (!modifiedDictName.empty()) {
        return VtValue(modifiedDictName);
    } else if (clearItem) {
        return VtValue(clearItem);
    } else
        return {};
}

}// namespace vox
