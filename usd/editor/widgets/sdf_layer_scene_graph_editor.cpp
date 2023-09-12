//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <array>
#include <cctype>
#include <iostream>
#include <sstream>
#include <stack>

#include <pxr/usd/sdf/fileFormat.h>
#include <pxr/usd/sdf/layer.h>
#include <pxr/usd/sdf/layerUtils.h>
#include <pxr/usd/sdf/primSpec.h>
#include <pxr/usd/sdf/schema.h>
#include <pxr/usd/sdf/types.h>
#include <pxr/usd/sdf/variantSetSpec.h>
#include <pxr/usd/sdf/variantSpec.h>
#include <pxr/usd/usd/prim.h>
#include <pxr/usd/usd/schemaRegistry.h>
#include <pxr/usd/usdGeom/camera.h>
#include <pxr/usd/usdGeom/gprim.h>

#include "commands/commands.h"
#include "composition_editor.h"
#include "editor.h"
#include "file_browser.h"
#include "base/imgui_helpers.h"
#include "sdf_layer_scene_graph_editor.h"
#include "modal_dialogs.h"
#include "sdf_layer_editor.h"
#include "sdf_prim_editor.h"
#include "commands/shortcuts.h"
#include "base/usd_helpers.h"
#include "base/constants.h"
#include "blueprints.h"

