//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "commands/commands.h"
#include "vt_value_editor.h"
#include <iostream>
#include <imgui_stdlib.h>
#include <pxr/usd/sdf/attributeSpec.h>
#include <pxr/usd/sdf/propertySpec.h>

#include "vt_array_editor.h"
#include "sdf_attribute_editor.h"
#include "sdf_layer_editor.h"
#include "base/imgui_helpers.h"
#include "modal_dialogs.h"
#include "table_layouts.h"
#include "vt_dictionary_editor.h"
#include "vt_value_editor.h"

namespace vox {
/// Dialogs

struct CreateTimeSampleDialog : public ModalDialog {
    CreateTimeSampleDialog(SdfLayerHandle layer, SdfPath attrPath) : _layer(layer), _attrPath(attrPath) {
        auto attr = _layer->GetAttributeAtPath(_attrPath);
        _hasSamples = layer->GetNumTimeSamplesForPath(attrPath) != 0;
        _hasDefault = attr->HasDefaultValue();
        _isArray = attr->GetTypeName().IsArray();
    };
    ~CreateTimeSampleDialog() override {}

    void draw() override {
        ImGui::InputDouble("Time Code", &_timeCode);
        auto attr = _layer->GetAttributeAtPath(_attrPath);
        auto typeName = attr->GetTypeName();
        if (_hasSamples) {
            ImGui::Checkbox("Copy closest value", &_copyClosestValue);
        } else if (_isArray && !_hasDefault) {
            ImGui::InputInt("New array number of element", &_numElements);
        }

        // TODO: check if the key already exists
        draw_ok_cancel_modal([=]() {
            VtValue value = typeName.GetDefaultValue();
            if (_copyClosestValue) {// we are normally sure that there is a least one element
                // This is not really the closest element right now
                auto sampleSet = _layer->ListTimeSamplesForPath(_attrPath);
                auto it = std::lower_bound(sampleSet.begin(), sampleSet.end(), _timeCode);
                if (it == sampleSet.end()) {
                    it--;
                }
                _layer->QueryTimeSample(_attrPath, *it, &value);
            } else if (_isArray && !_hasDefault) {
            }

            execute_after_draw(&SdfLayer::SetTimeSample<VtValue>, _layer, _attrPath, _timeCode, value);
        });
    }
    const char *dialog_id() const override { return "Create TimeSample"; }

