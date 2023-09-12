//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <array>
#include <memory>
#include <regex>
#include <iterator>
#include <pxr/usd/usd/stage.h>
#include "base/imgui_helpers.h"
#include "content_browser.h"
#include "sdf_layer_editor.h"// for DrawLayerMenuItems
#include "commands/commands.h"
#include "text_filter.h"
#include "modal_dialogs.h"
#include "file_browser.h"
#include "base/constants.h"
#include "editor.h"

PXR_NAMESPACE_USING_DIRECTIVE

struct ContentBrowserOptions {
    bool _filterAnonymous = false;
    bool _filterFiles = true;
    bool _filterUnmodified = true;
    bool _filterModified = true;
    bool _filterStage = true;
    bool _filterLayer = true;
    bool _showAssetName = false;
    bool _showIdentifier = true;
    bool _showDisplayName = false;
    bool _showRealPath = false;
};

namespace std {
template<>
struct hash<ContentBrowserOptions> {
    inline size_t operator()(const ContentBrowserOptions &options) const {
        size_t value = 0;
        value |= options._filterAnonymous << 0;
        value |= options._filterFiles << 1;
        value |= options._filterUnmodified << 2;
        value |= options._filterModified << 3;
        value |= options._filterStage << 4;
        value |= options._filterLayer << 5;
        value |= options._showAssetName << 6;
        value |= options._showIdentifier << 7;
        value |= options._showDisplayName << 8;
        value |= options._showRealPath << 9;
        return value;
    }
};
}// namespace std

namespace vox {
struct SessionLoadModalDialog : public ModalDialog {
    SessionLoadModalDialog(const UsdStageRefPtr &stage) : _stage(stage){};
    ~SessionLoadModalDialog() override {}

    void draw() override {
        draw_file_browser();
        auto filePath = get_file_browser_file_path();
        ImGui::Text("%s", filePath.c_str());
        draw_ok_cancel_modal([&]() {// On Ok ->
            if (!filePath.empty()) {
                SdfLayerRefPtr sessionLayer = _stage->GetSessionLayer();
                std::function<void()> importSession = [=]() {
                    sessionLayer->Import(filePath);
                };
                execute_after_draw<UsdFunctionCall>(sessionLayer, importSession);
            }
        });
    }

    const char *dialog_id() const override { return "Load session"; }