namespace vox {
//
#define LayerHierarchyEditorSeed 3456823
#define IdOf to_imgui_id<3456823, size_t>

static void draw_blueprint_menus(SdfPrimSpecHandle &primSpec, const std::string &folder) {
    Blueprints &blueprints = Blueprints::get_instance();
    for (const auto &subfolder : blueprints.get_sub_folders(folder)) {
        // TODO should check for name validity
        std::string subFolderName = subfolder.substr(subfolder.find_last_of("/") + 1);
        if (ImGui::BeginMenu(subFolderName.c_str())) {
            draw_blueprint_menus(primSpec, subfolder);
            ImGui::EndMenu();
        }
    }
    for (const auto &item : blueprints.get_items(folder)) {
        if (ImGui::MenuItem(item.first.c_str())) {
            execute_after_draw<PrimAddBlueprint>(primSpec, find_next_available_token_string(primSpec->GetName()), item.second);
        }
    }
}

void draw_tree_node_popup(SdfPrimSpecHandle &primSpec) {
    if (!primSpec)
        return;

    if (ImGui::MenuItem("Add child")) {
        execute_after_draw<PrimNew>(primSpec, find_next_available_token_string(SdfPrimSpecDefaultName));
    }
    auto parent = primSpec->GetNameParent();
    if (parent) {
        if (ImGui::MenuItem("Add sibling")) {
            execute_after_draw<PrimNew>(parent, find_next_available_token_string(primSpec->GetName()));
        }
    }
    if (ImGui::BeginMenu("Add blueprint")) {
        draw_blueprint_menus(primSpec, "");
        ImGui::EndMenu();
    }
    if (ImGui::MenuItem("Duplicate")) {
        execute_after_draw<PrimDuplicate>(primSpec, find_next_available_token_string(primSpec->GetName()));
    }
    if (ImGui::MenuItem("Remove")) {
        execute_after_draw<PrimRemove>(primSpec);
    }
    ImGui::Separator();
    if (ImGui::MenuItem("Copy")) {
        execute_after_draw<PrimCopy>(primSpec);
    }
    if (ImGui::MenuItem("Paste")) {
        execute_after_draw<PrimPaste>(primSpec);
    }
    ImGui::Separator();
    if (ImGui::BeginMenu("Create composition")) {
        draw_prim_create_composition_menu(primSpec);
        ImGui::EndMenu();
    }
    ImGui::Separator();
    if (ImGui::MenuItem("Copy prim path")) {
        ImGui::SetClipboardText(primSpec->GetPath().GetString().c_str());
    }
}

static void draw_background_selection(const SdfPrimSpecHandle &currentPrim, const Selection &selection, bool selected) {

    ImVec4 colorSelected = selected ? ImVec4(ColorPrimSelectedBg) : ImVec4(0.75, 0.60, 0.33, 0.2);
    ScopedStyleColor scopedStyle(ImGuiCol_HeaderHovered, selected ? colorSelected : ImVec4(ColorTransparent),
                                 ImGuiCol_HeaderActive, ImVec4(ColorTransparent), ImGuiCol_Header, colorSelected);
    ImVec2 sizeArg(0.0, TableRowDefaultHeight);
    const auto selectableFlags = ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowItemOverlap;
    if (ImGui::Selectable("##backgroundSelectedPrim", selected, selectableFlags, sizeArg)) {
        if (currentPrim) {
            execute_after_draw<EditorSetSelection>(currentPrim->GetLayer(), currentPrim->GetPath());
        }
    }
    ImGui::SetItemAllowOverlap();
    ImGui::SameLine();
}

inline void draw_tooltip(const char *text) {
    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(text);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

void draw_mini_toolbar(SdfLayerRefPtr layer, const SdfPrimSpecHandle &prim) {
    if (ImGui::Button(ICON_FA_PLUS)) {
        if (prim == SdfPrimSpecHandle()) {
            execute_after_draw<PrimNew>(layer, find_next_available_token_string(SdfPrimSpecDefaultName));
        } else {
            execute_after_draw<PrimNew>(prim, find_next_available_token_string(SdfPrimSpecDefaultName));
        }
    }
    draw_tooltip("New child prim");
    ImGui::SameLine();
    if (ImGui::Button(ICON_FA_PLUS_SQUARE) && prim) {
        auto parent = prim->GetNameParent();
        if (parent) {
            execute_after_draw<PrimNew>(parent, find_next_available_token_string(prim->GetName()));
        } else {
            execute_after_draw<PrimNew>(layer, find_next_available_token_string(prim->GetName()));
        }
    }
    draw_tooltip("New sibbling prim");
    ImGui::SameLine();
    if (ImGui::Button(ICON_FA_CLONE) && prim) {
        execute_after_draw<PrimDuplicate>(prim, find_next_available_token_string(prim->GetName()));
    }
    draw_tooltip("Duplicate");
    ImGui::SameLine();
    if (ImGui::Button(ICON_FA_ARROW_UP) && prim) {
        execute_after_draw<PrimReorder>(prim, true);
    }
    draw_tooltip("Move up");
    ImGui::SameLine();
    if (ImGui::Button(ICON_FA_ARROW_DOWN) && prim) {
        execute_after_draw<PrimReorder>(prim, false);
    }
    draw_tooltip("Move down");
    ImGui::SameLine();
    if (ImGui::Button(ICON_FA_TRASH) && prim) {
        execute_after_draw<PrimRemove>(prim);
    }
    draw_tooltip("Remove");
    ImGui::SameLine();
    if (ImGui::Button(ICON_FA_COPY) && prim) {
        execute_after_draw<PrimCopy>(prim);
    }
    draw_tooltip("Copy");
    ImGui::SameLine();
    if (ImGui::Button(ICON_FA_PASTE) && prim) {
        execute_after_draw<PrimPaste>(prim);
    }
    draw_tooltip("Paste");
}

static void handle_drag_and_drop(const SdfPrimSpecHandle &primSpec, const Selection &selection) {
    static SdfPathVector payload;
    // Drag and drop
    ImGuiDragDropFlags srcFlags = 0;
    srcFlags |= ImGuiDragDropFlags_SourceNoDisableHover;    // Keep the source displayed as hovered
    srcFlags |= ImGuiDragDropFlags_SourceNoHoldToOpenOthers;// Because our dragging is local, we disable the feature of opening
                                                            // foreign treenodes/tabs while dragging
    // src_flags |= ImGuiDragDropFlags_SourceNoPreviewTooltip; // Hide the tooltip
    if (ImGui::BeginDragDropSource(srcFlags)) {
        payload.clear();
        if (selection.is_selected(primSpec)) {
            for (const auto &selectedPath : selection.get_selected_paths(primSpec->GetLayer())) {
                payload.push_back(selectedPath);
            }
        } else {
            payload.push_back(primSpec->GetPath());
        }
        if (!(srcFlags & ImGuiDragDropFlags_SourceNoPreviewTooltip)) {
            ImGui::Text("Moving %s", primSpec->GetPath().GetString().c_str());
        }
        ImGui::SetDragDropPayload("DND", &payload, sizeof(SdfPathVector), ImGuiCond_Once);
        ImGui::EndDragDropSource();
    }

    if (ImGui::BeginDragDropTarget()) {
        ImGuiDragDropFlags targetFlags = 0;
        // target_flags |= ImGuiDragDropFlags_AcceptBeforeDelivery;    // Don't wait until the delivery (release mouse button on a
        // target) to do something target_flags |= ImGuiDragDropFlags_AcceptNoDrawDefaultRect; // Don't display the yellow
        // rectangle
        if (const ImGuiPayload *pl = ImGui::AcceptDragDropPayload("DND", targetFlags)) {
            SdfPathVector source(*(SdfPathVector *)pl->Data);
            execute_after_draw<PrimReparent>(primSpec->GetLayer(), source, primSpec->GetPath());
        }
        ImGui::EndDragDropTarget();
    }
}

static void handle_drag_and_drop(SdfLayerHandle layer, const Selection &selection) {
    static SdfPathVector payload;
    // Drop on the layer
    if (ImGui::BeginDragDropTarget()) {
        ImGuiDragDropFlags targetFlags = 0;
        if (const ImGuiPayload *pl = ImGui::AcceptDragDropPayload("DND", targetFlags)) {
            SdfPathVector source(*(SdfPathVector *)pl->Data);
            execute_after_draw<PrimReparent>(layer, source, SdfPath::AbsoluteRootPath());
        }
        ImGui::EndDragDropTarget();
    }
}

// Returns unfolded
static bool draw_tree_node_prim_name(const bool &primIsVariant, SdfPrimSpecHandle &primSpec, const Selection &selection, bool hasChildren) {
    // Format text differently when the prim is a variant
    std::string primSpecName;
    if (primIsVariant) {
        auto variantSelection = primSpec->GetPath().GetVariantSelection();
        primSpecName = std::string("{") + variantSelection.first.c_str() + ":" + variantSelection.second.c_str() + "}";
    } else {
        primSpecName = primSpec->GetPath().GetName();
    }
    ScopedStyleColor textColor(ImGuiCol_Text,
                               primIsVariant ? ImU32(ImColor::HSV(0.2 / 7.0f, 0.5f, 0.8f)) : ImGui::GetColorU32(ImGuiCol_Text),
                               ImGuiCol_HeaderHovered, 0, ImGuiCol_HeaderActive, 0);

    ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_AllowItemOverlap;
    nodeFlags |= hasChildren && !primSpec->HasVariantSetNames() ? ImGuiTreeNodeFlags_Leaf : ImGuiTreeNodeFlags_None;// ImGuiTreeNodeFlags_DefaultOpen;
    auto cursor = ImGui::GetCursorPos();                                                                            // Store position for the InputText to edit the prim name
    auto unfolded = ImGui::TreeNodeBehavior(IdOf(primSpec->GetPath().GetHash()), nodeFlags, primSpecName.c_str());

    // Edition of the prim name
    static SdfPrimSpecHandle editNamePrim;
    if (!ImGui::IsItemToggledOpen() && ImGui::IsItemClicked()) {
        execute_after_draw<EditorSetSelection>(primSpec->GetLayer(), primSpec->GetPath());
        if (editNamePrim != SdfPrimSpecHandle() && editNamePrim != primSpec) {
            editNamePrim = SdfPrimSpecHandle();
        }
        if (!primIsVariant && ImGui::IsMouseDoubleClicked(0)) {
            editNamePrim = primSpec;
        }
    }
    if (primSpec == editNamePrim) {
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.0, 0.0, 0.0, 1.0));
        ImGui::SetCursorPos(cursor);
        draw_prim_name(primSpec);// Draw the prim name editor
        if (ImGui::IsItemDeactivatedAfterEdit() || !ImGui::IsItemFocused()) {
            editNamePrim = SdfPrimSpecHandle();
        }
        ImGui::PopStyleColor();
    }

