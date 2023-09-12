//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <imgui.h>
#include <imgui_internal.h>
#include <vector>
#include <string>
#include "fonts/IconsFontAwesome5.h"

namespace vox {
/// One liner for creating multiple calls to ImGui::TableSetupColumn
template<typename T, typename... Args>
inline void table_setup_columns(T t, Args... args) {
    table_setup_columns(t);
    table_setup_columns(args...);
}
template<>
inline void table_setup_columns(const char *label) { ImGui::TableSetupColumn(label); }

/// Creates a scoped object that will push the pair of style and color passed in the constructor
/// It will pop the correct number of time when the object is destroyed
struct ScopedStyleColor {
    ScopedStyleColor() = delete;

    template<typename StyleT, typename ColorT, typename... Args>
    ScopedStyleColor(StyleT &&style, ColorT &&color, Args... args) : nbPop(1 + sizeof...(args) / 2) {
        push_styles(style, color, args...);
    }

    template<typename StyleT, typename ColorT, typename... Args>
    static void push_styles(StyleT &&style, ColorT &&color, Args... args) {// constexpr is
        ImGui::PushStyleColor(style, color);
        push_styles(args...);
    }
    static void push_styles(){};

    ~ScopedStyleColor() {
        ImGui::PopStyleColor(nbPop);
    }

    const size_t nbPop;// TODO: get rid of this constant and generate the correct number of pop at compile time
};

/// Creates a splitter
/// This is coming right from the imgui github repo
bool splitter(bool splitVertically, float thickness, float *size1, float *size2, float minSize1, float minSize2,
              float splitterLongAxisSize = -1.0f);

/// Creates a combo box with a search bar filtering the list elements
bool combo_with_filter(const char *label, const char *preview_value, const std::vector<std::string> &items, int *current_item,
                       ImGuiComboFlags combo_flags, int popup_max_height_in_items = -1);

template<typename PathT>
inline size_t get_hash(const PathT &path) {
    // The original implementation of GetHash can return inconsistent hashes for the same path at different frames
    // This makes the stage tree flicker and look terribly buggy on version > 21.11
    // This issue appears on point instancers.
    // It is expected: https://github.com/PixarAnimationStudios/USD/issues/1922
    return path.GetHash();
    // The following is terribly unefficient but works.
    // return std::hash<std::string>()(path.GetString());
    // For now we store the paths in StageOutliner.cpp TraverseOpenedPaths which seems to work as well.
}

/// Function to convert a hash from usd to ImGuiID with a seed, to avoid collision with path coming from layer and stages.
template<ImU32 seed, typename T>
inline ImGuiID to_imgui_id(const T &val) {
    return ImHashData(static_cast<const void *>(&val), sizeof(T), seed);
}

//// Correctly indent the tree nodes using a path. This is used when we are iterating in a list of paths as opposed to a tree.
//// It allocates a vector which might not be optimal, but it should be used only on visible items, that should mitigate the
//// allocation cost
template<ImU32 seed, typename PathT>
struct TreeIndenter {
    explicit TreeIndenter(const PathT &path) {
        path.GetPrefixes(&prefixes);
        for (int i = 0; i < prefixes.size(); ++i) {
            ImGui::TreePushOverrideID(to_imgui_id<seed>(get_hash(prefixes[i])));
        }
    }
    ~TreeIndenter() {
        for (int i = 0; i < prefixes.size(); ++i) {
            ImGui::TreePop();
        }
    }
    std::vector<PathT> prefixes;
};

}// namespace vox