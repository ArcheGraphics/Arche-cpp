//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "modal_dialogs.h"
#include <vector>
#include <imgui.h>

namespace vox {
// Stack of modal dialogs
std::vector<ModalDialog *> modalDialogStack;

void _push_modal_dialog(ModalDialog *modal) { modalDialogStack.push_back(modal); }

bool modalOpenTriggered = false;
bool modalCloseTriggered = false;

bool should_open_modal() {
    if (modalDialogStack.empty()) {
        return false;
    } else if (modalOpenTriggered) {
        modalOpenTriggered = false;
        return true;
    } else {
        return false;
    }
}

void ModalDialog::close_modal() {
    modalCloseTriggered = true;// Delete the dialog at the next frame;
    ImGui::CloseCurrentPopup();// this is called in the draw function
}

void check_close_modal() {
    if (modalCloseTriggered) {
        modalCloseTriggered = false;
        delete modalDialogStack.back();
        modalDialogStack.pop_back();
    }
}

void force_close_current_modal() {
    if (!modalDialogStack.empty()) {
        modalDialogStack.back()->close_modal();
        check_close_modal();
    }
}

static void begin_popup_modal_recursive(const std::vector<ModalDialog *> &modals, int index) {
    if (index < modals.size()) {
        ModalDialog *modal = modals[index];
        if (index == modals.size() - 1) {
            if (should_open_modal()) {
                ImGui::OpenPopup(modal->dialog_id());
            }
        }
        if (ImGui::BeginPopupModal(modal->dialog_id())) {
            modal->draw();
            begin_popup_modal_recursive(modals, index + 1);
            ImGui::EndPopup();
        }
    }
}

//
// DrawCurrentModal will check if there is anything to action
// like closing or draw ... it should really be called ProcessModalDialogs
void draw_current_modal() {
    check_close_modal();
    begin_popup_modal_recursive(modalDialogStack, 0);
}

void draw_ok_cancel_modal(const std::function<void()> &onOk, bool disableOk) {
    // Align right
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetWindowWidth() - 3 * ImGui::CalcTextSize(" Cancel ").x -
                         ImGui::GetScrollX() - 2 * ImGui::GetStyle().ItemSpacing.x);
    if (ImGui::Button(" Cancel ")) {
        modalDialogStack.back()->close_modal();
    }
    ImGui::SameLine();
    ImGui::BeginDisabled(disableOk);
    if (ImGui::Button("   Ok   ")) {
        onOk();
        modalDialogStack.back()->close_modal();
    }
    ImGui::EndDisabled();
}

}// namespace vox