    return unfolded;
}

/// Draw a node in the primspec tree
static void draw_sdf_prim_row(const SdfLayerRefPtr &layer, const SdfPath &primPath, const Selection &selection, int nodeId,
                              float &selectedPosY) {
    SdfPrimSpecHandle primSpec = layer->GetPrimAtPath(primPath);

    if (!primSpec)
        return;

    SdfPrimSpecHandle previousSelectedPrim;

    auto selectedPrim = layer->GetPrimAtPath(selection.get_anchor_prim_path(layer));// TODO should we have a function for that ?
    bool primIsVariant = primPath.IsPrimVariantSelectionPath();

    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);

    ImGui::PushID(nodeId);

    nodeId = 0;// reset the counter
    // Edit buttons
    if (selectedPrim == primSpec) {
        selectedPosY = ImGui::GetCursorPosY();
    }

    draw_background_selection(primSpec, selection, selectedPrim == primSpec);

    // Drag and drop on Selectable
    handle_drag_and_drop(primSpec, selection);

    // Draw the tree column
    auto childrenNames = primSpec->GetNameChildren();

    ImGui::SameLine();
    TreeIndenter<LayerHierarchyEditorSeed, SdfPath> indenter(primPath);
    bool unfolded = draw_tree_node_prim_name(primIsVariant, primSpec, selection, childrenNames.empty());

    // Right click will open the quick edit popup menu
    if (ImGui::BeginPopupContextItem()) {
        draw_mini_toolbar(layer, primSpec);
        ImGui::Separator();
        draw_tree_node_popup(primSpec);
        ImGui::EndPopup();
    }

    // We want transparent combos
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.0, 0.0, 0.0, 0.0));

    // Draw the description column
    ImGui::TableSetColumnIndex(1);
    ImGui::PushItemWidth(-FLT_MIN);// removes the combo label. The col needs to have a fixed size
    draw_prim_specifier(primSpec, ImGuiComboFlags_NoArrowButton);
    ImGui::PopItemWidth();
    ImGui::TableSetColumnIndex(2);
    ImGui::PushItemWidth(-FLT_MIN);// removes the combo label. The col needs to have a fixed size
    draw_prim_type(primSpec, ImGuiComboFlags_NoArrowButton);
    ImGui::PopItemWidth();
    // End of transparent combos
    ImGui::PopStyleColor();

    // Draw composition summary
    ImGui::TableSetColumnIndex(3);
    draw_prim_composition_summary(primSpec);
    ImGui::SetItemAllowOverlap();

    // Draw children
    if (unfolded) {
        ImGui::TreePop();
    }

    ImGui::PopID();
}

