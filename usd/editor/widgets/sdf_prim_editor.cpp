//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <array>

#include <pxr/usd/kind/registry.h>
#include <pxr/usd/sdf/attributeSpec.h>
#include <pxr/usd/sdf/fileFormat.h>
#include <pxr/usd/sdf/layerUtils.h>
#include <pxr/usd/sdf/primSpec.h>
#include <pxr/usd/sdf/propertySpec.h>
#include <pxr/usd/sdf/schema.h>
#include <pxr/usd/sdf/types.h>
#include <pxr/usd/sdf/variantSetSpec.h>
#include <pxr/usd/sdf/variantSpec.h>
#include <pxr/usd/usd/prim.h>
#include <pxr/usd/usd/tokens.h>
#include <pxr/base/plug/registry.h>
#include <imgui.h>
#include <imgui_stdlib.h>
#include "commands/commands.h"
#include "composition_editor.h"
#include "file_browser.h"
#include "base/imgui_helpers.h"
#include "modal_dialogs.h"
#include "base/usd_helpers.h"
#include "base/constants.h"
#include "edit_list_selector.h"
#include "sdf_layer_editor.h"
#include "sdf_prim_editor.h"
#include "sdf_attribute_editor.h"
#include "commands/shortcuts.h"
#include "table_layouts.h"
#include "variant_editor.h"
#include "vt_dictionary_editor.h"
#include "vt_value_editor.h"

namespace vox {
//// NOTES: Sdf API: Removing a variantSet and cleaning it from the list editing
//// -> https://groups.google.com/g/usd-interest/c/OeqtGl_1H-M/m/xjCx3dT9EgAJ

// TODO: by default use the schema type of the prim
// TODO: copy metadata ? interpolation, etc ??
// TODO: write the code for creating relationship from schema
// TODO: when a schema has no attribute, the ok button should be disabled
//       and the name should be empty instead of the widget disappearing

std::vector<std::string> get_prim_spec_property_names(const TfToken &primTypeName, SdfSpecType filter) {
    std::vector<std::string> filteredPropNames;
    if (auto *primDefinition = UsdSchemaRegistry::GetInstance().FindConcretePrimDefinition(primTypeName)) {
        for (const auto &propName : primDefinition->GetPropertyNames()) {
            if (primDefinition->GetSpecType(propName) == filter) {
                filteredPropNames.push_back(propName.GetString());
            }
        }
    }
    return filteredPropNames;
}

struct CreateAttributeDialog : public ModalDialog {
    explicit CreateAttributeDialog(const SdfPrimSpecHandle &sdfPrim) : _sdfPrim(sdfPrim), _schemaTypeNames(get_all_spec_type_names()) {
        // https://openusd.org/release/api/class_usd_prim_definition.html
        selectedSchemaTypeName = std::find(_schemaTypeNames.begin(), _schemaTypeNames.end(), _sdfPrim->GetTypeName().GetString());
        fill_attribute_names();
        update_with_new_prim_definition();
    };
    ~CreateAttributeDialog() override = default;

    void update_with_new_prim_definition() {
        if (selectedSchemaTypeName != _schemaTypeNames.end() && _selectedAttributeName != _allAttrNames.end()) {
            if (auto *primDefinition =
                    UsdSchemaRegistry::GetInstance().FindConcretePrimDefinition(TfToken(*selectedSchemaTypeName))) {
                _attributeName = *_selectedAttributeName;
                SdfAttributeSpecHandle attrSpec = primDefinition->GetSchemaAttributeSpec(TfToken(_attributeName));
                _typeName = attrSpec->GetTypeName();
                _variability = attrSpec->GetVariability();
            }
        } else {
            _attributeName = "";
        }
    }

    void fill_attribute_names() {
        if (selectedSchemaTypeName != _schemaTypeNames.end()) {
            _allAttrNames = get_prim_spec_property_names(TfToken(*selectedSchemaTypeName), SdfSpecTypeAttribute);
            _selectedAttributeName = _allAttrNames.begin();
        } else {
            _attributeName = "";
        }
    }

