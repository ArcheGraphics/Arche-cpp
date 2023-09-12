//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <pxr/usd/sdf/layer.h>
#include <pxr/usd/sdf/fileFormat.h>
#include <pxr/usd/sdf/primSpec.h>
#include <pxr/usd/sdf/variantSpec.h>
#include <pxr/usd/sdf/variantSetSpec.h>
#include <pxr/usd/sdf/schema.h>
#include <pxr/usd/usdGeom/gprim.h>
#include <pxr/usd/usdRender/settings.h>
#include <imgui.h>
#include <imgui_stdlib.h>
#include "editor.h"
#include "commands/commands.h"
#include "modal_dialogs.h"
#include "file_browser.h"
#include "sdf_layer_editor.h"
#include "base/imgui_helpers.h"
#include "vt_value_editor.h"
#include "table_layouts.h"
#include "vt_dictionary_editor.h"
#include "base/usd_helpers.h"
#include "base/constants.h"

#include <filesystem>
#include <utility>
namespace fs = std::filesystem;

namespace vox {
// This is very similar to AddSublayer, they should be merged together
struct EditSublayerPath : public ModalDialog {
    EditSublayerPath(const SdfLayerRefPtr &layer, std::string sublayerPath) : layer(layer), _sublayerPath(std::move(sublayerPath)) {
        if (!layer) return;
        // We have to setup the filebrowser first, setting the directory and filename it should point to
        // 2 cases:
        //    1. path is empty -> we want to create a new layer
        //          -> set the fb directory to layer directory
        //    2. path contains a name -> we want to edit this path using the browser
        //      2.1 name is relative to layer and pointing to an existing layer
        //            -> set relative on and
        //      2.2 name is absolute and pointing to an existing layer
        //            -> use the directory of path
        //      2.3 name does not point to an existing file
        //           -> set the directory to layer
        fs::path layerPath(layer->GetRealPath());
        set_file_browser_directory(layerPath.parent_path().string());
        if (!_sublayerPath.empty()) {
            // Does the file exists ??
            fs::path path(_sublayerPath);
            if (fs::exists(path)) {
                // Path is global
                _relative = false;
                set_file_browser_directory(path.parent_path().string());
                set_file_browser_file_path(path.string());
            } else {
                // Try relative to this layer
                auto relativePath = layerPath.parent_path() / path;
                auto absolutePath = fs::absolute(relativePath);
                if (fs::exists(absolutePath) || fs::exists(absolutePath.parent_path())) {
                    _relative = true;
                    set_file_browser_directory(absolutePath.parent_path().string());
                    set_file_browser_file_path(absolutePath.string());
                }
            }
            set_file_browser_file_path(_sublayerPath);
        }
        // Make sure we only use usd layers in the filebrowser
        set_valid_extensions(get_usd_valid_extensions());
        ensure_file_browser_default_extension("usd");
    };

    void draw() override {
        draw_file_browser();
        auto filePath = get_file_browser_file_path();
        auto insertedFilePath = _relative ? get_file_browser_file_path_relative_to(layer->GetRealPath(), _unixify) : filePath;
        if (insertedFilePath.empty()) insertedFilePath = _sublayerPath;
        const bool filePathExits = file_path_exists();
        const bool relativePathValid = _relative ? !insertedFilePath.empty() : true;
        ImGui::Checkbox("Use relative path", &_relative);
        ImGui::SameLine();
        ImGui::Checkbox("Unix compatible", &_unixify);
        ImGui::BeginDisabled(!relativePathValid || filePathExits);
        ImGui::Checkbox("Create layer", &_createLayer);
        ImGui::EndDisabled();

        if (filePathExits && relativePathValid) {
            ImGui::Text("Import found layer: ");
        } else {
            if (_createLayer) {
                // TODO: do we also want to be able to create new layer ??
                ImGui::Text("Creating and import layer: ");
            } else {
                ImGui::Text("Import unknown layer: ");
            }
        }// ... other messages like permission denied, or incorrect extension
        ImGui::SameLine();
        ImGui::Text("%s", insertedFilePath.c_str());
        draw_ok_cancel_modal([&]() {
            if (!insertedFilePath.empty()) {
                if (!filePathExits && _createLayer) {
                    SdfLayer::CreateNew(filePath);// TODO: in a command
                }
                if (_sublayerPath.empty()) {
                    execute_after_draw(&SdfLayer::InsertSubLayerPath, layer, insertedFilePath, 0);
                } else {
                    execute_after_draw<LayerRenameSubLayer>(layer, _sublayerPath, insertedFilePath);
                }
            }
        });
    }

