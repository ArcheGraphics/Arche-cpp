//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <utility>
#include <pxr/usd/usd/prim.h>
#include <pxr/usd/usd/variantSets.h>
#include <pxr/usd/usd/primCompositionQuery.h>
#include <pxr/usd/usdGeom/xformCommonAPI.h>
#include <pxr/usd/pcp/node.h>
#include <pxr/usd/pcp/layerStack.h>
#include <pxr/usd/usdShade/materialBindingAPI.h>
#include <imgui.h>
#include <imgui_stdlib.h>
#include "usd_prim_editor.h"
#include "vt_value_editor.h"
#include "commands/commands.h"
#include "modal_dialogs.h"
#include "base/imgui_helpers.h"
#include "table_layouts.h"
#include "base/constants.h"

PXR_NAMESPACE_USING_DIRECTIVE

namespace vox {
/// Very basic ui to create a connection
struct CreateConnectionDialog : public ModalDialog {
    explicit CreateConnectionDialog(UsdAttribute attribute) : _attribute(std::move(attribute)){};
    ~CreateConnectionDialog() override = default;

    void draw() override {
        ImGui::Text("Create connection for %s", _attribute.GetPath().GetString().c_str());
        ImGui::InputText("Path", &_connectionEndPoint);
        draw_ok_cancel_modal([&]() {
            execute_after_draw(&UsdAttribute::AddConnection, _attribute, SdfPath(_connectionEndPoint),
                               UsdListPositionBackOfPrependList);
        });
    }
    const char *dialog_id() const override { return "Attribute connection"; }

    UsdAttribute _attribute;
    std::string _connectionEndPoint;
};

/// Select and draw the appropriate editor depending on the type, metada and so on.
/// Returns the modified value or VtValue
static VtValue draw_attribute_value(const std::string &label, UsdAttribute &attribute, const VtValue &value) {
    // If the attribute is a TfToken, it might have an "allowedTokens" metadata
    // We assume that the attribute is a token if it has allowedToken, but that might not hold true
    VtValue allowedTokens;
    attribute.GetMetadata(TfToken("allowedTokens"), &allowedTokens);
    if (!allowedTokens.IsEmpty()) {
        return draw_tf_token(label, value, allowedTokens);
    }
    //
    if (attribute.GetRoleName() == TfToken("Color")) {
        // TODO: color values can be "dragged" they should be stored between
        // BeginEdition/EndEdition
        // It needs some refactoring to know when the widgets starts and stop edition
        return draw_color_value(label, value);
    }
    return draw_vt_value(label, value);
}

template<typename PropertyT>
static std::string get_display_name(const PropertyT &property) {
    return property.GetNamespace().GetString() + (property.GetNamespace() == TfToken() ? std::string() : std::string(":")) +
           property.GetBaseName().GetString();
}

void draw_attribute_type_info(const UsdAttribute &attribute) {
    auto attributeTypeName = attribute.GetTypeName();
    auto attributeRoleName = attribute.GetRoleName();
    ImGui::Text("%s(%s)", attributeRoleName.GetString().c_str(), attributeTypeName.GetAsToken().GetString().c_str());
}

// Right align is not used at the moment, but keeping the code as this is useful for quick layout testing
inline void right_align_next_item(const char *str) {
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetColumnWidth() - ImGui::CalcTextSize(str).x - ImGui::GetScrollX() -
                         2 * ImGui::GetStyle().ItemSpacing.x);
}

void draw_attribute_display_name(const UsdAttribute &attribute) {
    const std::string displayName = get_display_name(attribute);
    ImGui::Text("%s", displayName.c_str());
}