    void draw() override {
        // Schema
        ImGui::Checkbox("Find attribute in schema", &_useSchema);
        ImGui::BeginDisabled(!_useSchema);
        const char *schemaStr = selectedSchemaTypeName == _schemaTypeNames.end() ? "" : selectedSchemaTypeName->c_str();
        if (ImGui::BeginCombo("From schema", schemaStr)) {
            for (size_t i = 0; i < _schemaTypeNames.size(); i++) {
                if (!_schemaTypeNames[i].empty() && ImGui::Selectable(_schemaTypeNames[i].c_str(), false)) {
                    selectedSchemaTypeName = _schemaTypeNames.begin() + i;
                    fill_attribute_names();
                    update_with_new_prim_definition();
                }
            }
            ImGui::EndCombo();
        }
        ImGui::EndDisabled();
        // Attribute name
        if (!_useSchema) {
            ImGui::InputText("Name", &_attributeName);
        } else {
            if (ImGui::BeginCombo("Name", _attributeName.c_str())) {
                for (size_t i = 0; i < _allAttrNames.size(); i++) {
                    if (ImGui::Selectable(_allAttrNames[i].c_str(), false)) {
                        _selectedAttributeName = _allAttrNames.begin() + i;
                        update_with_new_prim_definition();
                    }
                }
                ImGui::EndCombo();
            }
        }
        ImGui::BeginDisabled(_useSchema);
        if (ImGui::BeginCombo("Type", _typeName.GetAsToken().GetString().c_str())) {
            for (auto i : get_all_value_type_names()) {
                if (ImGui::Selectable(i.GetAsToken().GetString().c_str(), false)) {
                    _typeName = i;
                }
            }
            ImGui::EndCombo();
        }
        bool varying = _variability == SdfVariabilityVarying;
        if (ImGui::Checkbox("Varying", &varying)) {
            _variability = _variability == SdfVariabilityVarying ? SdfVariabilityUniform : SdfVariabilityVarying;
        }
        ImGui::EndDisabled();

        ImGui::Checkbox("Custom attribute", &_custom);
        ImGui::Checkbox("Create default value", &_createDefault);
        draw_ok_cancel_modal(
            [&]() {
                execute_after_draw<PrimCreateAttribute>(_sdfPrim, _attributeName, _typeName, _variability, _custom, _createDefault);
            },
            bool(_attributeName.empty()));
    }

    [[nodiscard]] const char *dialog_id() const override { return "Create attribute"; }

    const SdfPrimSpecHandle &_sdfPrim;
    std::string _attributeName;
    SdfVariability _variability = SdfVariabilityVarying;
    SdfValueTypeName _typeName = SdfValueTypeNames->Bool;
    bool _custom = false;
    bool _createDefault = false;
    bool _useSchema = true;
    std::vector<std::string> _schemaTypeNames;
    std::vector<std::string>::iterator selectedSchemaTypeName = _schemaTypeNames.end();

    std::vector<std::string> _allAttrNames;
    std::vector<std::string>::iterator _selectedAttributeName = _allAttrNames.end();
};

struct CreateRelationDialog : public ModalDialog {
    explicit CreateRelationDialog(const SdfPrimSpecHandle &sdfPrim) : _sdfPrim(sdfPrim), _allSchemaNames(get_all_spec_type_names()) {
        _selectedSchemaName = std::find(_allSchemaNames.begin(), _allSchemaNames.end(), _sdfPrim->GetTypeName().GetString());
        fill_relationship_names();
        update_with_new_prim_definition();
    };
    ~CreateRelationDialog() override = default;

    void fill_relationship_names() {
        if (_selectedSchemaName != _allSchemaNames.end()) {
            _allRelationshipNames = get_prim_spec_property_names(TfToken(*_selectedSchemaName), SdfSpecTypeRelationship);
            _selectedRelationshipName = _allRelationshipNames.begin();
        } else {
            _relationName = "";
        }
    }

    void update_with_new_prim_definition() {
        if (_selectedSchemaName != _allSchemaNames.end() && _selectedRelationshipName != _allRelationshipNames.end()) {
            if (auto *primDefinition =
                    UsdSchemaRegistry::GetInstance().FindConcretePrimDefinition(TfToken(*_selectedSchemaName))) {
                _relationName = *_selectedRelationshipName;
                SdfRelationshipSpecHandle relSpec = primDefinition->GetSchemaRelationshipSpec(TfToken(_relationName));
                _variability = relSpec->GetVariability();
            }
        } else {
            _relationName = "";
        }
    }

    void draw() override {
        ImGui::Checkbox("Find relation in schema", &_useSchema);
        ImGui::BeginDisabled(!_useSchema);
        const char *schemaStr = _selectedSchemaName == _allSchemaNames.end() ? "" : _selectedSchemaName->c_str();
        if (ImGui::BeginCombo("From schema", schemaStr)) {
            for (size_t i = 0; i < _allSchemaNames.size(); i++) {
                if (!_allSchemaNames[i].empty() && ImGui::Selectable(_allSchemaNames[i].c_str(), false)) {
                    _selectedSchemaName = _allSchemaNames.begin() + i;
                    fill_relationship_names();
                    update_with_new_prim_definition();
                }
            }
            ImGui::EndCombo();
        }
        ImGui::EndDisabled();

        if (!_useSchema) {
            ImGui::InputText("Relationship name", &_relationName);
        } else {
            if (ImGui::BeginCombo("Name", _relationName.c_str())) {
                for (size_t i = 0; i < _allRelationshipNames.size(); i++) {
                    if (ImGui::Selectable(_allRelationshipNames[i].c_str(), false)) {
                        _selectedRelationshipName = _allRelationshipNames.begin() + i;
                        update_with_new_prim_definition();
                    }
                }
                ImGui::EndCombo();
            }
        }
        ImGui::InputText("Target path", &_targetPath);
        if (ImGui::BeginCombo("Edit list", get_list_editor_operation_name(_operation))) {
            for (int i = 0; i < get_list_editor_operation_size(); ++i) {
                if (ImGui::Selectable(get_list_editor_operation_name(i), false)) {
                    _operation = static_cast<SdfListOpType>(i);
                }
            }
            ImGui::EndCombo();
        }
        ImGui::BeginDisabled(_useSchema);
        bool varying = _variability == SdfVariabilityVarying;
        if (ImGui::Checkbox("Varying", &varying)) {
            _variability = _variability == SdfVariabilityVarying ? SdfVariabilityUniform : SdfVariabilityVarying;
        }
        ImGui::EndDisabled();
        ImGui::Checkbox("Custom relationship", &_custom);
        draw_ok_cancel_modal(
            [=]() {
                execute_after_draw<PrimCreateRelationship>(_sdfPrim, _relationName, _variability, _custom, _operation, _targetPath);
            },
            _relationName.empty());
    }
    [[nodiscard]] const char *dialog_id() const override { return "Create relationship"; }

