//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "commands/commands.h"
#include "editor.h"
#include "launcher_bar.h"
#include "modal_dialogs.h"
#include <imgui_stdlib.h>

namespace vox {
/// Very basic ui to create a connection
struct AddLauncherDialog : public ModalDialog {

    AddLauncherDialog(){};
    ~AddLauncherDialog() override {}

    void draw() override {
        ImGui::InputText("Launcher name", &_launcherName);
        ImGui::InputText("Command line", &_commandLine);
        draw_ok_cancel_modal([&]() { execute_after_draw<EditorAddLauncher>(_launcherName, _commandLine); });
    }
    const char *dialog_id() const override { return "Add launcher"; }

    std::string _launcherName;
    std::string _commandLine;
};

void draw_launcher_bar(Editor *editor) {
    if (!editor)
        return;

    if (ImGui::Button("+")) {
        draw_modal_dialog<AddLauncherDialog>();
    }

    ImGui::SameLine();
    for (const auto &commandName : editor->get_launcher_name_list()) {
        if (ImGui::Button(commandName.c_str())) {
            if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl)) {
                execute_after_draw<EditorRemoveLauncher>(commandName);
            } else {
                execute_after_draw<EditorRunLauncher>(commandName);
            }
        }
        ImGui::SameLine();
    }
    // TODO hint to say that control click deletes the launcher
}

}// namespace vox