    double _timeCode = 0;
    const SdfLayerHandle _layer;
    const SdfPath _attrPath;
    bool _copyClosestValue = false;
    bool _hasSamples = false;
    bool _isArray = false;
    bool _hasDefault = false;
    int _numElements = 0;
};

// we want to keep CreateTimeSampleDialog hidden
void draw_time_sample_creation_dialog(SdfLayerHandle layer, SdfPath attributePath) {
    draw_modal_dialog<CreateTimeSampleDialog>(layer, attributePath);
}

///
/// TypeName
//
struct TypeNameRow {};
template<>
inline void draw_second_column<TypeNameRow>(const int rowId, const SdfAttributeSpecHandle &attr) {
    ImGui::Text("Attribute type");
};
template<>
inline void draw_third_column<TypeNameRow>(const int rowId, const SdfAttributeSpecHandle &attr) {
    ImGui::Text("%s", attr->GetTypeName().GetAsToken().GetText());
};

///
/// Value Type
///
struct ValueTypeRow {};
template<>
inline void draw_second_column<ValueTypeRow>(const int rowId, const SdfAttributeSpecHandle &attr) {
    ImGui::Text("Value type");
};
template<>
inline void draw_third_column<ValueTypeRow>(const int rowId, const SdfAttributeSpecHandle &attr) {
    ImGui::Text("%s", attr->GetValueType().GetTypeName().c_str());
};

///
/// Display Name
///
struct DisplayNameRow {};
template<>
inline bool has_edits<DisplayNameRow>(const SdfAttributeSpecHandle &attr) { return !attr->GetDisplayName().empty(); }
template<>
inline void draw_first_column<DisplayNameRow>(const int rowId, const SdfAttributeSpecHandle &attr) {
    if (ImGui::Button(ICON_FA_TRASH)) {
        execute_after_draw(&SdfAttributeSpec::SetDisplayName, attr, "");
    }
};
template<>
inline void draw_second_column<DisplayNameRow>(const int rowId, const SdfAttributeSpecHandle &attr) {
    ImGui::Text("Display Name");
};
template<>
inline void draw_third_column<DisplayNameRow>(const int rowId, const SdfAttributeSpecHandle &attr) {
    std::string displayName = attr->GetDisplayName();
    ImGui::PushItemWidth(-FLT_MIN);
    ImGui::InputText("##DisplayName", &displayName);
    if (ImGui::IsItemDeactivatedAfterEdit()) {
        execute_after_draw(&SdfAttributeSpec::SetDisplayName, attr, displayName);
    }
};

///
/// Display Group
///
struct DisplayGroupRow {};
template<>
inline bool has_edits<DisplayGroupRow>(const SdfAttributeSpecHandle &attr) { return !attr->GetDisplayGroup().empty(); }
template<>
inline void draw_first_column<DisplayGroupRow>(const int rowId, const SdfAttributeSpecHandle &attr) {
    if (ImGui::Button(ICON_FA_TRASH)) {
        execute_after_draw(&SdfAttributeSpec::SetDisplayGroup, attr, "");
    }
};
template<>
inline void draw_second_column<DisplayGroupRow>(const int rowId, const SdfAttributeSpecHandle &attr) {
    ImGui::Text("Display Group");
};
template<>
inline void draw_third_column<DisplayGroupRow>(const int rowId, const SdfAttributeSpecHandle &attr) {
    std::string displayGroup = attr->GetDisplayGroup();
    ImGui::PushItemWidth(-FLT_MIN);
    ImGui::InputText("##DisplayGroup", &displayGroup);
    if (ImGui::IsItemDeactivatedAfterEdit()) {
        execute_after_draw(&SdfAttributeSpec::SetDisplayGroup, attr, displayGroup);
    }
};

///
/// Variability
///
struct VariabilityRow {};
template<>
inline void draw_second_column<VariabilityRow>(const int rowId, const SdfAttributeSpecHandle &attr) {
    ImGui::Text("Variability");
};
template<>
inline void draw_third_column<VariabilityRow>(const int rowId, const SdfAttributeSpecHandle &attr) {
    int variability = attr->GetVariability();
    ImGui::PushItemWidth(-FLT_MIN);
    const char *variabilityOptions[4] = {"Varying", "Uniform", "Config", "Computed"};// in the doc but not in the code
    if (variability >= 0 && variability < 4) {
        ImGui::Text("%s", variabilityOptions[variability]);
    } else {
        ImGui::Text("Unknown");
    }
};

///
/// Custom property
///
struct CustomRow {};
template<>
inline void draw_second_column<CustomRow>(const int rowId, const SdfAttributeSpecHandle &attr) {
    ImGui::Text("Custom");
};
template<>
inline void draw_third_column<CustomRow>(const int rowId, const SdfAttributeSpecHandle &attr) {
    bool custom = attr->IsCustom();
    ImGui::PushItemWidth(-FLT_MIN);
    ImGui::Checkbox("##Custom", &custom);
    if (ImGui::IsItemDeactivatedAfterEdit()) {
        execute_after_draw(&SdfAttributeSpec::SetCustom, attr, custom);
    }
};

static void DrawSdfAttributeMetadata(SdfAttributeSpecHandle attr) {
    int rowId = 0;
    if (begin_three_columns_table("##DrawSdfAttributeMetadata")) {
        setup_three_columns_table(false, "", "Metadata", "Value");
        draw_three_columns_row<TypeNameRow>(rowId++, attr);
        draw_three_columns_row<ValueTypeRow>(rowId++, attr);
        draw_three_columns_row<VariabilityRow>(rowId++, attr);
        draw_three_columns_row<CustomRow>(rowId++, attr);
        draw_three_columns_row<DisplayNameRow>(rowId++, attr);
        draw_three_columns_row<DisplayGroupRow>(rowId++, attr);
        // DrawThreeColumnsRow<Documentation>(rowId++, attr);
        // DrawThreeColumnsRow<Comment>(rowId++, attr);
        // DrawThreeColumnsRow<PrimHidden>(rowId++, attr);
        // Allowed token
        // Display Unit
        // Color space
        // Role name
        // Hidden
        // Doc
        draw_three_columns_dictionary_editor<SdfAttributeSpec>(rowId, attr, SdfFieldKeys->CustomData);
        draw_three_columns_dictionary_editor<SdfAttributeSpec>(rowId, attr, SdfFieldKeys->AssetInfo);
        end_three_columns_table();
    }
}

#define LEFT_PANE_WIDTH 140

static void draw_time_samples_editor(const SdfAttributeSpecHandle &attr, SdfTimeSampleMap &timeSamples,
                                     UsdTimeCode &selectedKeyframe) {

    if (ImGui::Button(ICON_FA_KEY)) {
        // Create a new key
        draw_time_sample_creation_dialog(attr->GetLayer(), attr->GetPath());
    }
    ImGui::SameLine();
    // TODO: ability to select multiple keyframes and delete them
    if (ImGui::Button(ICON_FA_TRASH)) {
        if (selectedKeyframe == UsdTimeCode::Default()) {
            if (attr->HasDefaultValue()) {
                execute_after_draw(&SdfAttributeSpec::ClearDefaultValue, attr);
            }
        } else {
            execute_after_draw(&SdfLayer::EraseTimeSample, attr->GetLayer(), attr->GetPath(), selectedKeyframe.GetValue());
        }
    }

    if (ImGui::BeginListBox("##Time", ImVec2(LEFT_PANE_WIDTH - 20, -FLT_MIN))) {// -20 to account for the scroll bar
        if (attr->HasDefaultValue()) {
            if (ImGui::Selectable("Default", selectedKeyframe == UsdTimeCode::Default())) {
                selectedKeyframe = UsdTimeCode::Default();
            }
        }
        TF_FOR_ALL(sample, timeSamples) {// Samples
            std::string sampleValueLabel = std::to_string(sample->first);
            if (ImGui::Selectable(sampleValueLabel.c_str(), selectedKeyframe == sample->first)) {
                selectedKeyframe = sample->first;
            }
        }
        ImGui::EndListBox();
    }
}

static void draw_samples_at_time_code(const SdfAttributeSpecHandle &attr, SdfTimeSampleMap &timeSamples,
                                      UsdTimeCode &selectedKeyframe) {
    if (selectedKeyframe == UsdTimeCode::Default()) {
        if (attr->HasDefaultValue()) {
            VtValue value = attr->GetDefaultValue();
            VtValue editedValue = value.IsArrayValued() ? draw_vt_array_value(value) : draw_vt_value("##default", value);
            if (editedValue != VtValue()) {
                execute_after_draw(&SdfAttributeSpec::SetDefaultValue, attr, editedValue);
            }
        }
    } else {
        auto foundSample = timeSamples.find(selectedKeyframe.GetValue());
        if (foundSample != timeSamples.end()) {
            VtValue editResult;
            if (foundSample->second.IsArrayValued()) {
                editResult = draw_vt_array_value(foundSample->second);
            } else {
                editResult = draw_vt_value("##timeSampleValue", foundSample->second);
            }

            if (editResult != VtValue()) {
                execute_after_draw(&SdfLayer::SetTimeSample<VtValue>, attr->GetLayer(), attr->GetPath(), foundSample->first,
                                   editResult);
            }
        }
    }
}

// Work in progress
void draw_sdf_attribute_editor(const SdfLayerHandle layer, const Selection &selection) {
    if (!layer)
        return;
    SdfPath path = selection.get_anchor_property_path(layer);
    if (path == SdfPath())
        return;

    SdfAttributeSpecHandle attr = layer->GetAttributeAtPath(path);
    if (!attr)
        return;

    if (ImGui::BeginTabBar("Stuff")) {

        static UsdTimeCode selectedKeyframe = UsdTimeCode::Default();

        if (ImGui::BeginTabItem("Values")) {
            SdfTimeSampleMap timeSamples = attr->GetTimeSampleMap();

            // Left pane with the time samples
            ScopedStyleColor col(ImGuiCol_FrameBg, ImVec4(0.260f, 0.300f, 0.360f, 1.000f));
            ImGui::BeginChild("left pane", ImVec2(LEFT_PANE_WIDTH, 0), true);// TODO variable width
            draw_time_samples_editor(attr, timeSamples, selectedKeyframe);
            ImGui::EndChild();
            ImGui::SameLine();
            ImGui::BeginChild("value", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()));
            draw_samples_at_time_code(attr, timeSamples, selectedKeyframe);
            ImGui::EndChild();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Metadata")) {
            draw_sdf_layer_identity(layer, path);
            DrawSdfAttributeMetadata(attr);
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Connections")) {
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }
}

}// namespace vox