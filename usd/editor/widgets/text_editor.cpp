//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "text_editor.h"
#include "commands/commands.h"
#include "base/imgui_helpers.h"

namespace vox {
// The following include contains the code which writes usd to text, but it's not
// distributed with the api
void DrawTextEditor(SdfLayerRefPtr layer) {
    static std::string layerText;
    ImGuiIO &io = ImGui::GetIO();
    ImGuiWindow *window = ImGui::GetCurrentWindow();
    if (window->SkipItems) {
        return;
    }
    ImGui::Text("WARNING: this will slow down the application if the layer is big");
    ImGui::Text("        and will consume lots of memory. Use with care for now");
    if (layer) {
        layer->ExportToString(&layerText);
        ImGui::Text("%s", layer->GetDisplayName().c_str());
    }
    ImGui::PushItemWidth(-FLT_MIN);
    ImGuiWindow *currentWindow = ImGui::GetCurrentWindow();
    ImVec2 sizeArg(0, currentWindow->Size[1] - 120);
    ImGui::PushFont(io.Fonts->Fonts[1]);
    {
        ScopedStyleColor color(ImGuiCol_FrameBg, ImVec4{0.0, 0.0, 0.0, 1.0});
        ImGui::InputTextMultiline("###TextEditor", &layerText, sizeArg,
                                  ImGuiInputTextFlags_None | ImGuiInputTextFlags_NoUndoRedo);
        if (layer && ImGui::IsItemDeactivatedAfterEdit()) {
            ExecuteAfterDraw<LayerTextEdit>(layer, layerText);
        }
    }
    ImGui::PopFont();
    ImGui::PopItemWidth();
    ImGui::Text("Ctrl+Enter to apply your change");
}

}// namespace vox