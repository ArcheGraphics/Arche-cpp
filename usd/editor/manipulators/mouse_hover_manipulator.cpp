//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "mouse_hover_manipulator.h"
#include "viewport.h"
#include <imgui.h>

namespace vox {
Manipulator *MouseHoverManipulator::on_update(Viewport &viewport) {
    ImGuiIO &io = ImGui::GetIO();

    if (ImGui::IsKeyDown(ImGuiKey_LeftAlt)) {
        return viewport.get_manipulator<CameraManipulator>();
    } else if (ImGui::IsMouseClicked(0)) {
        auto &manipulator = viewport.get_active_manipulator();
        if (manipulator.is_mouse_over(viewport)) {
            return &manipulator;
        } else {
            return viewport.get_manipulator<SelectionManipulator>();
        }
    } else if (ImGui::IsKeyDown(ImGuiKey_F)) {
        const Selection &selection = viewport.get_selection();
        if (!selection.is_selection_empty(viewport.get_current_stage())) {
            viewport.frame_selection(viewport.get_selection());
        } else {
            viewport.frame_root_prim();
        }
    } else {
        auto &manipulator = viewport.get_active_manipulator();
        manipulator.is_mouse_over(viewport);
    }
    return this;
}
}// namespace vox