    [[nodiscard]] const char *dialog_id() const override { return "Edit sublayer path"; }

    SdfLayerRefPtr layer;
    std::string _sublayerPath;
    bool _createLayer = false;
    bool _relative = false;
    bool _unixify = false;
};

void draw_sublayer_path_edit_dialog(const SdfLayerRefPtr &layer, const std::string &path) {
    draw_modal_dialog<EditSublayerPath>(layer, path);
}

void draw_default_prim(const SdfLayerRefPtr &layer) {
    auto defautPrim = layer->GetDefaultPrim();
    if (ImGui::BeginCombo("Default Prim", defautPrim.GetString().c_str())) {
        bool isSelected = defautPrim == "";
        if (ImGui::Selectable("##defaultprim", isSelected)) {
            defautPrim = TfToken("");
            if (isSelected)
                ImGui::SetItemDefaultFocus();
        }
        const auto &rootPrims = layer->GetRootPrims();
        for (const auto &prim : rootPrims) {
            isSelected = (defautPrim == prim->GetNameToken());
            if (ImGui::Selectable(prim->GetName().c_str(), isSelected)) {
                defautPrim = prim->GetNameToken();
            }
            if (isSelected)
                ImGui::SetItemDefaultFocus();
        }

        if (layer->GetDefaultPrim() != defautPrim) {
            if (defautPrim != "") {
                execute_after_draw(&SdfLayer::SetDefaultPrim, layer, defautPrim);
            } else {
                execute_after_draw(&SdfLayer::ClearDefaultPrim, layer);
            }
        }
        ImGui::EndCombo();
    }
}

/// Draw the up axis orientation.
/// It should normally be set by the stage, not the layer, so the code bellow follows what the api is doing
/// inside
void draw_up_axis(const SdfLayerRefPtr &layer) {
    VtValue upAxis = layer->GetField(SdfPath::AbsoluteRootPath(), UsdGeomTokens->upAxis);
    std::string upAxisStr("Default");
    if (!upAxis.IsEmpty()) {
        upAxisStr = upAxis.Get<TfToken>().GetString();
    }

    if (ImGui::BeginCombo("Up Axis", upAxisStr.c_str())) {
        bool selected = !upAxis.IsEmpty() && upAxis.Get<TfToken>() == UsdGeomTokens->z;
        if (ImGui::Selectable("Z", selected)) {
            execute_after_draw(&SdfLayer::SetField<TfToken>, layer, SdfPath::AbsoluteRootPath(), UsdGeomTokens->upAxis,
                               UsdGeomTokens->z);
        }
        selected = !upAxis.IsEmpty() && upAxis.Get<TfToken>() == UsdGeomTokens->y;
        if (ImGui::Selectable("Y", selected)) {
            execute_after_draw(&SdfLayer::SetField<TfToken>, layer, SdfPath::AbsoluteRootPath(), UsdGeomTokens->upAxis,
                               UsdGeomTokens->y);
        }
        ImGui::EndCombo();
    }
}

void draw_layer_meters_per_unit(const SdfLayerRefPtr &layer) {
    VtValue metersPerUnit = layer->HasField(SdfPath::AbsoluteRootPath(), UsdGeomTokens->metersPerUnit) ?
                                layer->GetField(SdfPath::AbsoluteRootPath(), UsdGeomTokens->metersPerUnit) :
                                VtValue(1.0);// Should be the default value
    VtValue result = draw_vt_value("Meters per unit", metersPerUnit);
    if (result != VtValue()) {
        execute_after_draw(&SdfLayer::SetField<VtValue>, layer, SdfPath::AbsoluteRootPath(), UsdGeomTokens->metersPerUnit, result);
    }
}

void draw_render_settings_prim_path(const SdfLayerRefPtr &layer) {
    VtValue renderSettingsPrimPath = layer->HasField(SdfPath::AbsoluteRootPath(), UsdRenderTokens->renderSettingsPrimPath) ?
                                         layer->GetField(SdfPath::AbsoluteRootPath(), UsdRenderTokens->renderSettingsPrimPath) :
                                         VtValue("");
    VtValue result = draw_vt_value("Render Settings Prim Path", renderSettingsPrimPath);
    if (result != VtValue()) {
        execute_after_draw(&SdfLayer::SetField<VtValue>, layer, SdfPath::AbsoluteRootPath(), UsdRenderTokens->renderSettingsPrimPath, result);
    }
}

#define GENERATE_DRAW_FUNCTION(Name_, Type_, Label_)                 \
    void DrawLayer##Name_(const SdfLayerRefPtr &layer) {             \
        auto value = layer->Get##Name_();                            \
        ImGui::Input##Type_(Label_, &value);                         \
        if (ImGui::IsItemDeactivatedAfterEdit()) {                   \
            execute_after_draw(&SdfLayer::Set##Name_, layer, value); \
        }                                                            \
    }

GENERATE_DRAW_FUNCTION(StartTimeCode, Double, "Start Time Code");
GENERATE_DRAW_FUNCTION(EndTimeCode, Double, "End Time Code");
GENERATE_DRAW_FUNCTION(FramesPerSecond, Double, "Frame per second");
GENERATE_DRAW_FUNCTION(FramePrecision, Int, "Frame precision");
GENERATE_DRAW_FUNCTION(TimeCodesPerSecond, Double, "TimeCodes per second");
GENERATE_DRAW_FUNCTION(Documentation, TextMultiline, "##DrawLayerDocumentation");
GENERATE_DRAW_FUNCTION(Comment, TextMultiline, "##DrawLayerComment");

// To avoid repeating the same code, we let the preprocessor do the work
#define GENERATE_METADATA_FIELD(ClassName_, Token_, DrawFun_, FieldName_)                          \
    struct ClassName_ {                                                                            \
        static constexpr const char *fieldName = FieldName_;                                       \
    };                                                                                             \
    template<>                                                                                     \
    inline void draw_third_column<ClassName_>(const int rowId, const SdfLayerRefPtr &owner) {      \
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);                                 \
        DrawFun_(owner);                                                                           \
    }                                                                                              \
    template<>                                                                                     \
    inline bool has_edits<ClassName_>(const SdfLayerRefPtr &owner) {                               \
        return !owner->GetField(SdfPath::AbsoluteRootPath(), Token_).IsEmpty();                    \
    }                                                                                              \
    template<>                                                                                     \
    inline void draw_first_column<ClassName_>(const int rowId, const SdfLayerRefPtr &owner) {      \
        ImGui::PushID(rowId);                                                                      \
        if (ImGui::Button(ICON_FA_TRASH) && has_edits<ClassName_>(owner)) {                        \
            execute_after_draw(&SdfLayer::EraseField, owner, SdfPath::AbsoluteRootPath(), Token_); \
        }                                                                                          \
        ImGui::PopID();                                                                            \
    }

