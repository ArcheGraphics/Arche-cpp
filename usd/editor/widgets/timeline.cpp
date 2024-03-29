//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "timeline.h"
#include "commands/commands.h"
#include <imgui.h>

namespace vox {
// The easiest version of a timeline: a slider
void draw_timeline(const UsdStageRefPtr &stage, UsdTimeCode &currentTimeCode) {
    const bool hasStage = stage;
    constexpr int widgetWidth = 80;
    int startTime = hasStage ? static_cast<int>(stage->GetStartTimeCode()) : 0;
    int endTime = hasStage ? static_cast<int>(stage->GetEndTimeCode()) : 0;

    // Start time
    ImGui::PushItemWidth(widgetWidth);
    ImGui::InputInt("##Start", &startTime, 0);
    if (ImGui::IsItemDeactivatedAfterEdit()) {
        if (hasStage && stage->GetStartTimeCode() != static_cast<double>(startTime)) {
            execute_after_draw(&UsdStage::SetStartTimeCode, stage, static_cast<double>(startTime));
        }
    }

    // Frame Slider
    ImGui::SameLine();
    ImGui::PushItemWidth(ImGui::GetWindowWidth() -
                         6 * widgetWidth);// 6 to account for the 5 input widgets and the space between them
    int currentTimeSlider = static_cast<int>(currentTimeCode.GetValue());
    if (ImGui::SliderInt("##SliderFrame", &currentTimeSlider, startTime, endTime)) {
        currentTimeCode = static_cast<UsdTimeCode>(currentTimeSlider);
    }

    // End time
    ImGui::SameLine();
    ImGui::PushItemWidth(widgetWidth);
    ImGui::InputInt("##End", &endTime, 0);
    if (ImGui::IsItemDeactivatedAfterEdit()) {
        if (hasStage && stage->GetEndTimeCode() != static_cast<double>(endTime)) {
            execute_after_draw(&UsdStage::SetEndTimeCode, stage, static_cast<double>(endTime));
        }
    }

    // Frame input
    ImGui::SameLine();
    ImGui::PushItemWidth(widgetWidth);
    double currentTime = currentTimeCode.GetValue();
    ImGui::InputDouble("##Frame", &currentTime);
    if (ImGui::IsItemDeactivatedAfterEdit()) {
        if (currentTimeCode.GetValue() != static_cast<double>(currentTime)) {
            currentTimeCode = static_cast<UsdTimeCode>(currentTime);
        }
    }

    // Play button
    ImGui::SameLine();
    ImGui::PushItemWidth(widgetWidth);
    if (ImGui::Button("Play", ImVec2(widgetWidth, 0))) {
        execute_after_draw<EditorStartPlayback>();
    }

    // Stop button
    ImGui::SameLine();
    ImGui::PushItemWidth(widgetWidth);
    if (ImGui::Button("Stop", ImVec2(widgetWidth, 0))) {
        execute_after_draw<EditorStopPlayback>();
    }
}

}// namespace vox