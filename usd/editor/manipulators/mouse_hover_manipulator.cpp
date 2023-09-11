//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "mouse_hover_manipulator.h"
#include "viewport.h"

namespace vox {
Manipulator *MouseHoverManipulator::OnUpdate(Viewport &viewport) {
    ImGuiIO &io = ImGui::GetIO();

    if (ImGui::IsKeyDown(ImGuiKey_LeftAlt)) {
        return viewport.GetManipulator<CameraManipulator>();
    } else if (ImGui::IsMouseClicked(0)) {
        auto &manipulator = viewport.GetActiveManipulator();
        if (manipulator.IsMouseOver(viewport)) {
            return &manipulator;
        } else {
            return viewport.GetManipulator<SelectionManipulator>();
        }
    } else if (ImGui::IsKeyDown(ImGuiKey_F)) {
        const Selection &selection = viewport.GetSelection();
        if (!selection.IsSelectionEmpty(viewport.GetCurrentStage())) {
            viewport.FrameSelection(viewport.GetSelection());
        } else {
            viewport.FrameRootPrim();
        }
    } else {
        auto &manipulator = viewport.GetActiveManipulator();
        manipulator.IsMouseOver(viewport);
    }
    return this;
}
}// namespace vox