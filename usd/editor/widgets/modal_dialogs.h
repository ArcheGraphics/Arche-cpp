//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once
#include <functional>

namespace vox {
// A modal dialog should know how to draw itself
struct ModalDialog {
    virtual void draw() = 0;
    virtual ~ModalDialog()= default;
    [[nodiscard]] virtual const char *dialog_id() const = 0;
    static void close_modal();
};

/// This is a private variable and function for DrawModalDialog don't use them !
extern bool modalOpenTriggered;
void _push_modal_dialog(ModalDialog *);

/// Trigger a modal dialog
template<typename T, typename... ArgTypes>
void draw_modal_dialog(ArgTypes &&...args) {
    modalOpenTriggered = true;
    _push_modal_dialog(new T(args...));
}

/// Draw the current modal dialog if it has been triggered
void draw_current_modal();

/// Convenience function to draw an Ok and Cancel buttons in a Modal dialog
void draw_ok_cancel_modal(const std::function<void()> &onOk, bool disableOk = false);

/// Force closing the current modal dialog
void force_close_current_modal();

}// namespace vox