//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <iostream>

#include <vector>

#include <pxr/usd/pcp/layerStack.h>
#include <pxr/usd/usd/primRange.h>
#include <pxr/usd/usdGeom/gprim.h>

#include "commands/commands.h"
#include "base/imgui_helpers.h"
#include "usd_prim_editor.h"// for DrawUsdPrimEditTarget
#include "stage_outliner.h"
#include "vt_value_editor.h"
#include "base/constants.h"

namespace vox {
#define StageOutlinerSeed 2342934
#define IdOf to_imgui_id<StageOutlinerSeed, size_t>

class StageOutlinerDisplayOptions {
public:
    StageOutlinerDisplayOptions() { compute_prim_flags_predicate(); }

    [[nodiscard]] Usd_PrimFlagsPredicate get_prim_flags_predicate() const { return _displayPredicate; }

    void toggle_show_prototypes() { _showPrototypes = !_showPrototypes; }

    void toggle_show_inactive() {
        _showInactive = !_showInactive;
        compute_prim_flags_predicate();
    }
    void toggle_show_abstract() {
        _showAbstract = !_showAbstract;
        compute_prim_flags_predicate();
    }
    void toggle_show_unloaded() {
        _showUnloaded = !_showUnloaded;
        compute_prim_flags_predicate();
    }
    void toggle_show_undefined() {
        _showUndefined = !_showUndefined;
        compute_prim_flags_predicate();
    }

    [[nodiscard]] bool get_show_prototypes() const { return _showPrototypes; }
    [[nodiscard]] bool get_show_inactive() const { return _showInactive; }
    [[nodiscard]] bool get_show_unloaded() const { return _showUnloaded; }
    [[nodiscard]] bool get_show_abstract() const { return _showAbstract; }
    [[nodiscard]] bool get_show_undefined() const { return _showUndefined; }

private:
    // Default is:
    // UsdPrimIsActive && UsdPrimIsDefined && UsdPrimIsLoaded && !UsdPrimIsAbstract
    void compute_prim_flags_predicate() {
        Usd_PrimFlagsConjunction flags;
        if (!_showInactive) {
            flags = flags && UsdPrimIsActive;
        }
        if (!_showUndefined) {
            flags = flags && UsdPrimIsDefined;
        }
        if (!_showUnloaded) {
            flags = flags && UsdPrimIsLoaded;
        }
        if (!_showAbstract) {
            flags = flags && !UsdPrimIsAbstract;
        }
        _displayPredicate = UsdTraverseInstanceProxies(flags);
    }

    Usd_PrimFlagsPredicate _displayPredicate;
    bool _showInactive = true;
    bool _showUndefined = false;
    bool _showUnloaded = true;
    bool _showAbstract = false;
    bool _showPrototypes = true;
};

static void explore_layer_tree(const SdfLayerTreeHandle &tree, PcpNodeRef node) {
    if (!tree)
        return;
    auto obj = tree->GetLayer()->GetObjectAtPath(node.GetPath());
    if (obj) {
        std::string format;
        format += tree->GetLayer()->GetDisplayName();
        format += " ";
        format += obj->GetPath().GetString();
        if (ImGui::MenuItem(format.c_str())) {
            execute_after_draw<EditorSetSelection>(tree->GetLayer(), obj->GetPath());
        }
    }
    for (const auto &subTree : tree->GetChildTrees()) {
        explore_layer_tree(subTree, node);
    }
}

static void explore_composition(PcpNodeRef root) {
    auto tree = root.GetLayerStack()->GetLayerTree();
    explore_layer_tree(tree, root);
    TF_FOR_ALL(childNode, root.GetChildrenRange()) { explore_composition(*childNode); }
}

static void DrawUsdPrimEditMenuItems(const UsdPrim &prim) {
    if (ImGui::MenuItem("Toggle active")) {
        const bool active = !prim.IsActive();
        execute_after_draw(&UsdPrim::SetActive, prim, active);
    }
    // TODO: Load and Unload are not in the undo redo :( ... make a command for them
    if (prim.HasAuthoredPayloads() && prim.IsLoaded() && ImGui::MenuItem("Unload")) {
        execute_after_draw(&UsdPrim::Unload, prim);
    }
    if (prim.HasAuthoredPayloads() && !prim.IsLoaded() && ImGui::MenuItem("Load")) {
        execute_after_draw(&UsdPrim::Load, prim, UsdLoadWithDescendants);
    }
    if (ImGui::MenuItem("Copy prim path")) {
        ImGui::SetClipboardText(prim.GetPath().GetString().c_str());
    }
    if (ImGui::BeginMenu("Edit layer")) {
        auto pcpIndex = prim.ComputeExpandedPrimIndex();
        if (pcpIndex.IsValid()) {
            auto rootNode = pcpIndex.GetRootNode();
            explore_composition(rootNode);
        }
        ImGui::EndMenu();
    }
}

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
    if (!prim.IsDefined()) {
        return ImVec4(ColorPrimUndefined);
    }
    return ImVec4(ColorPrimDefault);
}