static void draw_top_node_layer_row(const SdfLayerRefPtr &layer, const Selection &selection, float &selectedPosY) {
    ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_AllowItemOverlap;
    int nodeId = 0;
    if (layer->GetRootPrims().empty()) {
        treeNodeFlags |= ImGuiTreeNodeFlags_Leaf;
    }
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    auto rootPrim = layer->GetPrimAtPath(SdfPath::AbsoluteRootPath());
    draw_background_selection(rootPrim, selection, selection.is_selected(rootPrim));
    handle_drag_and_drop(layer, selection);
    ImGui::SetItemAllowOverlap();
    std::string label = std::string(ICON_FA_FILE) + " " + layer->GetDisplayName();

    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, 0);
    ImGui::PushStyleColor(ImGuiCol_HeaderActive, 0);
    bool unfolded = ImGui::TreeNodeBehavior(IdOf(SdfPath::AbsoluteRootPath().GetHash()), treeNodeFlags, label.c_str());
    ImGui::PopStyleColor(2);

    if (!ImGui::IsItemToggledOpen() && ImGui::IsItemClicked()) {
        execute_after_draw<EditorSetSelection>(layer, SdfPath::AbsoluteRootPath());
    }

    if (ImGui::BeginPopupContextItem()) {
        draw_mini_toolbar(layer, SdfPrimSpec());
        ImGui::Separator();
        if (ImGui::MenuItem("Add sublayer")) {
            draw_sublayer_path_edit_dialog(layer, "");
        }
        if (ImGui::MenuItem("Add root prim")) {
            execute_after_draw<PrimNew>(layer, find_next_available_token_string(SdfPrimSpecDefaultName));
        }
        const char *clipboard = ImGui::GetClipboardText();
        const bool clipboardEmpty = !clipboard || clipboard[0] == 0;
        if (!clipboardEmpty && ImGui::MenuItem("Paste path as Overs")) {
            execute_after_draw<LayerCreateOversFromPath>(layer, std::string(ImGui::GetClipboardText()));
        }
        if (ImGui::MenuItem("Paste")) {
            execute_after_draw<PrimPaste>(rootPrim);
        }
        ImGui::Separator();
        draw_layer_action_popup_menu(layer);

        ImGui::EndPopup();
    }
    if (unfolded) {
        ImGui::TreePop();
    }
    if (!layer->GetSubLayerPaths().empty()) {
        ImGui::TableSetColumnIndex(3);
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.0, 0.0, 0.0, 0.0));
        ImGui::PushItemWidth(-FLT_MIN);// removes the combo label.
        if (ImGui::BeginCombo("Sublayers", "Sublayers", ImGuiComboFlags_NoArrowButton)) {
            for (const auto &pathIt : layer->GetSubLayerPaths()) {
                const std::string &path = pathIt;
                if (ImGui::MenuItem(path.c_str())) {
                    auto subLayer = SdfLayer::FindOrOpenRelativeToLayer(layer, path);
                    if (subLayer) {
                        execute_after_draw<EditorFindOrOpenLayer>(subLayer->GetRealPath());
                    }
                }
            }
            ImGui::EndCombo();
        }
        ImGui::PopItemWidth();
        ImGui::PopStyleColor();
    }

    if (selectedPosY != -1) {
        ScopedStyleColor highlightButton(ImGuiCol_Button, ImVec4(ColorButtonHighlight));
        ImGui::SetCursorPosX(ImGui::GetWindowContentRegionMax().x - 160);
        ImGui::SetCursorPosY(selectedPosY);
        draw_mini_toolbar(layer, layer->GetPrimAtPath(selection.get_anchor_prim_path(layer)));
    }
}

