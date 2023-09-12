//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "debug.h"

#include <iostream>
#include <array>
#include <utility>
#include <imgui.h>
#include <imgui_internal.h>

namespace vox {
void draw_debug_info() {
    ImGuiIO &io = ImGui::GetIO();
    ImGui::Text("Keys pressed:");
    for (int i = 0; i < IM_ARRAYSIZE(io.KeysDown); i++)
        if (ImGui::IsKeyPressed(ImGuiKey(i))) {
            ImGui::SameLine();
            ImGui::Text("%d", i);
        }
    ImGui::Text("Keys release:");
    for (int i = 0; i < IM_ARRAYSIZE(io.KeysDown); i++)
        if (ImGui::IsKeyReleased(ImGuiKey(i))) {
            ImGui::SameLine();
            ImGui::Text("%d", i);
        }

    if (ImGui::IsMousePosValid())
        ImGui::Text("Mouse pos: (%g, %g)", io.MousePos.x, io.MousePos.y);
    ImGui::Text("Mouse clicked:");
    for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++)
        if (ImGui::IsMouseClicked(i)) {
            ImGui::SameLine();
            ImGui::Text("b%d", i);
        }
    ImGui::Text("NavInputs down:");
    for (int i = 0; i < IM_ARRAYSIZE(io.NavInputs); i++)
        if (io.NavInputs[i] > 0.0f) {
            ImGui::SameLine();
            ImGui::Text("[%d] %.2f", i, io.NavInputs[i]);
        }
    //    ImGui::Text("NavInputs pressed:");
    //    for (int i = 0; i < IM_ARRAYSIZE(io.NavInputs); i++)
    //        if (io.NavInputsDownDuration[i] == 0.0f) {
    //            ImGui::SameLine();
    //            ImGui::Text("[%d]", i);
    //        }
    //    ImGui::Text("NavInputs duration:");
    //    for (int i = 0; i < IM_ARRAYSIZE(io.NavInputs); i++)
    //        if (io.NavInputsDownDuration[i] >= 0.0f) {
    //            ImGui::SameLine();
    //            ImGui::Text("[%d] %.2f", i, io.NavInputsDownDuration[i]);
    //        }
    ImGui::Text("Mouse delta: (%g, %g)", io.MouseDelta.x, io.MouseDelta.y);
    ImGui::Text("WantCaptureMouse: %d", io.WantCaptureMouse);
    ImGui::Text("WantCaptureKeyboard: %d", io.WantCaptureKeyboard);
    ImGui::Text("WantTextInput: %d", io.WantTextInput);
    ImGui::Text("WantSetMousePos: %d", io.WantSetMousePos);
    ImGui::Text("NavActive: %d, NavVisible: %d", io.NavActive, io.NavVisible);
    ImGui::Text("Mouse released:");
    for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++)
        if (ImGui::IsMouseReleased(i)) {
            ImGui::SameLine();
            ImGui::Text("b%d", i);
        }
}

}// namespace vox