static inline const char *get_visibility_icon(const TfToken &visibility) {
    if (visibility == UsdGeomTokens->inherited) {
        return ICON_FA_HAND_POINT_UP;
    } else if (visibility == UsdGeomTokens->invisible) {
        return ICON_FA_EYE_SLASH;
    } else if (visibility == UsdGeomTokens->visible) {
        return ICON_FA_EYE;
    }
    return ICON_FA_EYE;
}

static void draw_visibility_button(const UsdPrim &prim) {
    // TODO: this should work with animation
    UsdGeomImageable imageable(prim);
    if (imageable) {
        ImGui::PushID(prim.GetPath().GetHash());
        // Get visibility value
        auto attr = imageable.GetVisibilityAttr();
        VtValue visibleValue;
        attr.Get(&visibleValue);
        TfToken visibilityToken = visibleValue.Get<TfToken>();
        const char *visibilityIcon = get_visibility_icon(visibilityToken);
        {
            ScopedStyleColor buttonColor(
                ImGuiCol_Text, attr.HasAuthoredValue() ? ImVec4(1.0, 1.0, 1.0, 1.0) : ImVec4(ColorPrimInactive));
            ImGui::SmallButton(visibilityIcon);
            // Menu to select the new visibility
            {
                ScopedStyleColor menuTextColor(ImGuiCol_Text, ImVec4(1.0, 1.0, 1.0, 1.0));
                if (ImGui::BeginPopupContextItem(nullptr, ImGuiPopupFlags_MouseButtonLeft)) {
                    if (attr.HasAuthoredValue() && ImGui::MenuItem("clear visibiliy")) {
                        execute_after_draw(&UsdPrim::RemoveProperty, prim, attr.GetName());
                    }
                    VtValue allowedTokens;
                    attr.GetMetadata(TfToken("allowedTokens"), &allowedTokens);
                    if (allowedTokens.IsHolding<VtArray<TfToken>>()) {
                        for (const auto &token : allowedTokens.Get<VtArray<TfToken>>()) {
                            if (ImGui::MenuItem(token.GetText())) {
                                execute_after_draw<AttributeSet>(attr, VtValue(token), UsdTimeCode::Default());
                            }
                        }
                    }
                    ImGui::EndPopup();
                }
            }
        }
        ImGui::PopID();
    }
}

// This is pretty similar to DrawBackgroundSelection in the SdfLayerSceneGraphEditor
static void DrawBackgroundSelection(const UsdPrim &prim, bool selected) {

    ImVec4 colorSelected = selected ? ImVec4(ColorPrimSelectedBg) : ImVec4(0.75, 0.60, 0.33, 0.2);
    ScopedStyleColor scopedStyle(ImGuiCol_HeaderHovered, selected ? colorSelected : ImVec4(ColorTransparent),
                                 ImGuiCol_HeaderActive, ImVec4(ColorTransparent), ImGuiCol_Header, colorSelected);
    ImVec2 sizeArg(0.0, TableRowDefaultHeight);
    const auto selectableFlags = ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowItemOverlap;
    ImGui::Selectable("##backgroundSelectedPrim", selected, selectableFlags, sizeArg);
    ImGui::SetItemAllowOverlap();
    ImGui::SameLine();
}