/// Traverse all the path of the layer and store them in a vector. Apply a filter to only traverse the path
/// that should be displayed, the ones inside the collapsed part of the tree view
void traverse_opened_paths(const SdfLayerRefPtr &layer, std::vector<SdfPath> &paths) {
    paths.clear();
    std::stack<SdfPath> st;
    st.push(SdfPath::AbsoluteRootPath());
    ImGuiContext &g = *GImGui;
    ImGuiWindow *window = g.CurrentWindow;
    ImGuiStorage *storage = window->DC.StateStorage;
    while (!st.empty()) {
        const SdfPath path = st.top();
        st.pop();
        const ImGuiID pathHash = IdOf(path.GetHash());
        const bool isOpen = storage->GetInt(pathHash, 0) != 0;
        if (isOpen) {
            if (layer->HasField(path, SdfChildrenKeys->PrimChildren)) {
                const std::vector<TfToken> &children =
                    layer->GetFieldAs<std::vector<TfToken>>(path, SdfChildrenKeys->PrimChildren);
                for (auto it = children.rbegin(); it != children.rend(); ++it) {
                    st.push(path.AppendChild(*it));
                }
            }
            if (layer->HasField(path, SdfChildrenKeys->VariantSetChildren)) {
                const std::vector<TfToken> &variantSetchildren =
                    layer->GetFieldAs<std::vector<TfToken>>(path, SdfChildrenKeys->VariantSetChildren);
                // Skip the variantSet paths and show only the variantSetChildren
                for (auto vSetIt = variantSetchildren.rbegin(); vSetIt != variantSetchildren.rend(); ++vSetIt) {
                    auto variantSetPath = path.AppendVariantSelection(*vSetIt, "");
                    if (layer->HasField(variantSetPath, SdfChildrenKeys->VariantChildren)) {
                        const std::vector<TfToken> &variantChildren =
                            layer->GetFieldAs<std::vector<TfToken>>(variantSetPath, SdfChildrenKeys->VariantChildren);
                        const std::string &variantSet = variantSetPath.GetVariantSelection().first;
                        for (auto vChildrenIt = variantChildren.rbegin(); vChildrenIt != variantChildren.rend(); ++vChildrenIt) {
                            st.push(path.AppendVariantSelection(TfToken(variantSet), *vChildrenIt));
                        }
                    }
                }
            }
        }
        paths.push_back(path);
    }
}