    const SdfPrimSpecHandle &_sdfPrim;
    std::string _relationName;
    std::string _targetPath;
    SdfListOpType _operation = SdfListOpTypeExplicit;
    SdfVariability _variability = SdfVariabilityVarying;
    bool _custom = false;
    bool _useSchema = true;
    std::vector<std::string> _allSchemaNames;
    std::vector<std::string>::iterator _selectedSchemaName = _allSchemaNames.end();
    std::vector<std::string> _allRelationshipNames;
    std::vector<std::string>::iterator _selectedRelationshipName = _allRelationshipNames.end();
};

struct CreateVariantModalDialog : public ModalDialog {

    explicit CreateVariantModalDialog(const SdfPrimSpecHandle &primSpec) : _primSpec(primSpec){};
    ~CreateVariantModalDialog() override = default;

    void draw() override {
        if (!_primSpec) {
            close_modal();
            return;
        }
        bool isVariant = _primSpec->GetPath().IsPrimVariantSelectionPath();
        ImGui::InputText("VariantSet name", &_variantSet);
        ImGui::InputText("Variant name", &_variant);
        if (isVariant) {
            ImGui::Checkbox("Add to variant edit list", &_addToEditList);
        }
        //
        if (ImGui::Button("Add")) {
            // TODO The call might not be safe as _primSpec is copied, so create an actual command instead
            std::function<void()> func = [=]() {
                SdfCreateVariantInLayer(_primSpec->GetLayer(), _primSpec->GetPath(), _variantSet, _variant);
                if (isVariant && _addToEditList) {
                    auto ownerPath = _primSpec->GetPath().StripAllVariantSelections();
                    // This won't work on doubly nested variants,
                    // when there is a Prim between the variants
                    auto ownerPrim = _primSpec->GetPrimAtPath(ownerPath);
                    if (ownerPrim) {
                        auto nameList = ownerPrim->GetVariantSetNameList();
                        if (!nameList.ContainsItemEdit(_variantSet)) {
                            ownerPrim->GetVariantSetNameList().Add(_variantSet);
                        }
                    }
                }
            };
            execute_after_draw<UsdFunctionCall>(_primSpec->GetLayer(), func);
        }
        ImGui::SameLine();
        if (ImGui::Button("Close")) {
            close_modal();
        }
    }

    [[nodiscard]] const char *dialog_id() const override { return "Add variant"; }

    SdfPrimSpecHandle _primSpec;
    std::string _variantSet;
    std::string _variant;
    bool _addToEditList = false;
};

/// Very basic ui to create a connection
struct CreateSdfAttributeConnectionDialog : public ModalDialog {
    explicit CreateSdfAttributeConnectionDialog(const SdfAttributeSpecHandle &attribute) : _attribute(attribute){};
    ~CreateSdfAttributeConnectionDialog() override = default;

    void draw() override {
        if (!_attribute)
            return;
        ImGui::Text("Create connection for %s", _attribute->GetPath().GetString().c_str());
        if (ImGui::BeginCombo("Edit list", get_list_editor_operation_name(_operation))) {
            for (int i = 0; i < get_list_editor_operation_size(); ++i) {
                if (ImGui::Selectable(get_list_editor_operation_name(i), false)) {
                    _operation = static_cast<SdfListOpType>(i);
                }
            }
            ImGui::EndCombo();
        }
        ImGui::InputText("Path", &_connectionEndPoint);
        draw_ok_cancel_modal(
            [&]() { execute_after_draw<PrimCreateAttributeConnection>(_attribute, _operation, _connectionEndPoint); });
    }
    [[nodiscard]] const char *dialog_id() const override { return "Attribute connection"; }

