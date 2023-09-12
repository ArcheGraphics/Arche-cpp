//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "commands/commands.h"
#include "composition_editor.h"
#include "file_browser.h"
#include "base/imgui_helpers.h"
#include "modal_dialogs.h"
#include "base/usd_helpers.h"
#include "edit_list_selector.h"
#include "table_layouts.h"
#include "base/constants.h"
#include <imgui_stdlib.h>
#include <algorithm>
#include <iostream>
#include <pxr/usd/sdf/payload.h>
#include <pxr/usd/sdf/primSpec.h>
#include <pxr/usd/sdf/reference.h>

namespace vox {
typedef enum CompositionArcListType { ReferenceList,
                                      PayloadList,
                                      InheritList,
                                      SpecializeList } CompositionArcListType;

// Unfortunately Inherit and Specialize are just alias to SdfPath, there is no way to differentiate find the
// edit list the sdfpath is coming from using the type.
// To reuse templated code we create new type SdfInherit and SdfSpecialize
template<int InheritOrSpecialize>
struct SdfInheritOrSpecialize : SdfPath {
    SdfInheritOrSpecialize() : SdfPath() {}
    SdfInheritOrSpecialize(const SdfPath &path) : SdfPath(path) {}
};

using SdfInherit = SdfInheritOrSpecialize<0>;
using SdfSpecialize = SdfInheritOrSpecialize<1>;

template<typename ArcListItemT>
struct ArcListItemTrait {
    typedef ArcListItemT type;
};
template<>
struct ArcListItemTrait<SdfInherit> {
    typedef SdfPath type;
};
template<>
struct ArcListItemTrait<SdfSpecialize> {
    typedef SdfPath type;
};

static bool has_composition(const SdfPrimSpecHandle &primSpec) {
    return primSpec->HasReferences() || primSpec->HasPayloads() || primSpec->HasInheritPaths() || primSpec->HasSpecializes();
}

inline SdfReferencesProxy get_composition_arc_list(const SdfPrimSpecHandle &primSpec, const SdfReference &val) {
    return primSpec->GetReferenceList();
}

inline SdfPayloadsProxy get_composition_arc_list(const SdfPrimSpecHandle &primSpec, const SdfPayload &val) {
    return primSpec->GetPayloadList();
}

inline SdfInheritsProxy get_composition_arc_list(const SdfPrimSpecHandle &primSpec, const SdfInherit &val) {
    return primSpec->GetInheritPathList();
}

inline SdfSpecializesProxy get_composition_arc_list(const SdfPrimSpecHandle &primSpec, const SdfSpecialize &val) {
    return primSpec->GetSpecializesList();
}

inline void clear_arc_list(const SdfPrimSpecHandle &primSpec, const SdfReference &val) {
    return primSpec->ClearReferenceList();
}

inline void clear_arc_list(const SdfPrimSpecHandle &primSpec, const SdfPayload &val) {
    return primSpec->ClearPayloadList();
}

inline void clear_arc_list(const SdfPrimSpecHandle &primSpec, const SdfInherit &val) {
    return primSpec->ClearInheritPathList();
}

inline void clear_arc_list(const SdfPrimSpecHandle &primSpec, const SdfSpecialize &val) {
    return primSpec->ClearSpecializesList();
}

inline void select_arc_type(const SdfPrimSpecHandle &primSpec, const SdfReference &ref) {
    auto realPath = ref.GetAssetPath().empty() ? primSpec->GetLayer()->GetRealPath() : primSpec->GetLayer()->ComputeAbsolutePath(ref.GetAssetPath());
    auto layerOrOpen = SdfLayer::FindOrOpen(realPath);
    execute_after_draw<EditorSetSelection>(layerOrOpen, ref.GetPrimPath());
}

inline void select_arc_type(const SdfPrimSpecHandle &primSpec, const SdfPayload &pay) {
    auto realPath = primSpec->GetLayer()->ComputeAbsolutePath(pay.GetAssetPath());
    auto layerOrOpen = SdfLayer::FindOrOpen(realPath);
    execute_after_draw<EditorSetSelection>(layerOrOpen, pay.GetPrimPath());
}

inline void select_arc_type(const SdfPrimSpecHandle &primSpec, const SdfPath &path) {
    execute_after_draw<EditorSetSelection>(primSpec->GetLayer(), path);
}

/// A SdfPath creation UI.
/// This is used for inherit and specialize
struct CreateSdfPathModalDialog : public ModalDialog {
    CreateSdfPathModalDialog(const SdfPrimSpecHandle &primSpec) : _primSpec(primSpec){};
    ~CreateSdfPathModalDialog() override {}

