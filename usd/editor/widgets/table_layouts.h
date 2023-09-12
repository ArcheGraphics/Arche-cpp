//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

namespace vox {
//
// Provide a standard and consistent layout for tables with button+field+value, and I don't have to write and fix the same code multiple time in the source
//
// TODO: rename to TableLayouts.h
// and functions like TableLayoutBeginThreeColumnTable() ...

template<typename FieldT, typename... Args>
inline bool has_edits(const Args &...args) { return true; }

template<typename FieldT, typename... Args>
inline ScopedStyleColor get_row_style(const int rowId, const Args &...args) {
    return ScopedStyleColor(ImGuiCol_Text,
                            has_edits<FieldT>(args...) ? ImVec4(ColorAttributeAuthored) : ImVec4(ColorAttributeUnauthored),
                            ImGuiCol_Button, ImVec4(ColorTransparent), ImGuiCol_FrameBg, ImVec4(0.260f, 0.300f, 0.360f, 1.000f));
}

//
// Default implementation - clients will have to reimplement the column function
//
template<typename FieldT, typename... Args>
inline void draw_first_column(const int rowId, const Args &...args) {}

template<typename FieldT, typename... Args>
inline void draw_second_column(const int rowId, const Args &...args) {
    ImGui::Text(FieldT::fieldName);
}
template<typename FieldT, typename... Args>
inline void draw_third_column(const int rowId, const Args &...args) {}

//
// Provides a standard/unified 3 columns layout
//
template<typename FieldT, typename... Args>
inline void draw_three_columns_row(const int rowId, const Args &...args) {
    ImGui::TableNextRow(ImGuiTableRowFlags_None, TableRowDefaultHeight);
    ScopedStyleColor style = get_row_style<FieldT>(rowId, args...);
    ImGui::TableSetColumnIndex(0);
    draw_first_column<FieldT>(rowId, args...);
    ImGui::TableSetColumnIndex(1);
    draw_second_column<FieldT>(rowId, args...);
    ImGui::TableSetColumnIndex(2);
    draw_third_column<FieldT>(rowId, args...);
}

//
// Standard table with 3 columns used for parameter editing
// First column is generally used for a button
// Second column shows the name of the parameter
// Third column shows the value
inline bool begin_three_columns_table(const char *strId) {
    constexpr ImGuiTableFlags tableFlags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg;
    return ImGui::BeginTable(strId, 3, tableFlags);
}

inline void end_three_columns_table() { ImGui::EndTable(); }

inline void setup_three_columns_table(const bool showHeaders, const char *button = "", const char *field = "Field",
                                      const char *value = "Value") {
    ImGui::TableSetupColumn(button, ImGuiTableColumnFlags_WidthFixed, 24);// 24 => size of the mini button
    ImGui::TableSetupColumn(field, ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableSetupColumn(value, ImGuiTableColumnFlags_WidthStretch);
    if (showHeaders) {
        ImGui::TableHeadersRow();
    }
}

// Rename to DrawTwoColumnRow
template<typename FieldT, typename... Args>
inline void draw_two_columns_row(const int rowId, const Args &...args) {
    ImGui::TableNextRow(ImGuiTableRowFlags_None, TableRowDefaultHeight);
    ScopedStyleColor style = get_row_style<FieldT>(rowId, args...);
    ImGui::TableSetColumnIndex(0);
    draw_first_column<FieldT>(rowId, args...);
    ImGui::TableSetColumnIndex(1);
    draw_second_column<FieldT>(rowId, args...);
}

inline bool begin_two_columns_table(const char *strId) {
    constexpr ImGuiTableFlags tableFlags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg;
    return ImGui::BeginTable(strId, 2, tableFlags);
}

inline void end_two_columns_table() { ImGui::EndTable(); }

inline void setup_two_columns_table(const bool showHeaders, const char *button = "", const char *value = "Value") {
    ImGui::TableSetupColumn(button, ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableSetupColumn(value, ImGuiTableColumnFlags_WidthStretch);
    if (showHeaders) {
        ImGui::TableHeadersRow();
    }
}

}// namespace vox