void draw_attribute_value_at_time(UsdAttribute &attribute, UsdTimeCode currentTime) {
    const std::string attributeLabel = get_display_name(attribute);
    VtValue value;
    // TODO: On the lower spec mac, this call appears to be really slow with some attributes
    //       AttributeQuery could be a solution but doesn't update with external data updates
    const bool HasValue = attribute.Get(&value, currentTime);

    if (HasValue) {
        VtValue modified = draw_attribute_value(attributeLabel, attribute, value);
        if (!modified.IsEmpty()) {
            execute_after_draw<AttributeSet>(attribute, modified,
                                             attribute.GetNumTimeSamples() ? currentTime : UsdTimeCode::Default());
        }
    }

    const bool HasConnections = attribute.HasAuthoredConnections();
    if (HasConnections) {
        SdfPathVector sources;
        attribute.GetConnections(&sources);
        for (auto &connection : sources) {
            ImGui::PushID(connection.GetString().c_str());
            if (ImGui::Button(ICON_FA_TRASH)) {
                execute_after_draw(&UsdAttribute::RemoveConnection, attribute, connection);
            }
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(ColorAttributeConnection), ICON_FA_LINK " %s", connection.GetString().c_str());
            ImGui::PopID();
        }
    }

    if (!HasValue && !HasConnections) {
        ImGui::TextColored(ImVec4({0.5, 0.5, 0.5, 0.5}), "no value");
    }
}

void draw_usd_relationship_display_name(const UsdRelationship &relationship) {
    std::string relationshipName = get_display_name(relationship);
    ImVec4 attributeNameColor = relationship.IsAuthored() ? ImVec4(ColorAttributeAuthored) : ImVec4(ColorAttributeUnauthored);
    ImGui::TextColored(ImVec4(attributeNameColor), "%s", relationshipName.c_str());
}

void draw_usd_relationship_list(const UsdRelationship &relationship) {
    SdfPathVector targets;
    // relationship.GetForwardedTargets(&targets);
    // for (const auto &path : targets) {
    //    ImGui::TextColored(ImVec4(AttributeRelationshipColor), "%s", path.GetString().c_str());
    //}
    relationship.GetTargets(&targets);
    if (!targets.empty()) {
        if (ImGui::BeginListBox("##Relationship", ImVec2(-FLT_MIN, targets.size() * 25))) {
            for (const auto &path : targets) {
                ImGui::PushID(path.GetString().c_str());
                if (ImGui::Button(ICON_FA_TRASH)) {
                    execute_after_draw(&UsdRelationship::RemoveTarget, relationship, path);
                }
                ImGui::SameLine();
                std::string buffer = path.GetString();
                ImGui::InputText("##EditRelation", &buffer);
                ImGui::PopID();
            }
            ImGui::EndListBox();
        }
    }
}

void draw_property_arcs(const UsdProperty &property, UsdTimeCode currentTime) {
    SdfPropertySpecHandleVector properties = property.GetPropertyStack(currentTime);
    for (const auto &prop : properties) {
        if (ImGui::MenuItem(prop.GetSpec().GetPath().GetText())) {
            execute_after_draw<EditorSelectAttributePath>(prop.GetSpec().GetPath());
        }
    }
}

/// Specialization for DrawPropertyMiniButton, between UsdAttribute and UsdRelashionship
template<typename UsdPropertyT>
const char *small_button_label();
template<>
const char *small_button_label<UsdAttribute>() { return "(a)"; }
template<>
const char *small_button_label<UsdRelationship>() { return "(r)"; }

template<typename UsdPropertyT>
void draw_menu_clear_authored_values(UsdPropertyT &property) {}
template<>
void draw_menu_clear_authored_values(UsdAttribute &attribute) {
    if (attribute.IsAuthored()) {
        if (ImGui::MenuItem(ICON_FA_EJECT " Clear values")) {
            execute_after_draw(&UsdAttribute::Clear, attribute);
        }
    }
}

template<typename UsdPropertyT>
void draw_menu_block_values(UsdPropertyT &property) {}
template<>
void draw_menu_block_values(UsdAttribute &attribute) {
    if (ImGui::MenuItem(ICON_FA_STOP " Block values")) {
        execute_after_draw(&UsdAttribute::Block, attribute);
    }
}

template<typename UsdPropertyT>
void draw_menu_remove_property(UsdPropertyT &property) {}
template<>
void draw_menu_remove_property(UsdAttribute &attribute) {
    if (ImGui::MenuItem(ICON_FA_TRASH " Remove property")) {
        execute_after_draw(&UsdPrim::RemoveProperty, attribute.GetPrim(), attribute.GetName());
    }
}