    void draw() override {
        if (!_primSpec) {
            close_modal();
            return;
        }
        // TODO: We will probably want to browse in the scene hierarchy to select the path
        //   create a selection tree, one day
        ImGui::Text("%s", _primSpec->GetPath().GetString().c_str());
        if (ImGui::BeginCombo("Operation", get_list_editor_operation_name(_operation))) {
            for (int n = 0; n < get_list_editor_operation_size(); n++) {
                if (ImGui::Selectable(get_list_editor_operation_name(SdfListOpType(n)))) {
                    _operation = SdfListOpType(n);
                }
            }
            ImGui::EndCombo();
        }
        ImGui::InputText("Target prim path", &_primPath);
        draw_ok_cancel_modal([=]() { on_ok_call_back(); });
    }

    virtual void on_ok_call_back() = 0;

    const char *dialog_id() const override { return "Sdf path"; }

    SdfPrimSpecHandle _primSpec;
    std::string _primPath;
    SdfListOpType _operation = SdfListOpTypeExplicit;
};

/// UI used to create an AssetPath having a file path to a layer and a
/// target prim path.
/// This is used by References and Payloads which share the same interface
struct CreateAssetPathModalDialog : public ModalDialog {
    CreateAssetPathModalDialog(const SdfPrimSpecHandle &primSpec) : _primSpec(primSpec){};
    ~CreateAssetPathModalDialog() override {}

