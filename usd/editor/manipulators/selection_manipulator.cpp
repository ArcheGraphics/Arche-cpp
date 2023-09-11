//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <pxr/usd/kind/registry.h>
#include <pxr/usd/usd/modelAPI.h>
#include <pxr/usd/usd/prim.h>
#include "viewport.h"
#include "selection_manipulator.h"
#include "commands/commands.h"
#include "fonts/IconsFontAwesome5.h"

namespace vox {
bool SelectionManipulator::is_pickable_path(const UsdStage &stage, const SdfPath &path) {
    auto prim = stage.GetPrimAtPath(path);
    if (prim.IsPseudoRoot())
        return true;
    if (get_pick_mode() == SelectionManipulator::PickMode::Prim)
        return true;

    TfToken primKind;
    UsdModelAPI(prim).GetKind(&primKind);
    if (get_pick_mode() == SelectionManipulator::PickMode::Model && KindRegistry::GetInstance().IsA(primKind, KindTokens->model)) {
        return true;
    }
    if (get_pick_mode() == SelectionManipulator::PickMode::Assembly &&
        KindRegistry::GetInstance().IsA(primKind, KindTokens->assembly)) {
        return true;
    }

    // Other possible tokens
    // KindTokens->component
    // KindTokens->group
    // KindTokens->subcomponent

    // We can also test for xformable or other schema API

    return false;
}

Manipulator *SelectionManipulator::on_update(Viewport &viewport) {
    Selection &selection = viewport.get_selection();
    auto mousePosition = viewport.get_mouse_position();
    SdfPath outHitPrimPath;
    SdfPath outHitInstancerPath;
    int outHitInstanceIndex = 0;
    viewport.test_intersection(mousePosition, outHitPrimPath, outHitInstancerPath, outHitInstanceIndex);
    if (!outHitPrimPath.IsEmpty()) {
        if (viewport.get_current_stage()) {
            while (!is_pickable_path(*viewport.get_current_stage(), outHitPrimPath)) {
                outHitPrimPath = outHitPrimPath.GetParentPath();
            }
        }

        if (ImGui::IsKeyDown(ImGuiKey_LeftShift)) {
            // TODO: a command
            selection.add_selected(viewport.get_current_stage(), outHitPrimPath);
        } else {
            execute_after_draw<EditorSetSelection>(viewport.get_current_stage(), outHitPrimPath);
        }
    } else if (outHitInstancerPath.IsEmpty()) {
        selection.clear(viewport.get_current_stage());
    }
    return viewport.get_manipulator<MouseHoverManipulator>();
}

void SelectionManipulator::on_draw_frame(const Viewport &) {
    // Draw a rectangle for the selection
}

void draw_pick_mode(SelectionManipulator &manipulator) {
    static const char *PickModeStr[3] = {ICON_FA_HAND_POINTER "      Prim  ", ICON_FA_HAND_POINTER "    Model  ", ICON_FA_HAND_POINTER " Assembly"};
    if (ImGui::BeginCombo("##Pick mode", PickModeStr[int(manipulator.get_pick_mode())], ImGuiComboFlags_NoArrowButton)) {
        if (ImGui::Selectable(PickModeStr[0])) {
            manipulator.set_pick_mode(SelectionManipulator::PickMode::Prim);
        }
        if (ImGui::Selectable(PickModeStr[1])) {
            manipulator.set_pick_mode(SelectionManipulator::PickMode::Model);
        }
        if (ImGui::Selectable(PickModeStr[2])) {
            manipulator.set_pick_mode(SelectionManipulator::PickMode::Assembly);
        }
        ImGui::EndCombo();
    }
}
}// namespace vox