    SdfAttributeSpecHandle _attribute;
    std::string _connectionEndPoint;
    SdfListOpType _operation = SdfListOpTypeExplicit;
};

void draw_prim_specifier(const SdfPrimSpecHandle &primSpec, ImGuiComboFlags comboFlags) {
    const SdfSpecifier current = primSpec->GetSpecifier();
    SdfSpecifier selected = current;
    const std::string specifierName = TfEnum::GetDisplayName(current);
    if (ImGui::BeginCombo("Specifier", specifierName.c_str(), comboFlags)) {
        for (int n = SdfSpecifierDef; n < SdfNumSpecifiers; n++) {
            const auto displayed = static_cast<SdfSpecifier>(n);
            const bool isSelected = (current == displayed);
            if (ImGui::Selectable(TfEnum::GetDisplayName(displayed).c_str(), isSelected)) {
                selected = displayed;
                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }
        }

        if (selected != current) {
            execute_after_draw(&SdfPrimSpec::SetSpecifier, primSpec, selected);
        }

        ImGui::EndCombo();
    }
}

void draw_prim_instanceable(const SdfPrimSpecHandle &primSpec) {
    if (!primSpec)
        return;
    bool isInstanceable = primSpec->GetInstanceable();
    if (ImGui::Checkbox("Instanceable", &isInstanceable)) {
        execute_after_draw(&SdfPrimSpec::SetInstanceable, primSpec, isInstanceable);
    }
}

void draw_prim_hidden(const SdfPrimSpecHandle &primSpec) {
    if (!primSpec)
        return;
    bool isHidden = primSpec->GetHidden();
    if (ImGui::Checkbox("Hidden", &isHidden)) {
        execute_after_draw(&SdfPrimSpec::SetHidden, primSpec, isHidden);
    }
}

void draw_prim_active(const SdfPrimSpecHandle &primSpec) {
    if (!primSpec)
        return;
    bool isActive = primSpec->GetActive();
    if (ImGui::Checkbox("Active", &isActive)) {
        // TODO: use CTRL click to clear the checkbox
        execute_after_draw(&SdfPrimSpec::SetActive, primSpec, isActive);
    }
}

void draw_prim_name(const SdfPrimSpecHandle &primSpec) {
    auto nameBuffer = primSpec->GetName();
    ImGui::InputText("Prim Name", &nameBuffer);
    if (ImGui::IsItemDeactivatedAfterEdit()) {
        auto primName = std::string(const_cast<char *>(nameBuffer.data()));
        if (primSpec->CanSetName(primName, nullptr)) {
            execute_after_draw(&SdfPrimSpec::SetName, primSpec, primName, true);
        }
    }
}

void draw_prim_kind(const SdfPrimSpecHandle &primSpec) {
    auto primKind = primSpec->GetKind();
    if (ImGui::BeginCombo("Kind", primKind.GetString().c_str())) {
        for (const auto &kind : KindRegistry::GetAllKinds()) {
            bool isSelected = primKind == kind;
            if (ImGui::Selectable(kind.GetString().c_str(), isSelected)) {
                execute_after_draw(&SdfPrimSpec::SetKind, primSpec, kind);
            }
        }
        ImGui::EndCombo();
    }
}

/// Convert prim class tokens to and from char *
/// The chars are stored in DrawPrimType
static inline const char *class_char_from_token(const TfToken &classToken) {
    return classToken == SdfTokens->AnyTypeToken ? "" : classToken.GetString().c_str();
}

static inline TfToken class_token_from_char(const char *classChar) {
    return strcmp(classChar, "") == 0 ? SdfTokens->AnyTypeToken : TfToken(classChar);
}

/// Draw a prim type name combo
void draw_prim_type(const SdfPrimSpecHandle &primSpec, ImGuiComboFlags comboFlags) {
    const char *currentItem = class_char_from_token(primSpec->GetTypeName());
    const auto &allSpecTypes = get_all_spec_type_names();
    static int selected = 0;

    if (combo_with_filter("Prim Type", currentItem, allSpecTypes, &selected, comboFlags)) {
        const auto newSelection = allSpecTypes[selected].c_str();
        if (primSpec->GetTypeName() != class_token_from_char(newSelection)) {
            execute_after_draw(&SdfPrimSpec::SetTypeName, primSpec, class_token_from_char(newSelection));
        }
    }
}
template<typename T>
static inline void draw_array_editor_button(T attribute) {
    if ((*attribute)->GetDefaultValue().IsArrayValued()) {
        if (ImGui::Button(ICON_FA_LIST)) {
            ExecuteAfterDraw<EditorSelectAttributePath>((*attribute)->GetPath());
        }
        ImGui::SameLine();
    }
}

inline SdfPathEditorProxy get_path_editor_proxy(const SdfSpecHandle &spec, const TfToken &field) {
    return SdfGetPathEditorProxy(spec, field);
}

// This editor is specialized for list editor of tokens in the metadata.
// TODO: The code is really similar to DrawSdfPathListOneLinerEditor and ideally they should be unified.
static void draw_tf_token_list_one_liner_editor(const SdfSpecHandle &spec, const TfToken &field) {
    auto proxy = spec->GetInfo(field).Get<SdfTokenListOp>();
    SdfListOpType currentList = get_edit_list_choice(proxy);

    // Edit list chooser
    draw_edit_list_small_button_selector(currentList, proxy);

    ImGui::SameLine();
    thread_local std::string itemsString;// avoid reallocating for every element at every frame
    itemsString.clear();
    for (const TfToken &item : get_sdf_list_op_items(proxy, currentList)) {
        itemsString.append(item.GetString());
        itemsString.append(" ");// we don't care about the last space, it also helps the user adding a new item
    }

    ImGui::InputText("##EditListOneLineEditor", &itemsString);
    if (ImGui::IsItemDeactivatedAfterEdit()) {
        std::vector<std::string> newList = TfStringSplit(itemsString, " ");
        // TODO: should the following code go in a command ??
        std::function<void()> updateList = [=]() {
            if (spec) {
                auto listOp = spec->GetInfo(field).Get<SdfTokenListOp>();
                SdfTokenListOp::ItemVector editList;
                for (const auto &path : newList) {
                    editList.emplace_back(path);
                }
                set_sdf_list_op_items(listOp, currentList, editList);
                spec->SetInfo(field, VtValue(listOp));
            }
        };
        execute_after_draw<UsdFunctionCall>(spec->GetLayer(), updateList);
    }
}

// This function is specialized for editing list of paths in a line editor
static void draw_sdf_path_list_one_liner_editor(const SdfSpecHandle &spec, const TfToken &field) {
    SdfPathEditorProxy proxy = get_path_editor_proxy(spec, field);
    SdfListOpType currentList = get_edit_list_choice(proxy);

    // Edit listop chooser
    draw_edit_list_small_button_selector(currentList, proxy);

    ImGui::SameLine();
    thread_local std::string itemsString;// avoid reallocating
    itemsString.clear();
    for (const SdfPath &item : get_sdf_list_op_items(proxy, currentList)) {
        itemsString.append(item.GetString());
        itemsString.append(" ");// we don't care about the last space, it also helps the user adding a new item
    }

    ImGui::InputText("##EditListOneLineEditor", &itemsString);
    if (ImGui::IsItemDeactivatedAfterEdit()) {
        std::vector<std::string> newList = TfStringSplit(itemsString, " ");
        // TODO: should the following code go in a command ??
        std::function<void()> updateList = [=]() {
            if (spec) {
                auto editorProxy = get_path_editor_proxy(spec, field);
                auto editList = get_sdf_list_op_items(editorProxy, currentList);
                editList.clear();
                for (const auto &path : newList) {
                    editList.push_back(SdfPath(path));
                }
            }
        };
        execute_after_draw<UsdFunctionCall>(spec->GetLayer(), updateList);
    }
}

static void draw_attribute_row_popup_menu(const SdfAttributeSpecHandle &attribute) {
    if (ImGui::MenuItem(ICON_FA_TRASH " Remove attribute")) {
        SdfPrimSpecHandle primSpec = attribute->GetLayer()->GetPrimAtPath(attribute->GetOwner()->GetPath());
        execute_after_draw(&SdfPrimSpec::RemoveProperty, primSpec, primSpec->GetPropertyAtPath(attribute->GetPath()));
    }
    if (!attribute->HasConnectionPaths() && ImGui::MenuItem(ICON_FA_LINK " Create connection")) {
        draw_modal_dialog<CreateSdfAttributeConnectionDialog>(attribute);
    }
    if (attribute->HasConnectionPaths() && ImGui::MenuItem(ICON_FA_LINK " Clear connection paths")) {
        execute_after_draw(&SdfAttributeSpec::ClearConnectionPaths, attribute);
    }

    // Only if there are no default
    if (!attribute->HasDefaultValue() && ImGui::MenuItem(ICON_FA_PLUS " Create default value")) {
        std::function<void()> createDefaultValue = [=]() {
            if (attribute) {
                auto defaultValue = attribute->GetTypeName().GetDefaultValue();
                attribute->SetDefaultValue(defaultValue);
            }
        };
        execute_after_draw<UsdFunctionCall>(attribute->GetLayer(), createDefaultValue);
    }
    if (attribute->HasDefaultValue() && ImGui::MenuItem(ICON_FA_TRASH " Clear default value")) {
        execute_after_draw(&SdfAttributeSpec::ClearDefaultValue, attribute);
    }

    if (ImGui::MenuItem(ICON_FA_KEY " Add key value")) {
        draw_time_sample_creation_dialog(attribute->GetLayer(), attribute->GetPath());
    }

    // TODO: delete keys if it has keys

    if (ImGui::MenuItem(ICON_FA_COPY " Copy property")) {
        execute_after_draw<PropertyCopy>(static_cast<SdfPropertySpecHandle>(attribute));
    }

    if (ImGui::MenuItem(ICON_FA_COPY " Copy property name")) {
        ImGui::SetClipboardText(attribute->GetName().c_str());
    }

    if (ImGui::MenuItem(ICON_FA_COPY " Copy property path")) {
        ImGui::SetClipboardText(attribute->GetPath().GetString().c_str());
    }
}

struct AttributeRow {};
// TODO:
// ICON_FA_EDIT could be great for edit target
// ICON_FA_DRAW_POLYGON great for meshes
template<>
inline ScopedStyleColor get_row_style<AttributeRow>(const int rowId, const SdfAttributeSpecHandle &attribute,
                                                    const Selection &selection, const int &showWhat) {
    const bool selected = selection.is_selected(attribute);
    ImVec4 colorSelected = selected ? ImVec4(ColorAttributeSelectedBg) : ImVec4(0.75, 0.60, 0.33, 0.2);
    return ScopedStyleColor(ImGuiCol_HeaderHovered, selected ? colorSelected : ImVec4(ColorTransparent), ImGuiCol_HeaderActive,
                            ImVec4(ColorTransparent), ImGuiCol_Header, colorSelected, ImGuiCol_Text,
                            ImVec4(ColorAttributeAuthored), ImGuiCol_Button, ImVec4(ColorTransparent), ImGuiCol_FrameBg,
                            ImVec4(ColorEditableWidgetBg));
}

template<>
inline void draw_first_column<AttributeRow>(const int rowId, const SdfAttributeSpecHandle &attribute, const Selection &selection,
                                            const int &showWhat) {
    ImGui::PushID(rowId);
    if (ImGui::Button(ICON_FA_TRASH)) {
        SdfPrimSpecHandle primSpec = attribute->GetLayer()->GetPrimAtPath(attribute->GetOwner()->GetPath());
        execute_after_draw(&SdfPrimSpec::RemoveProperty, primSpec, primSpec->GetPropertyAtPath(attribute->GetPath()));
    }
    ImGui::PopID();
};

template<>
inline void draw_second_column<AttributeRow>(const int rowId, const SdfAttributeSpecHandle &attribute, const Selection &selection,
                                             const int &showWhat) {
    // Still not sure we want to show the type at all or in the same column as the name
    ImGui::PushStyleColor(ImGuiCol_Text, attribute->HasDefaultValue() ? ImVec4(ColorAttributeAuthored) : ImVec4(ColorAttributeUnauthored));
    ImGui::Text(ICON_FA_FLASK);
    ImGui::PopStyleColor();
    ImGui::SameLine();
    const bool hasTimeSamples = attribute->GetLayer()->GetNumTimeSamplesForPath(attribute->GetPath()) != 0;
    ImGui::PushStyleColor(ImGuiCol_Text, hasTimeSamples ? ImVec4(ColorAttributeAuthored) : ImVec4(ColorAttributeUnauthored));
    ImGui::Text(ICON_FA_KEY);
    ImGui::PopStyleColor();
    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Text, attribute->HasConnectionPaths() ? ImVec4(ColorAttributeAuthored) : ImVec4(ColorAttributeUnauthored));
    ImGui::Text(ICON_FA_LINK);
    ImGui::PopStyleColor();
    ImGui::SameLine();
    const std::string attributeText(attribute->GetName() + " (" + attribute->GetTypeName().GetAsToken().GetString() + ")");
    ImGui::Text("%s", attributeText.c_str());
    if (ImGui::BeginPopupContextItem(attributeText.c_str())) {
        draw_attribute_row_popup_menu(attribute);
        ImGui::EndPopup();
    }
};

