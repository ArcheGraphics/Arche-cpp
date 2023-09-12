//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <pxr/usd/sdf/variantSpec.h>
#include <pxr/usd/sdf/variantSetSpec.h>
#include "variant_editor.h"
#include "base/imgui_helpers.h"
#include "table_layouts.h"
#include "base/constants.h"
#include "commands/commands.h"
#include "base/usd_helpers.h"

namespace vox {
static void draw_variant_selection_mini_button(const SdfPrimSpecHandle &primSpec, const std::string &variantSetName, int &buttonId) {
    ScopedStyleColor style(ImGuiCol_Text, ImVec4(1.0, 1.0, 1.0, 1.0), ImGuiCol_Button, ImVec4(ColorTransparent));
    ImGui::PushID(buttonId++);
    if (ImGui::SmallButton(ICON_UT_DELETE)) {
        // ExecuteAfterDraw(&SdfPrimSpec::BlockVariantSelection, primSpec); // TODO in 21.02
        execute_after_draw(&SdfPrimSpec::SetVariantSelection, primSpec, variantSetName, "");
    }
    ImGui::PopID();
}

static void draw_variant_selection_combo(const SdfPrimSpecHandle &primSpec, SdfVariantSelectionProxy::const_reference &variantSelection, int &buttonId) {
    ImGui::PushID(buttonId++);
    if (ImGui::BeginCombo("Variant selection", variantSelection.second.c_str())) {
        if (ImGui::Selectable("")) {// Empty variant selection
            execute_after_draw(&SdfPrimSpec::SetVariantSelection, primSpec, variantSelection.first, "");
        }
        for (const auto &variantName : primSpec->GetVariantNames(variantSelection.first)) {
            if (ImGui::Selectable(variantName.c_str())) {
                execute_after_draw(&SdfPrimSpec::SetVariantSelection, primSpec, variantSelection.first, variantName);
            }
        }
        ImGui::EndCombo();
    }
    ImGui::PopID();
}

static void draw_variant_selection(const SdfPrimSpecHandle &primSpec) {
    // TODO: we would like to have a list of potential variant we can select and the variant selected
    auto variantSetNameList = primSpec->GetVariantSetNameList();
    auto variantSelections = primSpec->GetVariantSelections();
    if (!variantSelections.empty()) {
        if (ImGui::BeginTable("##DrawPrimVariantSelections", 3, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg)) {
            table_setup_columns("", "Variant selections", "");
            ImGui::PushID(primSpec->GetPath().GetHash());
            ImGui::TableHeadersRow();
            int buttonId = 0;
            for (const auto &variantSelection : variantSelections) {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                draw_variant_selection_mini_button(primSpec, variantSelection.first, buttonId);

                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%s", variantSelection.first.c_str());

                ImGui::TableSetColumnIndex(2);
                ImGui::PushItemWidth(-FLT_MIN);
                draw_variant_selection_combo(primSpec, variantSelection, buttonId);
                ImGui::PopItemWidth();
            }
            ImGui::PopID();
            ImGui::EndTable();
            ImGui::Separator();
        }
    }
}

struct VariantSetNamesRow {};

#define VariantSetNamesRowArgs const int rowId, const SdfListOpType &operation, const std::string &variantName, const SdfPrimSpecHandle &primSpec

template<>
inline void draw_first_column<VariantSetNamesRow>(VariantSetNamesRowArgs) {
    ImGui::PushID(rowId);
    if (ImGui::Button(ICON_UT_DELETE)) {
        if (!primSpec)
            return;
        std::function<void()> removeVariantSetName = [=]() { primSpec->GetVariantSetNameList().RemoveItemEdits(variantName); };
        execute_after_draw<UsdFunctionCall>(primSpec->GetLayer(), removeVariantSetName);
    }
    ImGui::PopID();
}
template<>
inline void draw_second_column<VariantSetNamesRow>(VariantSetNamesRowArgs) {
    ImGui::Text("%s", get_list_editor_operation_name(operation));
}
template<>
inline void draw_third_column<VariantSetNamesRow>(VariantSetNamesRowArgs) {
    ImGui::Text("%s", variantName.c_str());
}

static void draw_variant_set_names_row(SdfListOpType operation, const std::string &variantName, const SdfPrimSpecHandle &primSpec,
                                       int &rowId) {
    draw_three_columns_row<VariantSetNamesRow>(rowId++, operation, variantName, primSpec);
}

/// Draw the variantSet names edit list
static void draw_variant_set_names(const SdfPrimSpecHandle &primSpec) {
    auto nameList = primSpec->GetVariantSetNameList();
    int rowId = 0;
    if (begin_three_columns_table("##DrawPrimVariantSets")) {
        setup_three_columns_table(false, "", "VariantSet names", "");
        ImGui::PushID(primSpec->GetPath().GetHash());
        iterate_list_editor_items(primSpec->GetVariantSetNameList(), draw_variant_set_names_row, primSpec, rowId);
        ImGui::PopID();
        end_three_columns_table();
    }
}

// Draw a table with the variant selections
void draw_prim_variants(const SdfPrimSpecHandle &primSpec) {
    if (!primSpec)
        return;
    const bool showVariants = !primSpec->GetVariantSelections().empty() || primSpec->HasVariantSetNames();
    if (showVariants && ImGui::CollapsingHeader("Variants", ImGuiTreeNodeFlags_DefaultOpen)) {
        // We can have variant selection even if there is no variantSet on this prim
        // So se draw variant selection AND variantSet names which is the edit list for the
        // visible variant.
        // The actual variant node can be edited in the layer editor
        if (primSpec->HasVariantSetNames()) {
            draw_variant_set_names(primSpec);
        }

        draw_variant_selection(primSpec);
    }
}

}// namespace vox