static void DrawPrimTreeRow(const UsdPrim &prim, Selection &selectedPaths, StageOutlinerDisplayOptions &displayOptions) {
    ImGuiTreeNodeFlags flags =
        ImGuiTreeNodeFlags_OpenOnArrow |
        ImGuiTreeNodeFlags_AllowItemOverlap;// for testing worse case scenario add | ImGuiTreeNodeFlags_DefaultOpen;

    // Another way ???
    const auto &children = prim.GetFilteredChildren(displayOptions.get_prim_flags_predicate());
    if (children.empty()) {
        flags |= ImGuiTreeNodeFlags_Leaf;
    }

    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    DrawBackgroundSelection(prim, selectedPaths.is_selected(prim.GetStage(), prim.GetPath()));
    bool unfolded;
    {
        {
            TreeIndenter<StageOutlinerSeed, SdfPath> indenter(prim.GetPath());
            ScopedStyleColor primColor(ImGuiCol_Text, get_prim_color(prim), ImGuiCol_HeaderHovered, 0, ImGuiCol_HeaderActive, 0);
            const ImGuiID pathHash = IdOf(get_hash(prim.GetPath()));

            unfolded = ImGui::TreeNodeBehavior(pathHash, flags, prim.GetName().GetText());
            // TreeSelectionBehavior(selectedPaths, &prim);
            if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
                // TODO selection, should go in commands, ultimately the selection is passed
                // as const
                if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl)) {
                    if (selectedPaths.is_selected(prim.GetStage(), prim.GetPath())) {
                        selectedPaths.remove_selected(prim.GetStage(), prim.GetPath());
                    } else {
                        selectedPaths.add_selected(prim.GetStage(), prim.GetPath());
                    }
                } else {
                    execute_after_draw<EditorSetSelection>(prim.GetStage(), prim.GetPath());
                }
            }
            {
                ScopedStyleColor popupColor(ImGuiCol_Text, ImVec4(ColorPrimDefault));
                if (ImGui::BeginPopupContextItem()) {
                    DrawUsdPrimEditMenuItems(prim);
                    ImGui::EndPopup();
                }
            }
        }
        // Visibility
        ImGui::TableSetColumnIndex(1);
        draw_visibility_button(prim);

        // Type
        ImGui::TableSetColumnIndex(2);
        ImGui::Text("%s", prim.GetTypeName().GetText());
    }
    if (unfolded) {
        ImGui::TreePop();
    }
}

static void DrawStageTreeRow(const UsdStageRefPtr &stage, Selection &selectedPaths) {
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);

    ImGuiTreeNodeFlags nodeflags = ImGuiTreeNodeFlags_OpenOnArrow;
    std::string stageDisplayName(stage->GetRootLayer()->GetDisplayName());
    auto unfolded = ImGui::TreeNodeBehavior(IdOf(get_hash(SdfPath::AbsoluteRootPath())), nodeflags, stageDisplayName.c_str());

    ImGui::TableSetColumnIndex(2);
    ImGui::SmallButton(ICON_FA_PEN);
    if (ImGui::BeginPopupContextItem(nullptr, ImGuiPopupFlags_MouseButtonLeft)) {
        const UsdPrim &selected =
            selectedPaths.is_selection_empty(stage) ? stage->GetPseudoRoot() : stage->GetPrimAtPath(selectedPaths.get_anchor_prim_path(stage));
        draw_usd_prim_edit_target(selected);
        ImGui::EndPopup();
    }
    ImGui::SameLine();
    ImGui::Text("%s", stage->GetEditTarget().GetLayer()->GetDisplayName().c_str());
    if (unfolded) {
        ImGui::TreePop();
    }
}