template<typename UsdPropertyT>
void draw_menu_set_key(UsdPropertyT &property, UsdTimeCode currentTime) {}
template<>
void draw_menu_set_key(UsdAttribute &attribute, UsdTimeCode currentTime) {
    if (attribute.GetVariability() == SdfVariabilityVarying && attribute.HasValue() && ImGui::MenuItem(ICON_FA_KEY " Set key")) {
        VtValue value;
        attribute.Get(&value, currentTime);
        execute_after_draw<AttributeSet>(attribute, value, currentTime);
    }
}

// TODO Share the code,
static void draw_property_mini_button(const char *btnStr, const ImVec4 &btnColor = ImVec4(ColorMiniButtonUnauthored)) {
    ScopedStyleColor miniButtonStyle(ImGuiCol_Text, btnColor, ImGuiCol_Button, ImVec4(ColorTransparent));
    ImGui::SmallButton(btnStr);
}

template<typename UsdPropertyT>
void draw_menu_edit_connection(UsdPropertyT &property) {}
template<>
void draw_menu_edit_connection(UsdAttribute &attribute) {
    if (ImGui::MenuItem(ICON_FA_LINK " Create connection")) {
        draw_modal_dialog<CreateConnectionDialog>(attribute);
    }
}

// TODO: relationship
template<typename UsdPropertyT>
void draw_menu_create_value(UsdPropertyT &property) {}

template<>
void draw_menu_create_value(UsdAttribute &attribute) {
    if (!attribute.HasValue()) {
        if (ImGui::MenuItem(ICON_FA_DONATE " Create value")) {
            execute_after_draw<AttributeCreateDefaultValue>(attribute);
        }
    }
}

// Property mini button, should work with UsdProperty, UsdAttribute and UsdRelationShip
template<typename UsdPropertyT>
void draw_property_mini_button(UsdPropertyT &property, const UsdEditTarget &editTarget, UsdTimeCode currentTime) {
    ImVec4 propertyColor =
        property.IsAuthoredAt(editTarget) ? ImVec4(ColorMiniButtonAuthored) : ImVec4(ColorMiniButtonUnauthored);
    draw_property_mini_button(small_button_label<UsdPropertyT>(), propertyColor);
    if (ImGui::BeginPopupContextItem(nullptr, ImGuiPopupFlags_MouseButtonLeft)) {
        draw_menu_set_key(property, currentTime);
        draw_menu_create_value(property);
        draw_menu_clear_authored_values(property);
        draw_menu_block_values(property);
        draw_menu_remove_property(property);
        draw_menu_edit_connection(property);
        if (ImGui::MenuItem(ICON_FA_COPY " Copy attribute path")) {
            ImGui::SetClipboardText(property.GetPath().GetString().c_str());
        }
        if (ImGui::BeginMenu(ICON_FA_HAND_HOLDING_USD " Select SdfAttribute")) {
            draw_property_arcs(property, currentTime);
            ImGui::EndMenu();
        }
        ImGui::EndPopup();
    }
}

bool draw_variant_sets_combos(UsdPrim &prim) {
    int buttonID = 0;
    if (!prim.HasVariantSets())
        return false;
    auto variantSets = prim.GetVariantSets();

    if (ImGui::BeginTable("##DrawVariantSetsCombos", 3, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg)) {
        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 24);// 24 => size of the mini button
        ImGui::TableSetupColumn("VariantSet");
        ImGui::TableSetupColumn("");

        ImGui::TableHeadersRow();

        for (const auto &variantSetName : variantSets.GetNames()) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);

            // Variant set mini button --- TODO move code from this function
            auto variantSet = variantSets.GetVariantSet(variantSetName);
            // TODO: how do we figure out if the variant set has been edited in this edit target ?
            // Otherwise after a "Clear variant selection" the button remains green and it visually looks like it did nothing
            ImVec4 variantColor =
                variantSet.HasAuthoredVariantSelection() ? ImVec4(ColorMiniButtonAuthored) : ImVec4(ColorMiniButtonUnauthored);
            ImGui::PushID(buttonID++);
            draw_property_mini_button("(v)", variantColor);
            ImGui::PopID();
            if (ImGui::BeginPopupContextItem(nullptr, ImGuiPopupFlags_MouseButtonLeft)) {
                if (ImGui::MenuItem("Clear variant selection")) {
                    execute_after_draw(&UsdVariantSet::ClearVariantSelection, variantSet);
                }
                ImGui::EndPopup();
            }

            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%s", variantSetName.c_str());

            ImGui::TableSetColumnIndex(2);
            ImGui::PushItemWidth(-FLT_MIN);
            if (ImGui::BeginCombo(variantSetName.c_str(), variantSet.GetVariantSelection().c_str())) {
                for (const auto &variant : variantSet.GetVariantNames()) {
                    if (ImGui::Selectable(variant.c_str(), false)) {
                        execute_after_draw(&UsdVariantSet::SetVariantSelection, variantSet, variant);
                    }
                }
                ImGui::EndCombo();
            }
            ImGui::PopItemWidth();
        }
        ImGui::EndTable();
    }
    return true;
}