// Draw the layer path
struct LayerFilenameRow {
    static constexpr const char *fieldName = "Path";
};

template<>
inline void draw_third_column<LayerFilenameRow>(const int rowId, const SdfPath &path) {
    if (path == SdfPath() || path == SdfPath::AbsoluteRootPath()) {
        ImGui::Text("Layer root");
    } else {
        ImGui::Text("%s", path.GetString().c_str());
    }
}

// Draw the layer identifier
struct LayerIdentityRow {
    static constexpr const char *fieldName = "Layer";
};

template<>
inline void draw_third_column<LayerIdentityRow>(const int rowId, const SdfLayerRefPtr &owner) {
    ImGui::Text("%s", owner->GetIdentifier().c_str());
}

GENERATE_METADATA_FIELD(LayerFieldDefaultPrim, SdfFieldKeys->DefaultPrim, draw_default_prim, "Default prim");
GENERATE_METADATA_FIELD(LayerFieldUpAxis, UsdGeomTokens->upAxis, draw_up_axis, "Up axis");
GENERATE_METADATA_FIELD(LayerFieldStartTimeCode, SdfFieldKeys->StartTimeCode, DrawLayerStartTimeCode, "Start timecode");
GENERATE_METADATA_FIELD(LayerFieldEndTimeCode, SdfFieldKeys->EndTimeCode, DrawLayerEndTimeCode, "End timecode");
GENERATE_METADATA_FIELD(LayerFieldFramesPerSecond, SdfFieldKeys->FramesPerSecond, DrawLayerFramesPerSecond, "Frames per second");
GENERATE_METADATA_FIELD(LayerFieldFramePrecision, SdfFieldKeys->FramePrecision, DrawLayerFramePrecision, "Frame precision");
GENERATE_METADATA_FIELD(LayerFieldTimeCodesPerSecond, SdfFieldKeys->TimeCodesPerSecond, DrawLayerTimeCodesPerSecond, "TimeCodes per second");
GENERATE_METADATA_FIELD(LayerFieldMetersPerUnit, UsdGeomTokens->metersPerUnit, draw_layer_meters_per_unit, "Meters per unit");
GENERATE_METADATA_FIELD(LayerFieldRenderSettingsPrimPath, UsdRenderTokens->renderSettingsPrimPath, draw_render_settings_prim_path, "Render Settings Prim Path");
GENERATE_METADATA_FIELD(LayerFieldDocumentation, SdfFieldKeys->Documentation, DrawLayerDocumentation, "Documentation");
GENERATE_METADATA_FIELD(LayerFieldComment, SdfFieldKeys->Comment, DrawLayerComment, "Comment");