/// This function should be called only when the Selection has changed
/// It modifies the internal imgui tree graph state.
static void OpenSelectedPaths(const UsdStageRefPtr &stage, Selection &selectedPaths) {
    ImGuiContext &g = *GImGui;
    ImGuiWindow *window = g.CurrentWindow;
    ImGuiStorage *storage = window->DC.StateStorage;
    for (const auto &path : selectedPaths.get_selected_paths(stage)) {
        for (const auto &element : path.GetParentPath().GetPrefixes()) {
            ImGuiID id = IdOf(get_hash(element));// This has changed with the optim one
            storage->SetInt(id, true);
        }
    }
}

static void TraverseRange(UsdPrimRange &range, std::vector<SdfPath> &paths) {
    static std::set<SdfPath> retainedPath;// to fix a bug with instanced prim which recreates the path at every call and give a different hash
    ImGuiContext &g = *GImGui;
    ImGuiWindow *window = g.CurrentWindow;
    ImGuiStorage *storage = window->DC.StateStorage;
    for (auto iter = range.begin(); iter != range.end(); ++iter) {
        const auto &path = iter->GetPath();
        const ImGuiID pathHash = IdOf(get_hash(path));
        const bool isOpen = storage->GetInt(pathHash, 0) != 0;
        if (!isOpen) {
            iter.PruneChildren();
        }
        // This bit of code is to avoid a bug. It appears that the SdfPath of instance proxies are not kept and the underlying memory
        // is deleted and recreated between each frame, invalidating the hash value. So for the same path we have different hash every frame :s not cool.
        // This problems appears on versions > 21.11
        // a look at the changelog shows that they were lots of changes on the SdfPath side:
        // https://github.com/PixarAnimationStudios/USD/commit/46c26f63d2a6e9c6c5dbfbcefa0235c3265457bb
        //
        // In the end we workaround this issue by keeping the instance proxy paths alive:
        if (iter->IsInstanceProxy()) {
            retainedPath.insert(path);
        }
        paths.push_back(path);
    }
}

// Traverse the stage skipping the paths closed by the tree ui.
static void TraverseOpenedPaths(const UsdStageRefPtr &stage, std::vector<SdfPath> &paths, StageOutlinerDisplayOptions &displayOptions) {
    if (!stage)
        return;
    ImGuiContext &g = *GImGui;
    ImGuiWindow *window = g.CurrentWindow;
    ImGuiStorage *storage = window->DC.StateStorage;
    paths.clear();
    const SdfPath &rootPath = SdfPath::AbsoluteRootPath();
    const bool rootPathIsOpen = storage->GetInt(IdOf(get_hash(rootPath)), 0) != 0;

    if (rootPathIsOpen) {
        // Stage
        auto range = UsdPrimRange::Stage(stage, displayOptions.get_prim_flags_predicate());
        TraverseRange(range, paths);
        // Prototypes
        if (displayOptions.get_show_prototypes()) {
            for (const auto &proto : stage->GetPrototypes()) {
                auto range = UsdPrimRange(proto, displayOptions.get_prim_flags_predicate());
                TraverseRange(range, paths);
            }
        }
    }
}

static void FocusedOnFirstSelectedPath(const SdfPath &selectedPath, const std::vector<SdfPath> &paths,
                                       ImGuiListClipper &clipper) {
    // linear search! it happens only when the selection has changed. We might want to maintain a map instead
    // if the hierarchies are big.
    for (int i = 0; i < paths.size(); ++i) {
        if (paths[i] == selectedPath) {
            // scroll only if the item is not visible
            if (i < clipper.DisplayStart || i > clipper.DisplayEnd) {
                ImGui::SetScrollY(clipper.ItemsHeight * i + 1);
            }
            return;
        }
    }
}