bool draw_asset_info(UsdPrim &prim) {
    auto assetInfo = prim.GetAssetInfo();
    if (assetInfo.empty())
        return false;

    if (ImGui::BeginTable("##DrawAssetInfo", 3, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg)) {
        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 24);// 24 => size of the mini button
        ImGui::TableSetupColumn("Asset info");
        ImGui::TableSetupColumn("");

        ImGui::TableHeadersRow();

        TF_FOR_ALL(keyValue, assetInfo) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            draw_property_mini_button("(x)");
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%s", keyValue->first.c_str());
            ImGui::TableSetColumnIndex(2);
            ImGui::PushItemWidth(-FLT_MIN);
            VtValue modified = draw_vt_value(keyValue->first, keyValue->second);
            if (!modified.IsEmpty()) {
                execute_after_draw(&UsdPrim::SetAssetInfoByKey, prim, TfToken(keyValue->first), modified);
            }
            ImGui::PopItemWidth();
        }
        ImGui::EndTable();
    }
    return true;
}

// WIP
bool IsMetadataShown(int options) { return true; }
bool IsTransformShown(int options) { return false; }

void DrawPropertyEditorMenuBar(UsdPrim &prim, int options) {

    if (ImGui::BeginMenuBar()) {
        if (prim && ImGui::BeginMenu("Create")) {
            // TODO: list all the attribute missing or incomplete
            if (ImGui::MenuItem("Attribute", nullptr)) {
            }
            if (ImGui::MenuItem("Reference", nullptr)) {
            }
            if (ImGui::MenuItem("Relation", nullptr)) {
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Show")) {
            if (ImGui::MenuItem("Metadata", nullptr, IsMetadataShown(options))) {
            }
            if (ImGui::MenuItem("Transform", nullptr, IsTransformShown(options))) {
            }
            if (ImGui::MenuItem("Attributes", nullptr, false)) {
            }
            if (ImGui::MenuItem("Relations", nullptr, false)) {
            }
            if (ImGui::MenuItem("Composition", nullptr, false)) {
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
}

/// Draws a menu list of all the sublayers, indented to reveal the parenting
static void draw_edit_target_sub_layers_menu_items(const UsdStageWeakPtr &stage, const SdfLayerHandle &layer, int indent = 0) {
    if (layer) {
        std::vector<std::string> subLayers = layer->GetSubLayerPaths();
        for (const auto &subLayerPath : subLayers) {
            auto subLayer = SdfLayer::FindOrOpenRelativeToLayer(layer, subLayerPath);
            if (!subLayer) {
                subLayer = SdfLayer::FindOrOpen(subLayerPath);
            }
            const std::string layerName = std::string(indent, ' ') + (subLayer ? subLayer->GetDisplayName() : subLayerPath);
            if (subLayer) {
                if (ImGui::MenuItem(layerName.c_str())) {
                    execute_after_draw<EditorSetEditTarget>(stage, UsdEditTarget(subLayer));
                }
            } else {
                ImGui::TextColored(ImVec4(1.0, 0.0, 0.0, 1.0), "%s", layerName.c_str());
            }

            ImGui::PushID(layerName.c_str());
            draw_edit_target_sub_layers_menu_items(stage, subLayer, indent + 4);
            ImGui::PopID();
        }
    }
}

bool draw_material_bindings(const UsdPrim &prim) {
#if (PXR_VERSION < 2208)
    return false;
#else
    if (!prim)
        return false;
    UsdShadeMaterialBindingAPI materialBindingAPI(prim);
    if (ImGui::BeginTable("##DrawPropertyEditorHeader", 3, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg)) {
        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 24);// 24 => size of the mini button
        ImGui::TableSetupColumn("Material Bindings");
        ImGui::TableSetupColumn("");
        ImGui::TableHeadersRow();
        UsdShadeMaterial material;
        for (const auto &purpose : materialBindingAPI.GetMaterialPurposes()) {
            ImGui::TableNextRow(ImGuiTableRowFlags_None, TableRowDefaultHeight);
            ImGui::TableSetColumnIndex(1);
            const std::string &purposeName = purpose.GetString();
            ImGui::Text("%s", purposeName.empty() ? "All purposes" : purposeName.c_str());
            ImGui::TableSetColumnIndex(2);
            material = materialBindingAPI.ComputeBoundMaterial(purpose);
            if (material) {
                ImGui::Text("%s", material.GetPrim().GetPath().GetText());
            } else {
                ImGui::Text("unbound");
            }
        }

        ImGui::EndTable();
        return true;
    }
    return false;
#endif
}

// Second version of an edit target selector
void draw_usd_prim_edit_target(const UsdPrim &prim) {
    if (!prim)
        return;
    ScopedStyleColor defaultStyle(DefaultColorStyle);
    if (ImGui::MenuItem("Session layer")) {
        execute_after_draw<EditorSetEditTarget>(prim.GetStage(), UsdEditTarget(prim.GetStage()->GetSessionLayer()));
    }
    if (ImGui::MenuItem("Root layer")) {
        execute_after_draw<EditorSetEditTarget>(prim.GetStage(), UsdEditTarget(prim.GetStage()->GetRootLayer()));
    }

    if (ImGui::BeginMenu("Sublayers")) {
        const auto &layer = prim.GetStage()->GetRootLayer();
        draw_edit_target_sub_layers_menu_items(prim.GetStage(), layer);
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Selected prim arcs")) {
        using Query = pxr::UsdPrimCompositionQuery;
        auto filter = UsdPrimCompositionQuery::Filter();
        pxr::UsdPrimCompositionQuery arc(prim, filter);
        auto compositionArcs = arc.GetCompositionArcs();
        for (const auto &a : compositionArcs) {
            // NOTE: we can use GetIntroducingLayer() and GetIntroducingPrimPath() to add more information
            if (a.GetTargetNode()) {
                std::string arcName = a.GetTargetNode().GetLayerStack()->GetIdentifier().rootLayer->GetDisplayName() + " " +
                                      a.GetTargetNode().GetPath().GetString();
                if (ImGui::MenuItem(arcName.c_str())) {
                    execute_after_draw<EditorSetEditTarget>(
                        prim.GetStage(),
                        UsdEditTarget(a.GetTargetNode().GetLayerStack()->GetIdentifier().rootLayer, a.GetTargetNode()),
                        a.GetTargetNode().GetPath());
                }
            }
        }
        ImGui::EndMenu();
    }
}

// Testing
// TODO: this is a copied function that needs to be moved in the next release
static ImVec4 get_prim_color(const UsdPrim &prim) {
    if (!prim.IsActive() || !prim.IsLoaded()) {
        return ImVec4(ColorPrimInactive);
    }
    if (prim.IsInstance()) {
        return ImVec4(ColorPrimInstance);
    }
    const auto hasCompositionArcs = prim.HasAuthoredReferences() || prim.HasAuthoredPayloads() || prim.HasAuthoredInherits() ||
                                    prim.HasAuthoredSpecializes() || prim.HasVariantSets();
    if (hasCompositionArcs) {
        return ImVec4(ColorPrimHasComposition);
    }
    if (prim.IsPrototype() || prim.IsInPrototype() || prim.IsInstanceProxy()) {
        return ImVec4(ColorPrimPrototype);
    }
    return ImVec4(ColorPrimDefault);
}

void draw_usd_prim_header(UsdPrim &prim) {
    auto editTarget = prim.GetStage()->GetEditTarget();
    const SdfPath targetPath = editTarget.MapToSpecPath(prim.GetPath());

    if (ImGui::BeginTable("##DrawPropertyEditorHeader", 3, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg)) {
        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 24);// 24 => size of the mini button
        ImGui::TableSetupColumn("Identity");
        ImGui::TableSetupColumn("Value");
        ImGui::TableHeadersRow();
        ImGui::TableNextRow(ImGuiTableRowFlags_None, TableRowDefaultHeight);
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("Stage");
        ImGui::TableSetColumnIndex(2);
        ImGui::Text("%s", prim.GetStage()->GetRootLayer()->GetIdentifier().c_str());

        ImGui::TableNextRow(ImGuiTableRowFlags_None, TableRowDefaultHeight);
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("Path");
        ImGui::TableSetColumnIndex(2);
        {
            ScopedStyleColor pathColor(ImGuiCol_Text, get_prim_color(prim));
            ImGui::Text("%s", prim.GetPrimPath().GetString().c_str());
        }
        ImGui::TableNextRow(ImGuiTableRowFlags_None, TableRowDefaultHeight);
        ImGui::TableSetColumnIndex(0);
        draw_property_mini_button(ICON_FA_PEN);
        ImGuiPopupFlags flags = ImGuiPopupFlags_MouseButtonLeft;
        if (ImGui::BeginPopupContextItem(nullptr, flags)) {
            draw_usd_prim_edit_target(prim);
            ImGui::EndPopup();
        }
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("Edit target");
        ImGui::TableSetColumnIndex(2);
        {
            auto targetPath = editTarget.MapToSpecPath(prim.GetPath());
            ScopedStyleColor color(ImGuiCol_Text,
                                   targetPath == SdfPath() ? ImVec4(1.0, 0.0, 0.0, 1.0) : ImVec4(1.0, 1.0, 1.0, 1.0));
            ImGui::Text("%s %s", editTarget.GetLayer()->GetDisplayName().c_str(), targetPath.GetString().c_str());
        }

        ImGui::TableNextRow(ImGuiTableRowFlags_None, TableRowDefaultHeight);
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("Type");
        ImGui::TableSetColumnIndex(2);
        ImGui::Text("%s", prim.GetTypeName().GetString().c_str());

        ImGui::EndTable();
    }
}

void draw_usd_prim_properties(UsdPrim &prim, UsdTimeCode currentTime) {
    DrawPropertyEditorMenuBar(prim, 0);

    if (prim) {
        auto headerSize = ImGui::GetWindowSize();
        headerSize.y = TableRowDefaultHeight * 5;// 5 rows (4 + header)
        headerSize.x = -FLT_MIN;                 // expand as much as possible
        ImGui::BeginChild("##Header", headerSize);
        draw_usd_prim_header(prim);
        ImGui::EndChild();
        ImGui::Separator();
        ImGui::BeginChild("##Body");
        if (draw_asset_info(prim)) {
            ImGui::Separator();
        }

        if (draw_material_bindings(prim)) {
            ImGui::Separator();
        }

        // Draw variant sets
        if (draw_variant_sets_combos(prim)) {
            ImGui::Separator();
        }

        // Transforms
        if (draw_xforms_common(prim, currentTime)) {
            ImGui::Separator();
        }

        if (ImGui::BeginTable("##DrawPropertyEditorTable", 3, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg)) {
            ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 24);// 24 => size of the mini button
            ImGui::TableSetupColumn("Property name");
            ImGui::TableSetupColumn("Value");
            ImGui::TableHeadersRow();

            int miniButtonId = 0;
            const auto &editTarget = prim.GetStage()->GetEditTarget();

            // Draw attributes
            for (auto &attribute : prim.GetAttributes()) {
                ImGui::TableNextRow(ImGuiTableRowFlags_None, TableRowDefaultHeight);
                ImGui::TableSetColumnIndex(0);
                ImGui::PushID(miniButtonId++);
                draw_property_mini_button(attribute, editTarget, currentTime);
                ImGui::PopID();

                ImGui::TableSetColumnIndex(1);
                draw_attribute_display_name(attribute);

                ImGui::TableSetColumnIndex(2);
                ImGui::PushItemWidth(-FLT_MIN);// Right align and get rid of widget label
                ImGui::PushID(attribute.GetPath().GetHash());
                draw_attribute_value_at_time(attribute, currentTime);
                ImGui::PopID();
                ImGui::PopItemWidth();
                // TODO: in the hint ???
                // DrawAttributeTypeInfo(attribute);
            }

            // Draw relations
            for (auto &relationship : prim.GetRelationships()) {
                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);
                ImGui::PushID(miniButtonId++);
                draw_property_mini_button(relationship, editTarget, currentTime);
                ImGui::PopID();

                ImGui::TableSetColumnIndex(1);
                draw_usd_relationship_display_name(relationship);

                ImGui::TableSetColumnIndex(2);
                draw_usd_relationship_list(relationship);
            }

            ImGui::EndTable();
        }
        ImGui::EndChild();
    }
}