template<>
inline void draw_third_column<AttributeRow>(const int rowId, const SdfAttributeSpecHandle &attribute, const Selection &selection,
                                            const int &showWhat) {
    // Check what to show, this could be stored in a variable ... check imgui
    // For the mini buttons: ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.f, 0.f));
    ImGui::PushID(rowId);
    bool selected = selection.is_selected(attribute);
    if (ImGui::Selectable("##select", selected, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowItemOverlap)) {
        execute_after_draw<EditorSelectAttributePath>(attribute->GetPath());
    }
    ImGui::SameLine();
    if (attribute->HasDefaultValue()) {
        // Show Default value if any
        ImGui::PushItemWidth(-FLT_MIN);
        VtValue modified = draw_vt_value("##Default", attribute->GetDefaultValue());
        if (modified != VtValue()) {
            execute_after_draw(&SdfPropertySpec::SetDefaultValue, attribute, modified);
        }
    }

    if (attribute->HasConnectionPaths()) {
        ScopedStyleColor connectionColor(ImGuiCol_Text, ImVec4(ColorAttributeConnection));
        SdfConnectionsProxy connections = attribute->GetConnectionPathList();
        draw_sdf_path_list_one_liner_editor(attribute, SdfFieldKeys->ConnectionPaths);
    }
    ImGui::PopID();
};