    UsdStageRefPtr _stage;
};

void draw_layer_tooltip(SdfLayerHandle layer) {
    ImGui::SetTooltip("%s\n%s", layer->GetRealPath().c_str(), layer->GetIdentifier().c_str());
    auto assetInfo = layer->GetAssetInfo();
    if (!assetInfo.IsEmpty()) {
        if (assetInfo.CanCast<VtDictionary>()) {
            auto assetInfoDict = assetInfo.Get<VtDictionary>();
            TF_FOR_ALL(keyValue, assetInfoDict) {
                std::stringstream ss;
                ss << keyValue->second;
                ImGui::SetTooltip("%s %s", keyValue->first.c_str(), ss.str().c_str());
            }
        }
    }
}

static bool pass_options_filter(SdfLayerHandle layer, const ContentBrowserOptions &options, bool isStage) {
    if (!options._filterAnonymous) {
        if (layer->IsAnonymous())
            return false;
    }
    if (!options._filterFiles) {
        if (!layer->IsAnonymous())
            return false;
    }
    if (!options._filterModified) {
        if (layer->IsDirty())
            return false;
    }
    if (!options._filterUnmodified) {
        if (!layer->IsDirty())
            return false;
    }
    if (!options._filterStage && isStage) {
        return false;
    }
    if (!options._filterLayer && !isStage) {
        return false;
    }
    return true;
}

static const std::string &layer_name_from_options(const SdfLayerHandle &layer, const ContentBrowserOptions &options) {
    // GetDisplayName proved to be really slow when the number of layers is high.
    // We maintain a map to store the displayName and avoid allocating and releasing memory
    static std::unordered_map<void const *, std::string> displayNames;
    if (options._showAssetName) {
        return layer->GetAssetName();
    } else if (options._showDisplayName) {
        auto displayNameIt = displayNames.find(layer->GetUniqueIdentifier());
        if (displayNameIt == displayNames.end()) {
            displayNames[layer->GetUniqueIdentifier()] = layer->GetDisplayName();
        }
        return displayNames[layer->GetUniqueIdentifier()];
    } else if (options._showRealPath) {
        return layer->GetRealPath();
    }
    return layer->GetIdentifier();
}

static inline void draw_save_button(SdfLayerHandle layer) {
    ScopedStyleColor style(ImGuiCol_Button, ImVec4(ColorTransparent), ImGuiCol_Text,
                           layer->IsAnonymous() ? ImVec4(ColorTransparent) : (layer->IsDirty() ? ImVec4(1.0, 1.0, 1.0, 1.0) : ImVec4(ColorTransparent)));
    if (ImGui::Button(ICON_FA_SAVE "###Save")) {
        execute_after_draw(&SdfLayer::Save, layer, true);
    }
}

static inline void draw_select_stage_button(SdfLayerHandle layer, bool isStage, SdfLayerHandle *selectedStage) {
    ScopedStyleColor style(ImGuiCol_Button, ImVec4(ColorTransparent), ImGuiCol_Text,
                           isStage ? ((selectedStage && *selectedStage == layer) ? ImVec4(1.0, 1.0, 1.0, 1.0) : ImVec4(0.6, 0.6, 0.6, 1.0)) : ImVec4(ColorTransparent));
    if (ImGui::Button(ICON_FA_DESKTOP "###Stage")) {
        execute_after_draw<EditorSetCurrentStage>(layer);
    }
}

static inline void draw_layer_description_row(SdfLayerHandle layer, bool isStage, const std::string &layerName,
                                              SdfLayerHandle *selectedLayer, SdfLayerHandle *selectedStage) {
    ScopedStyleColor style(ImGuiCol_Text, isStage ? (selectedStage && *selectedStage == layer ? ImVec4(1.0, 1.0, 1.0, 1.0) : ImVec4(1.0, 1.0, 1.0, 1.0)) : ImVec4(0.6, 0.6, 0.6, 1.0));
    bool selected = selectedLayer && *selectedLayer == layer;
    if (ImGui::Selectable(layerName.c_str(), selected)) {
        if (selectedLayer) {
            *selectedLayer = layer;
        }
    }
    if (ImGui::IsItemClicked() && ImGui::IsMouseDoubleClicked(0)) {
        if (!isStage) {
            execute_after_draw<EditorOpenStage>(layer->GetRealPath());
        } else {
            execute_after_draw<EditorSetCurrentStage>(layer);
        }
    }
}

inline size_t compute_layer_set_hash(SdfLayerHandleSet &layerSet) {
    size_t seed = 0;
    for (auto it = layerSet.begin(); it != layerSet.end(); ++it) {
        seed ^= hash_value(*it) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    return seed;
}

void draw_layer_set(UsdStageCache &cache, SdfLayerHandleSet &layerSet, SdfLayerHandle *selectedLayer, SdfLayerHandle *selectedStage,
                    const ContentBrowserOptions &options, const ImVec2 &listSize = ImVec2(0, -10)) {

    static std::vector<SdfLayerHandle> sortedLayerList;
    static std::vector<SdfLayerHandle>::iterator endOfPartition = sortedLayerList.end();
    static size_t pastLayerSetHash = 0;
    static TextFilter filter;
    static size_t pastTextFilterHash;
    static size_t pastOptionFilterHash;
    filter.draw();

    ImGui::PushItemWidth(-1);
    if (ImGui::BeginListBox("##DrawLayerSet", listSize)) {
        // Filter and sort the layer set. This is done only when the layer set or the filter have changed, otherwise it can be
        // really costly to do it at every frame, mainly because of the string creation and deletion.
        // We check if anything has changed using a hash which should be relatively quick to compute.
        size_t currentLayerSetHash = compute_layer_set_hash(layerSet);
        size_t currentTextFilterHash = filter.get_hash();
        size_t currentOptionFilterHash = std::hash<ContentBrowserOptions>()(options);
        if (currentLayerSetHash != pastLayerSetHash || currentTextFilterHash != pastTextFilterHash ||
            currentOptionFilterHash != pastOptionFilterHash) {
            sortedLayerList.assign(layerSet.begin(), layerSet.end());
            endOfPartition = partition(sortedLayerList.begin(), sortedLayerList.end(), [&](const auto &layer) {
                const bool isStage = cache.FindOneMatching(layer);
                return filter.pass_filter(layer_name_from_options(layer, options).c_str()) &&
                       pass_options_filter(layer, options, isStage);
            });

            std::sort(sortedLayerList.begin(), endOfPartition, [&](const auto &t1, const auto &t2) {
                return layer_name_from_options(t1, options) < layer_name_from_options(t2, options);
            });
            pastLayerSetHash = currentLayerSetHash;
            pastTextFilterHash = currentTextFilterHash;
            pastOptionFilterHash = currentOptionFilterHash;
        }
        //
        // Actual drawing of the listed layers using a clipper, we only draw the visible lines
        //
        ImGuiListClipper clipper;
        clipper.Begin(static_cast<int>(std::distance(sortedLayerList.begin(), endOfPartition)));
        while (clipper.Step()) {
            for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++) {
                const auto &layer = sortedLayerList[row];
                const std::string &layerName = layer_name_from_options(layer, options);
                const UsdStageRefPtr isStage = cache.FindOneMatching(layer);
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, ImGui::GetStyle().ItemSpacing.y));
                ImGui::PushID(layer->GetUniqueIdentifier());
                draw_select_stage_button(layer, isStage, selectedStage);
                ImGui::SameLine();
                draw_save_button(layer);
                ImGui::PopStyleVar();
                ImGui::SameLine();
                draw_layer_description_row(layer, isStage, layerName, selectedLayer, selectedStage);

                if (ImGui::IsItemHovered() && GImGui->HoveredIdTimer > 2) {
                    draw_layer_tooltip(layer);
                }
                if (ImGui::BeginPopupContextItem()) {
                    if (isStage) {
                        if (ImGui::MenuItem("Edit session layer")) {
                            execute_after_draw<EditorSetCurrentLayer>(isStage->GetSessionLayer());
                        }
                        if (ImGui::MenuItem("Load session layer")) {
                            draw_modal_dialog<SessionLoadModalDialog>(isStage);
                        }
                        if (ImGui::MenuItem("Save session layer as")) {
                            execute_after_draw<EditorSaveLayerAs>(isStage->GetSessionLayer());
                        }
                    }

                    if (ImGui::MenuItem("Edit layer")) {
                        execute_after_draw<EditorSetCurrentLayer>(layer);
                    }

                    // TODO: split stage and layer ??
                    // Stage could be flatten and exported - or zipped
                    draw_layer_action_popup_menu(layer, isStage);

                    ImGui::EndPopup();
                }
                ImGui::PopID();
            }
        }
        ImGui::EndListBox();
    }
    ImGui::PopItemWidth();
}