void DrawStageOutlinerMenuBar(StageOutlinerDisplayOptions &displayOptions) {

    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("Show")) {
            if (ImGui::MenuItem("Inactive", nullptr, displayOptions.get_show_inactive())) {
                displayOptions.toggle_show_inactive();
            }
            if (ImGui::MenuItem("Undefined", nullptr, displayOptions.get_show_undefined())) {
                displayOptions.toggle_show_undefined();
            }
            if (ImGui::MenuItem("Unloaded", nullptr, displayOptions.get_show_unloaded())) {
                displayOptions.toggle_show_unloaded();
            }
            if (ImGui::MenuItem("Abstract", nullptr, displayOptions.get_show_abstract())) {
                displayOptions.toggle_show_abstract();
            }
            if (ImGui::MenuItem("Prototypes", nullptr, displayOptions.get_show_prototypes())) {
                displayOptions.toggle_show_prototypes();
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
}

/// Draw the hierarchy of the stage
void DrawStageOutliner(const UsdStageRefPtr &stage, Selection &selectedPaths) {
    if (!stage)
        return;

    static StageOutlinerDisplayOptions displayOptions;
    DrawStageOutlinerMenuBar(displayOptions);

    auto rootPrim = stage->GetPseudoRoot();
    auto layer = stage->GetSessionLayer();

    static SelectionHash lastSelectionHash = 0;

    ImGuiWindow *currentWindow = ImGui::GetCurrentWindow();
    ImVec2 tableOuterSize(0, currentWindow->Size[1] - 100);// TODO: set the correct size
    constexpr ImGuiTableFlags tableFlags = ImGuiTableFlags_SizingFixedFit | /*ImGuiTableFlags_RowBg |*/ ImGuiTableFlags_ScrollY;
    if (ImGui::BeginTable("##DrawStageOutliner", 3, tableFlags, tableOuterSize)) {
        ImGui::TableSetupScrollFreeze(3, 1);// Freeze the root node of the tree (the layer)
        ImGui::TableSetupColumn("Hierarchy");
        ImGui::TableSetupColumn("V", ImGuiTableColumnFlags_WidthFixed, 40);
        ImGui::TableSetupColumn("Type");

        // Unfold the selected path
        const bool selectionHasChanged = selectedPaths.update_selection_hash(stage, lastSelectionHash);
        if (selectionHasChanged) {                  // We could use the imgui id as well instead of a static ??
            OpenSelectedPaths(stage, selectedPaths);// Also we could have a UsdTweakFrame which contains all the changes that happened
                                                    // between the last frame and the new one
        }

        // Find all the opened paths
        std::vector<SdfPath> paths;
        paths.reserve(1024);
        TraverseOpenedPaths(stage, paths, displayOptions);// This must be inside the table scope to get the correct treenode hash table

        // Draw the tree root node, the layer
        DrawStageTreeRow(stage, selectedPaths);

        // Display only the visible paths with a clipper
        ImGuiListClipper clipper;
        clipper.Begin(static_cast<int>(paths.size()));
        while (clipper.Step()) {
            for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++) {
                ImGui::PushID(row);
                const SdfPath &path = paths[row];
                const auto &prim = stage->GetPrimAtPath(path);
                DrawPrimTreeRow(prim, selectedPaths, displayOptions);
                ImGui::PopID();
            }
        }
        if (selectionHasChanged) {
            // This function can only be called in this context and after the clipper.Step()
            FocusedOnFirstSelectedPath(selectedPaths.get_anchor_prim_path(stage), paths, clipper);
        }
        ImGui::EndTable();
    }

    // Search prim bar
    static char patternBuffer[256];
    static bool useRegex = false;
    auto enterPressed = ImGui::InputTextWithHint("##SearchPrims", "Find prim", patternBuffer, 256, ImGuiInputTextFlags_EnterReturnsTrue);
    ImGui::SameLine();
    ImGui::Checkbox("use regex", &useRegex);
    ImGui::SameLine();
    if (ImGui::Button("Select next") || enterPressed) {
        execute_after_draw<EditorFindPrim>(std::string(patternBuffer), useRegex);
    }
}

}// namespace vox