void DrawSdfLayerIdentity(const SdfLayerRefPtr &layer, const SdfPath &path) {
    if (!layer)
        return;
    int rowId = 0;
    if (begin_three_columns_table("##DrawLayerHeader")) {
        setup_three_columns_table(true, "", "Identity", "Value");
        draw_three_columns_row<LayerIdentityRow>(rowId++, layer);
        draw_three_columns_row<LayerFilenameRow>(rowId++, path);
        end_three_columns_table();
    }
}

void DrawSdfLayerMetadata(const SdfLayerRefPtr &layer) {
    if (!layer)
        return;
    int rowId = 0;
    if (ImGui::CollapsingHeader("Core Metadata", ImGuiTreeNodeFlags_DefaultOpen)) {
        if (begin_three_columns_table("##DrawLayerMetadata")) {
            // SetupFieldValueTableColumns(true, "", "Metadata");
            draw_three_columns_row<LayerFieldDefaultPrim>(rowId++, layer);
            draw_three_columns_row<LayerFieldUpAxis>(rowId++, layer);
            draw_three_columns_row<LayerFieldMetersPerUnit>(rowId++, layer);
            draw_three_columns_row<LayerFieldStartTimeCode>(rowId++, layer);
            draw_three_columns_row<LayerFieldEndTimeCode>(rowId++, layer);
            draw_three_columns_row<LayerFieldFramesPerSecond>(rowId++, layer);
            draw_three_columns_row<LayerFieldFramePrecision>(rowId++, layer);
            draw_three_columns_row<LayerFieldTimeCodesPerSecond>(rowId++, layer);
            draw_three_columns_row<LayerFieldRenderSettingsPrimPath>(rowId++, layer);
            draw_three_columns_row<LayerFieldDocumentation>(rowId++, layer);
            draw_three_columns_row<LayerFieldComment>(rowId++, layer);
            draw_three_columns_dictionary_editor<SdfPrimSpec>(rowId, layer->GetPseudoRoot(), SdfFieldKeys->CustomData);
            draw_three_columns_dictionary_editor<SdfPrimSpec>(rowId, layer->GetPseudoRoot(), SdfFieldKeys->AssetInfo);
            end_three_columns_table();
        }
    }
}

static void DrawSubLayerActionPopupMenu(const SdfLayerRefPtr &layer, const std::string &path) {
    auto subLayer = SdfLayer::FindOrOpenRelativeToLayer(layer, path);
    if (subLayer) {
        if (ImGui::MenuItem("Open as Stage")) {
            execute_after_draw<EditorOpenStage>(subLayer->GetRealPath());
        }
        if (ImGui::MenuItem("Open as Layer")) {
            execute_after_draw<EditorFindOrOpenLayer>(subLayer->GetRealPath());
        }
    } else {
        ImGui::Text("Unable to resolve layer path");
    }
}

struct SublayerPathRow {
    static constexpr const char *fieldName = "";
};

template<>
inline void draw_second_column<SublayerPathRow>(const int rowId, const SdfLayerRefPtr &layer, const std::string &path) {
    std::string newPath(path);
    ImGui::PushID(rowId);
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 3 * 28);
    ImGui::InputText("##sublayername", &newPath);
    if (ImGui::IsItemDeactivatedAfterEdit()) {
        execute_after_draw<LayerRenameSubLayer>(layer, path, newPath);
    }
    if (ImGui::BeginPopupContextItem("sublayer")) {
        DrawSubLayerActionPopupMenu(layer, path);
        ImGui::EndPopup();
    }
    ImGui::SameLine();
    if (ImGui::SmallButton(ICON_FA_FILE)) {
        draw_sublayer_path_edit_dialog(layer, path);
    }
    ImGui::SameLine();
    if (ImGui::SmallButton(ICON_FA_ARROW_UP)) {
        execute_after_draw<LayerMoveSubLayer>(layer, path, true);
    }
    ImGui::SameLine();
    if (ImGui::SmallButton(ICON_FA_ARROW_DOWN)) {
        execute_after_draw<LayerMoveSubLayer>(layer, path, false);
    }
    ImGui::PopID();
}