#define GENERATE_XFORMCOMMON_FIELD(OpName_, OpType_, OpRawType_)                                                       \
    struct XformCommon##OpName_##Field {                                                                               \
        static constexpr const char *fieldName = "" #OpName_ "";                                                       \
    };                                                                                                                 \
    template<>                                                                                                         \
    inline void draw_third_column<XformCommon##OpName_##Field>(const int rowId, const UsdGeomXformCommonAPI &xformAPI, \
                                                               const OpType_ &value, const UsdTimeCode &currentTime) { \
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);                                                     \
        OpType_ valueLocal(value[0], value[1], value[2]);                                                              \
        ImGui::InputScalarN("" #OpName_ "", OpRawType_, valueLocal.data(), 3, nullptr, nullptr, DecimalPrecision);     \
        if (ImGui::IsItemDeactivatedAfterEdit()) {                                                                     \
            execute_after_draw(&UsdGeomXformCommonAPI::Set##OpName_, xformAPI, valueLocal, currentTime);               \
        }                                                                                                              \
    }

GENERATE_XFORMCOMMON_FIELD(Translate, GfVec3d, ImGuiDataType_Double)
GENERATE_XFORMCOMMON_FIELD(Scale, GfVec3f, ImGuiDataType_Float)
GENERATE_XFORMCOMMON_FIELD(Pivot, GfVec3f, ImGuiDataType_Float)

struct XformCommonRotateField {
    static constexpr const char *fieldName = "Rotate";
};
template<>
inline void draw_third_column<XformCommonRotateField>(const int rowId, const UsdGeomXformCommonAPI &xformAPI,
                                                      const GfVec3f &rotation, const UsdGeomXformCommonAPI::RotationOrder &rotOrder,
                                                      const UsdTimeCode &currentTime) {
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    GfVec3f rotationf(rotation[0], rotation[1], rotation[2]);
    ImGui::InputScalarN("Rotate", ImGuiDataType_Float, rotationf.data(), 3, nullptr, nullptr, DecimalPrecision);
    if (ImGui::IsItemDeactivatedAfterEdit()) {
        execute_after_draw(&UsdGeomXformCommonAPI::SetRotate, xformAPI, rotationf, rotOrder, currentTime);
    }
}

// Draw a xform common api in a table
// I am not sure this is really useful
bool draw_xforms_common(UsdPrim &prim, UsdTimeCode currentTime) {
    UsdGeomXformCommonAPI xformAPI(prim);

    if (xformAPI) {
        GfVec3d translation{};
        GfVec3f scale{};
        GfVec3f pivot{};
        GfVec3f rotation{};
        UsdGeomXformCommonAPI::RotationOrder rotOrder;
        xformAPI.GetXformVectors(&translation, &rotation, &scale, &pivot, &rotOrder, currentTime);

        int rowId = 0;
        if (begin_three_columns_table("##DrawXformsCommon")) {
            setup_three_columns_table(true, "", "UsdGeomXformCommonAPI", "");
            draw_three_columns_row<XformCommonTranslateField>(rowId++, xformAPI, translation, currentTime);
            draw_three_columns_row<XformCommonRotateField>(rowId++, xformAPI, rotation, rotOrder, currentTime);
            draw_three_columns_row<XformCommonScaleField>(rowId++, xformAPI, scale, currentTime);
            draw_three_columns_row<XformCommonPivotField>(rowId++, xformAPI, pivot, currentTime);
            end_three_columns_table();
        }
        return true;
    }
    return false;
}

}// namespace vox