void draw_prim_spec_attributes(const SdfPrimSpecHandle &primSpec, const Selection &selection) {
    if (!primSpec)
        return;

    const auto &attributes = primSpec->GetAttributes();
    if (attributes.empty())
        return;
    if (ImGui::CollapsingHeader("Attributes", ImGuiTreeNodeFlags_DefaultOpen)) {
        int rowId = 0;
        if (begin_three_columns_table("##DrawPrimSpecAttributes")) {
            setup_three_columns_table(false, "", "Attribute", "");
            ImGui::PushID(primSpec->GetPath().GetHash());
            // the third column allows to show different attribute properties:
            // Default value, keyed values or connections (and summary ??)
            // int showWhat = DrawValueColumnSelector();
            int showWhat = 0;
            for (const SdfAttributeSpecHandle &attribute : attributes) {
                draw_three_columns_row<AttributeRow>(rowId++, attribute, selection, showWhat);
            }
            ImGui::PopID();
            ImGui::EndTable();
        }
    }
}

struct RelationRow {};
template<>
inline void draw_first_column<RelationRow>(const int rowId, const SdfPrimSpecHandle &primSpec,
                                           const SdfRelationshipSpecHandle &relation) {
    if (ImGui::Button(ICON_FA_TRASH)) {
        execute_after_draw(&SdfPrimSpec::RemoveProperty, primSpec, primSpec->GetPropertyAtPath(relation->GetPath()));
    }
};
template<>
inline void draw_second_column<RelationRow>(const int rowId, const SdfPrimSpecHandle &primSpec,
                                            const SdfRelationshipSpecHandle &relation) {
    ImGui::Text("%s", relation->GetName().c_str());
};
template<>
inline void draw_third_column<RelationRow>(const int rowId, const SdfPrimSpecHandle &primSpec,
                                           const SdfRelationshipSpecHandle &relation) {
    ImGui::PushItemWidth(-FLT_MIN);
    draw_sdf_path_list_one_liner_editor(relation, SdfFieldKeys->TargetPaths);
};

