//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "commands.h"
#include <imgui.h>

namespace vox {
// Boiler plate code for adding a shortcut, mainly to avoid writing the same code multiple time
// Only configurable at compile time, so it should change in the future.
template<typename... Args>
inline bool KeyPressed(ImGuiKey key, Args... others) {
    return KeyPressed(key) && KeyPressed(others...);
}
template<>
inline bool KeyPressed(ImGuiKey key) { return ImGui::IsKeyDown(key); }

template<typename CommandT, ImGuiKey... Keys, typename... Args>
inline void AddShortcut(Args &&...args) {
    static bool KeyPressedOnce = false;
    if (KeyPressed(Keys...)) {
        if (!KeyPressedOnce) {
            ExecuteAfterDraw<CommandT>(args...);
            KeyPressedOnce = true;
        }
    } else {
        KeyPressedOnce = false;
    }
}

}// namespace vox