    void draw() override {
        if (!_primSpec) {
            close_modal();
            return;
        }
        ImGui::Text("%s", _primSpec->GetPath().GetString().c_str());

        if (ImGui::BeginCombo("Operation", get_list_editor_operation_name(_operation))) {
            for (int n = 0; n < get_list_editor_operation_size(); n++) {
                if (ImGui::Selectable(get_list_editor_operation_name(n))) {
                    _operation = static_cast<SdfListOpType>(n);
                }
            }
            ImGui::EndCombo();
        }
        ImGui::InputText("File path", &_assetPath);
        ImGui::SameLine();
        if (ImGui::Button(ICON_FA_FILE)) {
            ImGui::OpenPopup("Asset path browser");
        }
        if (ImGui::BeginPopupModal("Asset path browser")) {
            draw_file_browser();
            ImGui::Checkbox("Use relative path", &_relative);
            ImGui::Checkbox("Unix compatible", &_unixify);
            if (ImGui::Button("Use selected file")) {
                if (_relative) {
                    _assetPath = get_file_browser_file_path_relative_to(_primSpec->GetLayer()->GetRealPath(), _unixify);
                } else {
                    _assetPath = get_file_browser_file_path();
                }
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Close")) {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
        ImGui::InputText("Target prim path", &_primPath);
        ImGui::InputDouble("Layer time offset", &_timeOffset);
        ImGui::InputDouble("Layer time scale", &_timeScale);
        draw_ok_cancel_modal([=]() { on_ok_call_back(); });
    }

    virtual void on_ok_call_back() = 0;

    const char *dialog_id() const override { return "Asset path"; }

    SdfLayerOffset get_layer_offset() const {
        return (_timeScale != 1.0 || _timeOffset != 0.0) ? SdfLayerOffset(_timeOffset, _timeScale) : SdfLayerOffset();
    }

    SdfPrimSpecHandle _primSpec;
    std::string _assetPath;
    std::string _primPath;
    SdfListOpType _operation = SdfListOpTypeExplicit;

    bool _relative = false;
    bool _unixify = false;
    double _timeScale = 1.0;
    double _timeOffset = 0.0;
};

// Inheriting, but could also be done with templates, would the code be cleaner ?
struct CreateReferenceModalDialog : public CreateAssetPathModalDialog {
    CreateReferenceModalDialog(const SdfPrimSpecHandle &primSpec) : CreateAssetPathModalDialog(primSpec) {}
    const char *dialog_id() const override { return "Create reference"; }
    void on_ok_call_back() override {
        SdfReference reference(_assetPath, SdfPath(_primPath), get_layer_offset());
        execute_after_draw<PrimCreateReference>(_primSpec, _operation, reference);
    }
};

struct CreatePayloadModalDialog : public CreateAssetPathModalDialog {
    CreatePayloadModalDialog(const SdfPrimSpecHandle &primSpec) : CreateAssetPathModalDialog(primSpec) {}
    const char *dialog_id() const override { return "Create payload"; }
    void on_ok_call_back() override {
        SdfPayload payload(_assetPath, SdfPath(_primPath), get_layer_offset());
        execute_after_draw<PrimCreatePayload>(_primSpec, _operation, payload);
    }
};

struct CreateInheritModalDialog : public CreateSdfPathModalDialog {
    CreateInheritModalDialog(const SdfPrimSpecHandle &primSpec) : CreateSdfPathModalDialog(primSpec) {}
    const char *dialog_id() const override { return "Create inherit"; }
    void on_ok_call_back() override { execute_after_draw<PrimCreateInherit>(_primSpec, _operation, SdfPath(_primPath)); }
};

struct CreateSpecializeModalDialog : public CreateSdfPathModalDialog {
    CreateSpecializeModalDialog(const SdfPrimSpecHandle &primSpec) : CreateSdfPathModalDialog(primSpec) {}
    const char *dialog_id() const override { return "Create specialize"; }
    void on_ok_call_back() override { execute_after_draw<PrimCreateSpecialize>(_primSpec, _operation, SdfPath(_primPath)); }
};

void draw_prim_create_reference(const SdfPrimSpecHandle &primSpec) { draw_modal_dialog<CreateReferenceModalDialog>(primSpec); }
void draw_prim_create_payload(const SdfPrimSpecHandle &primSpec) { draw_modal_dialog<CreatePayloadModalDialog>(primSpec); }
void draw_prim_create_inherit(const SdfPrimSpecHandle &primSpec) { draw_modal_dialog<CreateInheritModalDialog>(primSpec); }
void draw_prim_create_specialize(const SdfPrimSpecHandle &primSpec) { draw_modal_dialog<CreateSpecializeModalDialog>(primSpec); }

static SdfListOpType opList = SdfListOpTypeExplicit;

template<typename ArcT>
void draw_arc_creation_dialog(const SdfPrimSpecHandle &primSpec, SdfListOpType opList);
template<>
void draw_arc_creation_dialog<SdfReference>(const SdfPrimSpecHandle &primSpec, SdfListOpType opList) {
    draw_modal_dialog<CreateReferenceModalDialog>(primSpec);
}
template<>
void draw_arc_creation_dialog<SdfPayload>(const SdfPrimSpecHandle &primSpec, SdfListOpType opList) {
    draw_modal_dialog<CreatePayloadModalDialog>(primSpec);
}
template<>
void draw_arc_creation_dialog<SdfInherit>(const SdfPrimSpecHandle &primSpec, SdfListOpType opList) {
    draw_modal_dialog<CreateInheritModalDialog>(primSpec);
}

template<>
void draw_arc_creation_dialog<SdfSpecialize>(const SdfPrimSpecHandle &primSpec, SdfListOpType opList) {
    draw_modal_dialog<CreateSpecializeModalDialog>(primSpec);
}
template<typename ArcT>
inline void remove_arc(const SdfPrimSpecHandle &primSpec, const ArcT &arc) {
    std::function<void()> removeItem = [=]() {
        get_composition_arc_list(primSpec, arc).RemoveItemEdits(arc);
        // Also clear the arc list if there are no more items
        if (get_composition_arc_list(primSpec, arc).HasKeys()) {
            clear_arc_list(primSpec, arc);
        }
    };
    execute_after_draw<UsdFunctionCall>(primSpec->GetLayer(), removeItem);
}

///
template<typename ArcT>
void draw_sdf_path_menu_items(const SdfPrimSpecHandle &primSpec, const SdfPath &path) {
    if (ImGui::MenuItem("Remove")) {
        if (!primSpec)
            return;
        remove_arc(primSpec, ArcT(path));
    }
    if (ImGui::MenuItem("Copy path")) {
        ImGui::SetClipboardText(path.GetString().c_str());
    }
}

/// Draw the menu items for AssetPaths (SdfReference and SdfPayload)
template<typename AssetPathT>
void draw_asset_path_menu_items(const SdfPrimSpecHandle &primSpec, const AssetPathT &assetPath) {
    if (ImGui::MenuItem("Select Arc")) {
        auto realPath = primSpec->GetLayer()->ComputeAbsolutePath(assetPath.GetAssetPath());
        execute_after_draw<EditorFindOrOpenLayer>(realPath);
    }
    if (ImGui::MenuItem("Open as Stage")) {
        auto realPath = primSpec->GetLayer()->ComputeAbsolutePath(assetPath.GetAssetPath());
        execute_after_draw<EditorOpenStage>(realPath);
    }
    ImGui::Separator();
    if (ImGui::MenuItem("Remove")) {
        if (!primSpec)
            return;
        remove_arc(primSpec, assetPath);
    }
    ImGui::Separator();
    if (ImGui::MenuItem("Copy asset path")) {
        ImGui::SetClipboardText(assetPath.GetAssetPath().c_str());
    }
}

template<typename InheritOrSpecialize>
InheritOrSpecialize draw_sdf_path_editor(const SdfPrimSpecHandle &primSpec, const InheritOrSpecialize &arc, ImVec2 outerSize) {
    InheritOrSpecialize updatedArc;
    std::string updatedPath = arc.GetString();
    constexpr ImGuiTableFlags tableFlags = ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_PreciseWidths |
                                           ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_Resizable;
    if (ImGui::BeginTable("DrawSdfPathEditor", 1, tableFlags, outerSize)) {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::SetNextItemWidth(-FLT_MIN);
        ImGui::InputText("##assetpath", &updatedPath);
        if (ImGui::IsItemDeactivatedAfterEdit()) {
            updatedArc = SdfPath(updatedPath);
        }
        ImGui::EndTable();
    }
    return updatedArc;
}

// Works with SdfReference and SdfPayload
template<typename ReferenceOrPayloadT>
ReferenceOrPayloadT draw_reference_or_payload_editor(const SdfPrimSpecHandle &primSpec, const ReferenceOrPayloadT &ref,
                                                     ImVec2 outerSize) {
    ReferenceOrPayloadT ret;
    std::string updatedPath = ref.GetAssetPath();
    std::string targetPath = ref.GetPrimPath().GetString();
    SdfLayerOffset layerOffset = ref.GetLayerOffset();
    float offset = layerOffset.GetOffset();
    float scale = layerOffset.GetScale();
    ImGui::PushID("DrawAssetPathArcEditor");
    constexpr ImGuiTableFlags tableFlags = ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_PreciseWidths |
                                           ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_Resizable;
    if (ImGui::BeginTable("DrawAssetPathArcEditorTable", 4, tableFlags, outerSize)) {
        const float stretchLayer = (layerOffset == SdfLayerOffset()) ? 0.01 : 0.1;
        const float stretchTarget = (targetPath.empty()) ? 0.01 : 0.3 * (1 - 2 * stretchLayer);
        const float stretchPath = 1 - 2 * stretchLayer - stretchTarget;
        ImGui::TableSetupColumn("path", ImGuiTableColumnFlags_WidthStretch, stretchPath);
        ImGui::TableSetupColumn("target", ImGuiTableColumnFlags_WidthStretch, stretchTarget);
        ImGui::TableSetupColumn("offset", ImGuiTableColumnFlags_WidthStretch, stretchLayer);
        ImGui::TableSetupColumn("scale", ImGuiTableColumnFlags_WidthStretch, stretchLayer);
        ImGui::TableNextRow();

        ImGui::TableSetColumnIndex(0);
        ImGui::SetNextItemWidth(-FLT_MIN);
        ImGui::InputText("##assetpath", &updatedPath);
        if (ImGui::IsItemDeactivatedAfterEdit()) {
            ret = ref;
            ret.SetAssetPath(updatedPath);
        }
        // TODO more operation on the path: unixify, make relative, etc
        if (ImGui::BeginPopupContextItem("sublayer")) {
            ImGui::Text("%s", updatedPath.c_str());
            draw_asset_path_menu_items(primSpec, ref);
            ImGui::EndPopup();
        }
        ImGui::SameLine();

        ImGui::TableSetColumnIndex(1);
        ImGui::SetNextItemWidth(-FLT_MIN);
        ImGui::InputText("##targetpath", &targetPath);
        if (ImGui::IsItemDeactivatedAfterEdit()) {
            ret = ref;
            ret.SetPrimPath(SdfPath(targetPath));
        }

        ImGui::TableSetColumnIndex(2);
        ImGui::SetNextItemWidth(-FLT_MIN);
        ImGui::InputFloat("##offset", &offset);
        if (ImGui::IsItemDeactivatedAfterEdit()) {
            SdfLayerOffset updatedLayerOffset(layerOffset);
            updatedLayerOffset.SetOffset(offset);
            ret = ref;
            ret.SetLayerOffset(updatedLayerOffset);
        }

        ImGui::TableSetColumnIndex(3);
        ImGui::SetNextItemWidth(-FLT_MIN);
        ImGui::InputFloat("##scale", &scale);
        if (ImGui::IsItemDeactivatedAfterEdit()) {
            SdfLayerOffset updatedLayerOffset(layerOffset);
            updatedLayerOffset.SetScale(scale);
            ret = ref;
            ret.SetLayerOffset(updatedLayerOffset);
        }
        ImGui::EndTable();
    }
    ImGui::PopID();
    return ret;
}

inline SdfReference draw_composition_arc_editor(const SdfPrimSpecHandle &primSpec, const SdfReference &arc, ImVec2 outerSize) {
    return draw_reference_or_payload_editor(primSpec, arc, outerSize);
}

inline SdfPayload draw_composition_arc_editor(const SdfPrimSpecHandle &primSpec, const SdfPayload &arc, ImVec2 outerSize) {
    return draw_reference_or_payload_editor(primSpec, arc, outerSize);
}

inline SdfInherit draw_composition_arc_editor(const SdfPrimSpecHandle &primSpec, const SdfInherit &arc, ImVec2 outerSize) {
    return draw_sdf_path_editor(primSpec, arc, outerSize);
}

inline SdfSpecialize draw_composition_arc_editor(const SdfPrimSpecHandle &primSpec, const SdfSpecialize &arc, ImVec2 outerSize) {
    return draw_sdf_path_editor(primSpec, arc, outerSize);
}

template<typename CompositionArcT>
void draw_composition_arc_row(int rowId, const SdfPrimSpecHandle &primSpec, const CompositionArcT &arc, const SdfListOpType &op) {
    // Arbitrary outer size for the reference editor
    using ItemType = typename ArcListItemTrait<CompositionArcT>::type;
    const float regionAvailable = ImGui::GetWindowWidth() - 3 * 28 - 20;// 28 to account for buttons, 20 is arbitrary
    ImVec2 outerSize(regionAvailable, TableRowDefaultHeight);
    CompositionArcT updatedArc = draw_composition_arc_editor(primSpec, arc, outerSize);
    if (updatedArc != CompositionArcT()) {
        std::function<void()> updateReferenceFun = [=]() {
            auto arcList = get_composition_arc_list(primSpec, arc);
            auto editList = get_sdf_list_op_items(arcList, op);
            editList[rowId] = updatedArc;
        };
        execute_after_draw<UsdFunctionCall>(primSpec->GetLayer(), updateReferenceFun);
    }
    ImGui::SameLine();
    if (ImGui::Button(ICON_FA_ARROW_UP)) {
        std::function<void()> moveUp = [=]() {
            auto arcList = get_composition_arc_list(primSpec, arc);
            auto editList = get_sdf_list_op_items(arcList, op);
            if (rowId >= 1) {// std::swap doesn't compile here
                auto it = editList[rowId];
                ItemType tmp = it;
                editList.erase(editList.begin() + rowId);
                editList.insert(editList.begin() + rowId - 1, tmp);
            }
        };
        execute_after_draw<UsdFunctionCall>(primSpec->GetLayer(), moveUp);
    }
    ImGui::SameLine();
    if (ImGui::Button(ICON_FA_ARROW_DOWN)) {
        std::function<void()> moveDown = [=]() {
            auto arcList = get_composition_arc_list(primSpec, arc);
            auto editList = get_sdf_list_op_items(arcList, op);
            if (rowId < editList.size() - 1) {// std::swap doesn't compile here
                auto it = editList[rowId];
                ItemType tmp = it;
                editList.erase(editList.begin() + rowId);
                editList.insert(editList.begin() + rowId + 1, tmp);
            }
        };
        execute_after_draw<UsdFunctionCall>(primSpec->GetLayer(), moveDown);
    }
}

struct CompositionArcRow {};// Rany of SdfReference SdfPayload SdfInherit and SdfSpecialize

#define GENERATE_ARC_DRAW_CODE(ClassName_)                                                                                 \
    template<>                                                                                                             \
    inline void draw_first_column<CompositionArcRow>(int rowId, const SdfPrimSpecHandle &primSpec, const ClassName_ &arc,  \
                                                     const SdfListOpType &chosenList) {                                    \
        if (ImGui::Button(ICON_FA_TRASH)) {                                                                                \
            remove_arc(primSpec, arc);                                                                                     \
        }                                                                                                                  \
    }                                                                                                                      \
                                                                                                                           \
    template<>                                                                                                             \
    inline void draw_second_column<CompositionArcRow>(int rowId, const SdfPrimSpecHandle &primSpec, const ClassName_ &arc, \
                                                      const SdfListOpType &chosenList) {                                   \
        draw_composition_arc_row(rowId, primSpec, arc, chosenList);                                                        \
    }
GENERATE_ARC_DRAW_CODE(SdfReference)
GENERATE_ARC_DRAW_CODE(SdfPayload)
GENERATE_ARC_DRAW_CODE(SdfInherit)
GENERATE_ARC_DRAW_CODE(SdfSpecialize)

template<typename CompositionArcItemT>
void DrawCompositionEditor(const SdfPrimSpecHandle &primSpec) {
    using ItemType = typename ArcListItemTrait<CompositionArcItemT>::type;

    if (ImGui::Button(ICON_FA_PLUS)) {
        draw_arc_creation_dialog<CompositionArcItemT>(primSpec, opList);
    }

    auto arcList = get_composition_arc_list(primSpec, CompositionArcItemT());
    SdfListOpType opList = get_edit_list_choice(arcList);

    ImGui::SameLine();
    draw_edit_list_combo_selector(opList, arcList);

    if (begin_two_columns_table("##AssetTypeTable")) {
        auto editList = get_sdf_list_op_items(arcList, opList);
        for (int ind = 0; ind < editList.size(); ind++) {
            const ItemType &item = editList[ind];
            ImGui::PushID(static_cast<int>(TfHash{}(item)));
            const CompositionArcItemT arcItem(item);// Sadly we have to copy here,
            draw_two_columns_row<CompositionArcRow>(ind, primSpec, arcItem, opList);
            ImGui::PopID();
        }
        end_two_columns_table();
    }
}

void DrawPrimCompositions(const SdfPrimSpecHandle &primSpec) {
    if (!primSpec || !has_composition(primSpec))
        return;
    if (ImGui::CollapsingHeader("Composition", ImGuiTreeNodeFlags_DefaultOpen)) {
        if (ImGui::BeginTabBar("##CompositionType")) {
            if (ImGui::BeginTabItem("References", nullptr,
                                    primSpec->HasReferences() ? ImGuiTabItemFlags_UnsavedDocument : ImGuiTabItemFlags_None)) {
                DrawCompositionEditor<SdfReference>(primSpec);
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Payloads", nullptr,
                                    primSpec->HasPayloads() ? ImGuiTabItemFlags_UnsavedDocument : ImGuiTabItemFlags_None)) {
                DrawCompositionEditor<SdfPayload>(primSpec);
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Inherits", nullptr,
                                    primSpec->HasInheritPaths() ? ImGuiTabItemFlags_UnsavedDocument : ImGuiTabItemFlags_None)) {
                DrawCompositionEditor<SdfInherit>(primSpec);
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Specializes", nullptr,
                                    primSpec->HasSpecializes() ? ImGuiTabItemFlags_UnsavedDocument : ImGuiTabItemFlags_None)) {
                DrawCompositionEditor<SdfSpecialize>(primSpec);
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
    }
}
/////////////// Summaries used in the layer scene editor

template<typename ArcT>
inline void DrawSdfPathSummary(std::string &&header, SdfListOpType operation, const SdfPath &path,
                               const SdfPrimSpecHandle &primSpec, int &menuItemId) {
    ScopedStyleColor transparentStyle(ImGuiCol_Button, ImVec4(ColorTransparent));
    ImGui::PushID(menuItemId++);
    if (ImGui::Button(ICON_FA_TRASH)) {
        remove_arc(primSpec, ArcT(path));
    }
    ImGui::SameLine();
    std::string summary = path.GetString();
    if (ImGui::SmallButton(summary.c_str())) {
        select_arc_type(primSpec, path);
    }
    if (ImGui::BeginPopupContextItem()) {
        draw_sdf_path_menu_items<ArcT>(primSpec, path);
        ImGui::EndPopup();
    }
    ImGui::PopID();
}

template<typename AssetPathT>
inline void DrawAssetPathSummary(std::string &&header, SdfListOpType operation, const AssetPathT &assetPath,
                                 const SdfPrimSpecHandle &primSpec, int &menuItemId) {
    ScopedStyleColor transparentStyle(ImGuiCol_Button, ImVec4(ColorTransparent));
    ImGui::PushID(menuItemId++);
    if (ImGui::Button(ICON_FA_TRASH)) {
        remove_arc(primSpec, assetPath);
    }
    ImGui::PopID();
    ImGui::SameLine();
    std::string summary = get_list_editor_operation_name(operation);
    summary += " ";
    summary += assetPath.GetAssetPath().empty() ? "" : "@" + assetPath.GetAssetPath() + "@";
    summary += assetPath.GetPrimPath().GetString().empty() ? "" : "<" + assetPath.GetPrimPath().GetString() + ">";
    ImGui::PushID(menuItemId++);
    if (ImGui::Button(summary.c_str())) {
        select_arc_type(primSpec, assetPath);
    }
    if (ImGui::BeginPopupContextItem("###AssetPathMenuItems")) {
        draw_asset_path_menu_items(primSpec, assetPath);
        ImGui::EndPopup();
    }
    ImGui::PopID();
}

void draw_reference_summary(SdfListOpType operation, const SdfReference &assetPath, const SdfPrimSpecHandle &primSpec,
                            int &menuItemId) {
    DrawAssetPathSummary("References", operation, assetPath, primSpec, menuItemId);
}

void draw_payload_summary(SdfListOpType operation, const SdfPayload &assetPath, const SdfPrimSpecHandle &primSpec, int &menuItemId) {
    DrawAssetPathSummary("Payloads", operation, assetPath, primSpec, menuItemId);
}

void draw_inherit_path_summary(SdfListOpType operation, const SdfPath &path, const SdfPrimSpecHandle &primSpec, int &menuItemId) {
    DrawSdfPathSummary<SdfInherit>("Inherits", operation, path, primSpec, menuItemId);
}

void draw_specializes_summary(SdfListOpType operation, const SdfPath &path, const SdfPrimSpecHandle &primSpec, int &menuItemId) {
    DrawSdfPathSummary<SdfSpecialize>("Specializes", operation, path, primSpec, menuItemId);
}

inline std::string get_arc_summary(const SdfReference &arc) { return arc.IsInternal() ? arc.GetPrimPath().GetString() : arc.GetAssetPath(); }
inline std::string get_arc_summary(const SdfPayload &arc) { return arc.GetAssetPath(); }
inline std::string get_arc_summary(const SdfPath &arc) { return arc.GetString(); }

inline void draw_arc_type_menu_items(const SdfPrimSpecHandle &primSpec, const SdfReference &ref) {
    draw_asset_path_menu_items(primSpec, ref);
}
inline void draw_arc_type_menu_items(const SdfPrimSpecHandle &primSpec, const SdfPayload &pay) {
    draw_asset_path_menu_items(primSpec, pay);
}
inline void draw_arc_type_menu_items(const SdfPrimSpecHandle &primSpec, const SdfInherit &inh) {
    draw_sdf_path_menu_items<SdfInherit>(primSpec, inh);
}
inline void draw_arc_type_menu_items(const SdfPrimSpecHandle &primSpec, const SdfSpecialize &spe) {
    draw_sdf_path_menu_items<SdfSpecialize>(primSpec, spe);
}

template<typename ArcT>
inline void draw_path_in_row(SdfListOpType operation, const ArcT &assetPath, const SdfPrimSpecHandle &primSpec, int *itemId) {
    std::string path;
    path += get_arc_summary(assetPath);
    ImGui::PushID((*itemId)++);
    ImGui::SameLine();
    if (ImGui::Button(path.c_str())) {
        select_arc_type(primSpec, assetPath);
    }
    if (ImGui::BeginPopupContextItem("###AssetPathMenuItems")) {
        draw_arc_type_menu_items(primSpec, assetPath);
        ImGui::EndPopup();
    }
    ImGui::PopID();
}

#define CREATE_COMPOSITION_BUTTON(NAME_, ABBR_, GETLIST_)                                                            \
    if (primSpec->Has##NAME_##s()) {                                                                                 \
        if (buttonId > 0)                                                                                            \
            ImGui::SameLine();                                                                                       \
        ImGui::PushID(buttonId++);                                                                                   \
        ImGui::SmallButton(#ABBR_);                                                                                  \
        if (ImGui::BeginPopupContextItem(nullptr, buttonFlags)) {                                                    \
            iterate_list_editor_items(primSpec->Get##GETLIST_##List(), Draw##GETLIST_##Summary, primSpec, buttonId); \
            ImGui::EndPopup();                                                                                       \
        }                                                                                                            \
        ImGui::PopID();                                                                                              \
    }

void DrawPrimCompositionSummary(const SdfPrimSpecHandle &primSpec) {
    if (!primSpec || !has_composition(primSpec))
        return;
    ScopedStyleColor transparentStyle(ImGuiCol_Button, ImVec4(ColorTransparent));

    // Buttons are too far appart
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.f, -FLT_MIN));
    int buttonId = 0;
    // First draw the Buttons, Ref, Pay etc
    constexpr ImGuiPopupFlags buttonFlags = ImGuiPopupFlags_MouseButtonLeft;

    CREATE_COMPOSITION_BUTTON(Reference, Ref, Reference)
    CREATE_COMPOSITION_BUTTON(Payload, Pay, Payload)
    CREATE_COMPOSITION_BUTTON(InheritPath, Inh, InheritPath)
    CREATE_COMPOSITION_BUTTON(Specialize, Inh, Specializes)

    // TODO: stretch each paths to match the cell size. Add ellipsis at the beginning if they are too short
    // The idea is to see the relevant informations, and be able to quicly click on them
    // - another thought ... replace the common prefix by an ellipsis ? (only for asset paths)
    int itemId = 0;
    iterate_list_editor_items(primSpec->GetReferenceList(), draw_path_in_row<SdfReference>, primSpec, &itemId);
    iterate_list_editor_items(primSpec->GetPayloadList(), draw_path_in_row<SdfPayload>, primSpec, &itemId);
    iterate_list_editor_items(primSpec->GetInheritPathList(), draw_path_in_row<SdfInherit>, primSpec, &itemId);
    iterate_list_editor_items(primSpec->GetSpecializesList(), draw_path_in_row<SdfSpecialize>, primSpec, &itemId);
    ImGui::PopStyleVar();
}

}// namespace vox