template<>
inline void draw_first_column<SublayerPathRow>(const int rowId, const SdfLayerRefPtr &layer, const std::string &path) {
    ImGui::PushID(rowId);
    if (ImGui::SmallButton(ICON_FA_TRASH)) {
        execute_after_draw<LayerRemoveSubLayer>(layer, path);
    }
    ImGui::PopID();
}

void draw_sdf_layer_editor_menu_bar(const SdfLayerRefPtr &layer) {
    bool enabled = layer;
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("New", enabled)) {
            if (ImGui::MenuItem("Sublayer path")) {
                std::string newName = "sublayer_" + std::to_string(layer->GetNumSubLayerPaths() + 1) + ".usd";
                execute_after_draw(&SdfLayer::InsertSubLayerPath, layer, newName, 0);// TODO find proper name that is not in the list of sublayer
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
}

// TODO that Should move to layerproperty
void draw_layer_sublayer_stack(const SdfLayerRefPtr &layer) {
    if (!layer || layer->GetNumSubLayerPaths() == 0)
        return;
    if (ImGui::CollapsingHeader("Sublayers", ImGuiTreeNodeFlags_DefaultOpen)) {
        int rowId = 0;
        bool hasUpdate = false;
        if (begin_two_columns_table("##DrawLayerSublayerStack")) {
            setup_two_columns_table(false, "", "Sublayers");
            auto subLayersProxy = layer->GetSubLayerPaths();
            for (int i = 0; i < layer->GetNumSubLayerPaths(); ++i) {
                const std::string &path = subLayersProxy[i];
                draw_two_columns_row<SublayerPathRow>(rowId++, layer, path);
            }
            end_two_columns_table();
        }
    }
}

void draw_layer_navigation(const SdfLayerRefPtr &layer) {
    if (!layer)
        return;
    if (ImGui::Button(ICON_FA_ARROW_LEFT)) {
        execute_after_draw<EditorSetPreviousLayer>();
    }
    ImGui::SameLine();
    if (ImGui::Button(ICON_FA_ARROW_RIGHT)) {
        execute_after_draw<EditorSetNextLayer>();
    }
    ImGui::SameLine();
    {
        ScopedStyleColor layerIsDirtyColor(ImGuiCol_Text,
                                           layer->IsDirty() ? ImGui::GetColorU32(ImGuiCol_Text) : ImU32(ImColor{ColorGreyish}));
        if (ImGui::Button(ICON_FA_REDO_ALT)) {
            execute_after_draw(&SdfLayer::Reload, layer, false);
        }
        ImGui::SameLine();
        if (ImGui::Button(ICON_FA_SAVE)) {
            execute_after_draw(&SdfLayer::Save, layer, false);
        }
    }
    ImGui::SameLine();
    if (!layer)
        return;

    {
        ScopedStyleColor textBackground(ImGuiCol_Header, ImU32(ImColor{ColorPrimHasComposition}));
        ImGui::Selectable("##LayerNavigation");
        if (ImGui::BeginPopupContextItem()) {
            draw_layer_action_popup_menu(layer);
            ImGui::EndPopup();
        }
        ImGui::SameLine();
        ImGui::Text("Layer: %s", layer->GetDisplayName().c_str());
        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::Text("%s", layer->GetRealPath().c_str());
            ImGui::EndTooltip();
        }
    }
}

/// Draw a popup menu with the possible action on a layer
void draw_layer_action_popup_menu(const SdfLayerHandle &layer, bool isStage) {
    if (!layer)
        return;

    if (!layer->IsAnonymous() && ImGui::MenuItem("Reload")) {
        execute_after_draw(&SdfLayer::Reload, layer, false);
    }
    if (!isStage && ImGui::MenuItem("Open as Stage")) {
        execute_after_draw<EditorOpenStage>(layer->GetRealPath());
    }
    if (layer->IsDirty() && !layer->IsAnonymous() && ImGui::MenuItem("Save layer")) {
        execute_after_draw(&SdfLayer::Save, layer, true);
    }
    if (ImGui::MenuItem("Save layer as")) {
        execute_after_draw<EditorSaveLayerAs>(layer);
    }

    ImGui::Separator();

    // Not sure how safe this is with the undo/redo
    if (layer->IsMuted() && ImGui::MenuItem("Unmute")) {
        execute_after_draw<LayerUnmute>(layer);
    }
    if (!layer->IsMuted() && ImGui::MenuItem("Mute")) {
        execute_after_draw<LayerMute>(layer);
    }

    ImGui::Separator();
    if (ImGui::MenuItem("Copy layer path")) {
        ImGui::SetClipboardText(layer->GetRealPath().c_str());
    }
}

}// namespace vox