//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "debug.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_stdlib.h>
#include "pxr/base/trace/reporter.h"
#include "pxr/base/trace/trace.h"
#include <pxr/base/plug/plugin.h>
#include <pxr/base/plug/registry.h>
#include <pxr/base/tf/debug.h>

PXR_NAMESPACE_USING_DIRECTIVE

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

static void draw_trace_reporter() {
    static std::string reportStr;

    if (ImGui::Button("Start Tracing")) {
        TraceCollector::GetInstance().SetEnabled(true);
    }
    ImGui::SameLine();
    if (ImGui::Button("Stop Tracing")) {
        TraceCollector::GetInstance().SetEnabled(false);
    }
    ImGui::SameLine();
    if (ImGui::Button("Reset counters")) {
        TraceReporter::GetGlobalReporter()->ClearTree();
    }
    ImGui::SameLine();

    if (ImGui::Button("Update tree")) {
        TraceReporter::GetGlobalReporter()->UpdateTraceTrees();
    }
    if (TraceCollector::IsEnabled()) {
        std::ostringstream report;
        TraceReporter::GetGlobalReporter()->Report(report);
        reportStr = report.str();
    }
    ImGuiIO &io = ImGui::GetIO();
    ImGui::PushFont(io.Fonts->Fonts[1]);
    const ImVec2 size(-FLT_MIN, -10);
    ImGui::InputTextMultiline("##TraceReport", &reportStr, size);
    ImGui::PopFont();
}

static void draw_debug_codes() {
    // TfDebug::IsCompileTimeEnabled()
    ImVec2 listBoxSize(-FLT_MIN, -10);
    if (ImGui::BeginListBox("##DebugCodes", listBoxSize)) {
        for (auto &code : TfDebug::GetDebugSymbolNames()) {
            bool isEnabled = TfDebug::IsDebugSymbolNameEnabled(code);
            if (ImGui::Checkbox(code.c_str(), &isEnabled)) {
                TfDebug::SetDebugSymbolsByName(code, isEnabled);
            }
        }
        ImGui::EndListBox();
    }
}

static void draw_plugins() {
    const PlugPluginPtrVector &plugins = PlugRegistry::GetInstance().GetAllPlugins();
    ImVec2 listBoxSize(-FLT_MIN, -10);
    if (ImGui::BeginListBox("##Plugins", listBoxSize)) {
        for (const auto &plug : plugins) {
            const std::string &plugName = plug->GetName();
            const std::string &plugPath = plug->GetPath();
            bool isLoaded = plug->IsLoaded();
            if (ImGui::Checkbox(plugName.c_str(), &isLoaded)) {
                plug->Load();// There is no Unload in the API
            }
            ImGui::SameLine();
            ImGui::Text("%s", plugPath.c_str());
        }
        ImGui::EndListBox();
    }
}

// Draw a preference like panel
void draw_debug_ui() {
    static const char *const panels[] = {"Timings", "Debug codes", "Trace reporter", "Plugins"};
    static int current_item = 0;
    ImGui::PushItemWidth(100);
    ImGui::ListBox("##DebugPanels", &current_item, panels, 4);
    ImGui::SameLine();
    if (current_item == 0) {
        ImGui::BeginChild("##Timing");
        ImGui::Text("ImGui: %.3f ms/frame  (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::EndChild();
    } else if (current_item == 1) {
        ImGui::BeginChild("##DebugCodes");
        draw_debug_codes();
        ImGui::EndChild();
    } else if (current_item == 2) {
        ImGui::BeginChild("##TraceReporter");
        draw_trace_reporter();
        ImGui::EndChild();
    } else if (current_item == 3) {
        ImGui::BeginChild("##Plugins");
        draw_plugins();
        ImGui::EndChild();
    }
}

}// namespace vox