void draw_prim_spec_relations(const SdfPrimSpecHandle &primSpec) {
    if (!primSpec)
        return;
    const auto &relationships = primSpec->GetRelationships();
    if (relationships.empty())
        return;
    if (ImGui::CollapsingHeader("Relations", ImGuiTreeNodeFlags_DefaultOpen)) {
        int rowId = 0;
        if (begin_three_columns_table("##DrawPrimSpecRelations")) {
            setup_three_columns_table(false, "", "Relations", "");
            auto relations = primSpec->GetRelationships();
            for (const SdfRelationshipSpecHandle &relation : relations) {
                ImGui::PushID(relation->GetPath().GetHash());
                draw_three_columns_row<RelationRow>(rowId++, primSpec, relation);
                ImGui::PopID();
            }
            ImGui::EndTable();
        }
    }
}

struct ApiSchemaRow {};
template<>
inline bool has_edits<ApiSchemaRow>(const SdfPrimSpecHandle &prim) { return prim->HasInfo(UsdTokens->apiSchemas); }
template<>
inline void draw_first_column<ApiSchemaRow>(const int rowId, const SdfPrimSpecHandle &primSpec) {
    ImGui::PushID(rowId);
    if (ImGui::Button(ICON_FA_TRASH)) {
        execute_after_draw(&SdfPrimSpec::ClearInfo, primSpec, UsdTokens->apiSchemas);
    }
    ImGui::PopID();
};
template<>
inline void draw_second_column<ApiSchemaRow>(const int rowId, const SdfPrimSpecHandle &primSpec) {
    ImGui::Text("API Schemas");
};
template<>
inline void draw_third_column<ApiSchemaRow>(const int rowId, const SdfPrimSpecHandle &primSpec) {
    ImGui::PushItemWidth(-FLT_MIN);
    draw_tf_token_list_one_liner_editor(primSpec, UsdTokens->apiSchemas);
};

#define GENERATE_FIELD(ClassName_, FieldName_, DrawFunction_)                                       \
    struct ClassName_ {                                                                             \
        static constexpr const char *fieldName = FieldName_;                                        \
    };                                                                                              \
    template<>                                                                                      \
    inline void draw_third_column<ClassName_>(const int rowId, const SdfPrimSpecHandle &primSpec) { \
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);                                  \
        DrawFunction_(primSpec);                                                                    \
    }

#define GENERATE_FIELD_WITH_BUTTON(ClassName_, Token_, FieldName_, DrawFunction_)                       \
    GENERATE_FIELD(ClassName_, FieldName_, DrawFunction_);                                              \
    template<>                                                                                          \
    inline bool has_edits<ClassName_>(const SdfPrimSpecHandle &prim) { return prim->HasField(Token_); } \
    template<>                                                                                          \
    inline void draw_first_column<ClassName_>(const int rowId, const SdfPrimSpecHandle &primSpec) {     \
        ImGui::PushID(rowId);                                                                           \
        if (ImGui::Button(ICON_FA_TRASH) && has_edits<ClassName_>(primSpec)) {                          \
            execute_after_draw(&SdfPrimSpec::ClearField, primSpec, Token_);                             \
        }                                                                                               \
        ImGui::PopID();                                                                                 \
    }

GENERATE_FIELD(Specifier, "Specifier", draw_prim_specifier);
GENERATE_FIELD(PrimType, "Type", draw_prim_type);
GENERATE_FIELD(PrimName, "Name", draw_prim_name);
GENERATE_FIELD_WITH_BUTTON(PrimKind, SdfFieldKeys->Kind, "Kind", draw_prim_kind);
GENERATE_FIELD_WITH_BUTTON(PrimActive, SdfFieldKeys->Active, "Active", draw_prim_active);
GENERATE_FIELD_WITH_BUTTON(PrimInstanceable, SdfFieldKeys->Instanceable, "Instanceable", draw_prim_instanceable);
GENERATE_FIELD_WITH_BUTTON(PrimHidden, SdfFieldKeys->Hidden, "Hidden", draw_prim_hidden);