void DrawContentBrowserMenuBar(ContentBrowserOptions &options) {
    if (ImGui::BeginMenuBar()) {
        bool selected = true;
        if (ImGui::BeginMenu("Filter")) {
            ImGui::MenuItem("Anonymous", nullptr, &options._filterAnonymous, true);
            ImGui::MenuItem("File", nullptr, &options._filterFiles, true);
            ImGui::Separator();
            ImGui::MenuItem("Unmodified", nullptr, &options._filterUnmodified, true);
            ImGui::MenuItem("Modified", nullptr, &options._filterModified, true);
            ImGui::Separator();
            ImGui::MenuItem("Stage", nullptr, &options._filterStage, true);
            ImGui::MenuItem("Layer", nullptr, &options._filterLayer, true);
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Show")) {
            if (ImGui::MenuItem("Asset name", nullptr, &options._showAssetName, true)) {
                options._showRealPath = options._showIdentifier = options._showDisplayName = false;
            }
            if (ImGui::MenuItem("Identifier", nullptr, &options._showIdentifier, true)) {
                options._showAssetName = options._showRealPath = options._showDisplayName = false;
            }
            if (ImGui::MenuItem("Display name", nullptr, &options._showDisplayName, true)) {
                options._showAssetName = options._showIdentifier = options._showRealPath = false;
            }
            if (ImGui::MenuItem("Real path", nullptr, &options._showRealPath, true)) {
                options._showAssetName = options._showIdentifier = options._showDisplayName = false;
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
}

void DrawContentBrowser(Editor &editor) {
    static ContentBrowserOptions options;
    DrawContentBrowserMenuBar(options);
    // TODO: we might want to remove completely the editor here, just pass as selected layer and a selected stage
    SdfLayerHandle selectedLayer(editor.get_current_layer());
    SdfLayerHandle selectedStage(editor.get_current_stage() ? editor.get_current_stage()->GetRootLayer() : SdfLayerHandle());
    auto layers = SdfLayer::GetLoadedLayers();
    draw_layer_set(editor.get_stage_cache(), layers, &selectedLayer, &selectedStage, options);
    if (selectedLayer != editor.get_current_layer()) {
        execute_after_draw<EditorSetSelection>(selectedLayer, SdfPath::AbsoluteRootPath());
    }
}

}// namespace vox