void draw_layer_prim_hierarchy(SdfLayerRefPtr layer, const Selection &selection) {
    if (!layer)
        return;

    SdfPrimSpecHandle selectedPrim = layer->GetPrimAtPath(selection.get_anchor_prim_path(layer));
    draw_layer_navigation(layer);
    auto flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY;

    if (ImGui::BeginTable("##DrawArrayEditor", 4, flags)) {
        ImGui::TableSetupScrollFreeze(4, 1);
        ImGui::TableSetupColumn("Prim hierarchy");
        ImGui::TableSetupColumn("Spec", ImGuiTableColumnFlags_WidthFixed, 40);
        ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 150);
        ImGui::TableSetupColumn("Composition");

        ImGui::TableHeadersRow();

        std::vector<SdfPath> paths;

        // Find all the opened paths
        paths.reserve(1024);
        traverse_opened_paths(layer, paths);

        int nodeId = 0;
        float selectedPosY = -1;
        const size_t arraySize = paths.size();
        SdfPathVector pathPrefixes;
        ImGuiListClipper clipper;
        clipper.Begin(static_cast<int>(arraySize));
        while (clipper.Step()) {
            for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++) {
                ImGui::PushID(row);
                const SdfPath &path = paths[row];
                if (path.IsAbsoluteRootPath()) {
                    draw_top_node_layer_row(layer, selection, selectedPosY);
                } else {
                    draw_sdf_prim_row(layer, path, selection, row, selectedPosY);
                }
                ImGui::PopID();
            }
        }
        ImGui::EndTable();
    }
    if (ImGui::IsItemHovered() && selectedPrim) {
        add_shortcut<PrimRemove, ImGuiKey_Delete>(selectedPrim);
        add_shortcut<PrimCopy, ImGuiKey_LeftCtrl, ImGuiKey_C>(selectedPrim);
        add_shortcut<PrimPaste, ImGuiKey_LeftCtrl, ImGuiKey_V>(selectedPrim);
        add_shortcut<PrimDuplicate, ImGuiKey_LeftCtrl, ImGuiKey_D>(selectedPrim,
                                                                   find_next_available_token_string(selectedPrim->GetName()));
    }
}

}// namespace vox