void draw_prim_spec_metadata(const SdfPrimSpecHandle &primSpec) {
    if (!primSpec->GetPath().IsPrimVariantSelectionPath()) {
        if (ImGui::CollapsingHeader("Core Metadata", ImGuiTreeNodeFlags_DefaultOpen)) {
            int rowId = 0;
            if (begin_three_columns_table("##DrawPrimSpecMetadata")) {
                setup_three_columns_table(false, "", "Metadata", "Value");
                ImGui::PushID(primSpec->GetPath().GetHash());
                draw_three_columns_row<Specifier>(rowId++, primSpec);
                draw_three_columns_row<PrimType>(rowId++, primSpec);
                draw_three_columns_row<PrimName>(rowId++, primSpec);
                draw_three_columns_row<PrimKind>(rowId++, primSpec);
                draw_three_columns_row<PrimActive>(rowId++, primSpec);
                draw_three_columns_row<PrimInstanceable>(rowId++, primSpec);
                draw_three_columns_row<PrimHidden>(rowId++, primSpec);
                draw_three_columns_row<ApiSchemaRow>(rowId++, primSpec);
                draw_three_columns_dictionary_editor<SdfPrimSpec>(rowId, primSpec, SdfFieldKeys->CustomData);
                draw_three_columns_dictionary_editor<SdfPrimSpec>(rowId, primSpec, SdfFieldKeys->AssetInfo);
                ImGui::PopID();
                end_three_columns_table();
            }
            ImGui::Separator();
        }
    }
}

void DrawPrimCreateCompositionMenu(const SdfPrimSpecHandle &primSpec) {
    if (primSpec) {
        if (ImGui::MenuItem("Reference")) {
            draw_prim_create_reference(primSpec);
        }
        if (ImGui::MenuItem("Payload")) {
            draw_prim_create_payload(primSpec);
        }
        if (ImGui::MenuItem("Inherit")) {
            draw_prim_create_inherit(primSpec);
        }
        if (ImGui::MenuItem("Specialize")) {
            draw_prim_create_specialize(primSpec);
        }
        if (ImGui::MenuItem("Variant")) {
            draw_modal_dialog<CreateVariantModalDialog>(primSpec);
        }
    }
}

void DrawPrimCreatePropertyMenu(const SdfPrimSpecHandle &primSpec) {
    if (primSpec) {
        if (ImGui::MenuItem("Attribute")) {
            draw_modal_dialog<CreateAttributeDialog>(primSpec);
        }
        if (ImGui::MenuItem("Relation")) {
            draw_modal_dialog<CreateRelationDialog>(primSpec);
        }
    }
}

void draw_sdf_prim_editor_menu_bar(const SdfPrimSpecHandle &primSpec) {
    bool enabled = primSpec;
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("New", enabled)) {
            DrawPrimCreateCompositionMenu(primSpec);
            ImGui::Separator();
            DrawPrimCreatePropertyMenu(primSpec);// attributes and relation
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit", enabled)) {
            if (ImGui::MenuItem("Paste")) {
                execute_after_draw<PropertyPaste>(primSpec);
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
}

void draw_sdf_prim_editor(const SdfPrimSpecHandle &primSpec, const Selection &selection) {
    if (!primSpec)
        return;
    auto headerSize = ImGui::GetWindowSize();
    headerSize.y = TableRowDefaultHeight * 3;// 3 fields in the header
    headerSize.x = -FLT_MIN;
    ImGui::BeginChild("##LayerHeader", headerSize);
    draw_sdf_layer_identity(primSpec->GetLayer(), primSpec->GetPath());// TODO rename to DrawUsdObjectInfo()
    ImGui::EndChild();
    ImGui::Separator();
    ImGui::BeginChild("##LayerBody");
    draw_prim_spec_metadata(primSpec);
    draw_prim_compositions(primSpec);
    draw_prim_variants(primSpec);
    draw_prim_spec_attributes(primSpec, selection);
    draw_prim_spec_relations(primSpec);
    ImGui::EndChild();
    if (ImGui::IsItemHovered()) {
        const SdfPath &selectedProperty = selection.get_anchor_property_path(primSpec->GetLayer());
        if (selectedProperty != SdfPath()) {
            SdfPropertySpecHandle selectedPropertySpec = primSpec->GetLayer()->GetPropertyAtPath(selectedProperty);
            add_shortcut<PropertyCopy, ImGuiKey_LeftCtrl, ImGuiKey_C>(selectedPropertySpec);
        }
        add_shortcut<PropertyPaste, ImGuiKey_LeftCtrl, ImGuiKey_V>(primSpec);
    }
}

}// namespace vox