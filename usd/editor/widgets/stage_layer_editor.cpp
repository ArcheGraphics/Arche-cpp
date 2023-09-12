//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "stage_layer_editor.h"
#include "sdf_layer_editor.h"
#include "base/imgui_helpers.h"
#include "table_layouts.h"
#include "commands/commands.h"
#include "file_browser.h"
#include "modal_dialogs.h"
#include "base/constants.h"

namespace vox {
static void draw_sublayer_tree_node_popup_menu(const SdfLayerRefPtr &layer, const SdfLayerRefPtr &parent, const std::string &layerPath,
                                               const UsdStageRefPtr &stage) {
    if (ImGui::BeginPopupContextItem()) {
        // Creating anonymous layers means that we need a structure to hold the layers alive as only the path is stored
        // Testing with a static showed that using the identifier as the path works, but it also means more data handling,
        // making sure that the layers are saved before closing, having visual clew for anonymous layers, etc.
        // if (layer && ImGui::MenuItem("Add anonymous sublayer")) {
        //    static auto sublayer = SdfLayer::CreateAnonymous("NewLayer");
        //    ExecuteAfterDraw(&SdfLayer::InsertSubLayerPath, layer, sublayer->GetIdentifier(), 0);
        //}
        if (layer && ImGui::MenuItem("Add sublayer")) {
            draw_sublayer_path_edit_dialog(layer, "");
        }
        if (parent && !layerPath.empty()) {
            if (ImGui::MenuItem("Remove sublayer")) {
                execute_after_draw<LayerRemoveSubLayer>(parent, layerPath);
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Move up")) {
                execute_after_draw<LayerMoveSubLayer>(parent, layerPath, true);
            }
            if (ImGui::MenuItem("Move down")) {
                execute_after_draw<LayerMoveSubLayer>(parent, layerPath, false);
            }
            ImGui::Separator();
        }
        if (layer) {
            if (ImGui::MenuItem("Set edit target")) {
                execute_after_draw<EditorSetEditTarget>(stage, UsdEditTarget(layer));
            }
            ImGui::Separator();
            draw_layer_action_popup_menu(layer);
        }
        ImGui::EndPopup();
    }
}

static void draw_layer_sublayer_tree_node_buttons(const SdfLayerRefPtr &layer, const SdfLayerRefPtr &parent,
                                                  const std::string &layerPath, const UsdStageRefPtr &stage, int nodeID) {
    if (!layer)
        return;

    ScopedStyleColor transparentButtons(ImGuiCol_Button, ImVec4(ColorTransparent));
    ImGui::BeginDisabled(!layer || !layer->IsDirty() || layer->IsAnonymous());
    if (ImGui::Button(ICON_FA_SAVE)) {
        //ExecuteAfterDraw<LayerRemoveSubLayer>(parent, layerPath);
    }
    ImGui::EndDisabled();
    ImGui::SameLine();
    ImGui::BeginDisabled(!parent);
    if (ImGui::Button(ICON_FA_TRASH)) {
        execute_after_draw<LayerRemoveSubLayer>(parent, layerPath);
    }
    ImGui::EndDisabled();

    ImGui::SameLine();

    if (layer->IsMuted() && ImGui::Button(ICON_FA_VOLUME_MUTE)) {
        execute_after_draw<LayerUnmute>(layer);
    }

    if (!layer->IsMuted() && ImGui::Button(ICON_FA_VOLUME_OFF)) {
        execute_after_draw<LayerMute>(layer);
    }

    ImGui::SameLine();

    ImGui::BeginDisabled(!parent || nodeID == 0);

    if (ImGui::Button(ICON_FA_ARROW_UP)) {
        execute_after_draw<LayerMoveSubLayer>(parent, layerPath, true);
    }
    ImGui::EndDisabled();
    ImGui::SameLine();
    ImGui::BeginDisabled(!parent || nodeID == parent->GetNumSubLayerPaths() - 1);
    if (ImGui::Button(ICON_FA_ARROW_DOWN)) {
        execute_after_draw<LayerMoveSubLayer>(parent, layerPath, false);
    }
    ImGui::EndDisabled();
}

static void draw_layer_sublayer_tree(const SdfLayerRefPtr &layer, const SdfLayerRefPtr &parent, const std::string &layerPath, const UsdStageRefPtr &stage,
                                     int nodeID = 0) {
    // Note: layer can be null if it wasn't found
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);

    ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth;
    if (!layer || !layer->GetNumSubLayerPaths()) {
        treeNodeFlags |= ImGuiTreeNodeFlags_Leaf;
    }
    ImGui::PushID(nodeID);

    bool unfolded = false;
    std::string label =
        layer ? (layer->IsAnonymous() ? std::string(ICON_FA_ATOM " ") : std::string(ICON_FA_FILE " ")) + layer->GetDisplayName() : "Not found " + layerPath;
    {
        ScopedStyleColor color(ImGuiCol_Text,
                               layer ? (layer->IsMuted() ? ImVec4(0.5, 0.5, 0.5, 1.0) : ImGui::GetStyleColorVec4(ImGuiCol_Text)) : ImVec4(1.0, 0.2, 0.2, 1.0));
        unfolded = ImGui::TreeNodeEx(label.c_str(), treeNodeFlags);
    }
    draw_sublayer_tree_node_popup_menu(layer, parent, layerPath, stage);

    ImGui::TableSetColumnIndex(1);
    draw_layer_sublayer_tree_node_buttons(layer, parent, layerPath, stage, nodeID);

    if (unfolded) {
        if (layer) {
            std::vector<std::string> subLayers = layer->GetSubLayerPaths();
            for (int layerId = 0; layerId < subLayers.size(); ++layerId) {
                const std::string &subLayerPath = subLayers[layerId];
                auto subLayer = SdfLayer::FindOrOpenRelativeToLayer(layer, subLayerPath);
                if (!subLayer) {// Try for anonymous layers
                    subLayer = SdfLayer::FindOrOpen(subLayerPath);
                }
                draw_layer_sublayer_tree(subLayer, layer, subLayerPath, stage, layerId);
            }
        }
        ImGui::TreePop();
    }
    ImGui::PopID();
}

struct SublayerRow {
    static constexpr const char *fieldName = "";
};

template<>
inline void draw_third_column<SublayerRow>(const int rowId, const UsdStageRefPtr &stage, const SdfLayerHandle &layer) {
    ImGui::PushID(rowId);
    if (ImGui::Selectable(layer->GetIdentifier().c_str())) {
        execute_after_draw(&UsdStage::SetEditTarget, stage, UsdEditTarget(layer));
    }
    ImGui::PopID();
}

void draw_stage_layer_editor(const UsdStageRefPtr &stage) {
    if (!stage)
        return;

    constexpr ImGuiTableFlags tableFlags = ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit;
    if (ImGui::BeginTable("##DrawLayerSublayers", 2, tableFlags)) {
        ImGui::TableSetupColumn("Stage sublayers", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 28 * 5);
        ImGui::TableHeadersRow();
        ImGui::PushID(0);
        draw_layer_sublayer_tree(stage->GetSessionLayer(), SdfLayerRefPtr(), std::string(), stage, 0);
        ImGui::PopID();
        ImGui::PushID(1);
        draw_layer_sublayer_tree(stage->GetRootLayer(), SdfLayerRefPtr(), std::string(), stage, 0);
        ImGui::PopID();
        ImGui::EndTable();
    }
}

}// namespace vox