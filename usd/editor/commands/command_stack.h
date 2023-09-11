//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <memory>
#include <vector>

#include "commands_impl.h"

namespace vox {
struct CommandStack {
    // Undo and Redo calls are implemented as commands.
    // We compile them in the CommandStack.cpp unit
    friend struct UndoCommand;
    friend struct RedoCommand;
    // Same for ClearUndoRedo
    friend struct ClearUndoRedoCommand;

    //
    friend struct UsdFunctionCall;
    friend class SdfUndoRedoRecorder;

    static CommandStack &get_instance();

    inline bool has_next_command() { return lastCmd != nullptr; }
    inline void set_next_command(Command *command) { lastCmd = command; }

    // Execute next command and push it on the stack
    void execute_commands();

private:
    // The undo stack should ultimately belong to an Editor, not be a global variable
    using UndoStackT = std::vector<std::unique_ptr<Command>>;
    UndoStackT undoStack;

    /// The pointer to the current command in the undo stack
    int undoStackPos = 0;

    // Storing only one command per frame for now, easier to reason about.
    Command *lastCmd = nullptr;

    /// The ProcessCommands function is called after the frame is rendered and displayed and execute the
    /// last command. The command passed here now belongs to this stack
    void _push_command(Command *cmd);

private:
    CommandStack();
    ~CommandStack();
    static CommandStack *instance;
};

/// Dispatching Commands.
template<typename CommandClass, typename... ArgTypes>
void execute_after_draw(ArgTypes... arguments) {
    CommandStack &commandStack = CommandStack::get_instance();
    if (!commandStack.has_next_command()) {
        commandStack.set_next_command(new CommandClass(arguments...));
    }
}

}// namespace vox