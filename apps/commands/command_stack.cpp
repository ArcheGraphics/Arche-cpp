//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "command_stack.h"

#include <utility>
#include "sdf_command_group_recorder.h"

namespace vox {
CommandStack *CommandStack::instance = nullptr;

CommandStack &CommandStack::get_instance() {
    if (!instance) {
        instance = new CommandStack();
    }
    return *instance;
}

CommandStack::CommandStack() = default;

CommandStack::~CommandStack() {
    delete instance;
}

void CommandStack::execute_commands() {
    if (lastCmd) {
        if (lastCmd->do_it()) {
            _push_command(lastCmd);
        } else {
            delete lastCmd;
        }
        lastCmd = nullptr;// Reset the command
    }
}

void CommandStack::_push_command(Command *cmd) {
    if (undoStackPos != undoStack.size()) {
        undoStack.resize(undoStackPos);
    }
    undoStack.emplace_back(cmd);
    undoStackPos++;
}

struct UndoCommand : public Command {
    UndoCommand() = default;
    ~UndoCommand() override = default;

    /// Undo the last command in the stack
    bool do_it() override;
    bool undo_it() override { return false; }
};

struct RedoCommand : public Command {
    RedoCommand() = default;
    ~RedoCommand() override = default;

    /// Undo the last command in the stack
    bool do_it() override;
    bool undo_it() override { return false; }
};

struct ClearUndoRedoCommand : public Command {
    ClearUndoRedoCommand() = default;
    ~ClearUndoRedoCommand() override = default;

    /// Undo the last command in the stack
    bool do_it() override;
    bool undo_it() override { return false; }
};

// EditorUndo Command
bool UndoCommand::do_it() {
    CommandStack &commandStack = CommandStack::get_instance();
    // TODO : move into stacK ??
    if (commandStack.undoStackPos > 0) {
        commandStack.undoStackPos--;
        commandStack.undoStack[commandStack.undoStackPos]->undo_it();
    }
    return false;// Should never be stored in the stack
}
template void execute_after_draw<UndoCommand>();

/// Undo the last command in the stack
/// Editor Redo command
bool RedoCommand::do_it() {
    // TODO : move into stacK ??
    CommandStack &commandStack = CommandStack::get_instance();
    if (commandStack.undoStackPos < commandStack.undoStack.size()) {
        commandStack.undoStack[commandStack.undoStackPos]->do_it();
        commandStack.undoStackPos++;
    }

    return false;// Should never be stored in the stack
}

template void execute_after_draw<RedoCommand>();

/// Undo the last command in the stack
bool ClearUndoRedoCommand::do_it() {
    CommandStack &commandStack = CommandStack::get_instance();
    commandStack.undoStackPos = 0;
    commandStack.undoStack.clear();
    delete commandStack.lastCmd;
    commandStack.lastCmd = nullptr;
    return false;// Should never be stored in the stack
}
template void execute_after_draw<ClearUndoRedoCommand>();

/// Undo the last command in the stack
bool UsdFunctionCall::do_it() {
    CommandStack &commandStack = CommandStack::get_instance();
    auto *command = new SdfUndoRedoCommand();
    {
        SdfCommandGroupRecorder recorder(command->_undoCommands, _layer);
        _func();
    }
    // Push this SdfUndoRedoCommand command on the stack
    commandStack._push_command(command);

    // We don't want to push UsdFunctionCall, as we already pushed
    // a SdfUndoRedoCommand
    return false;
}

template<>
UsdFunctionCall::UsdFunctionCall(UsdStageRefPtr stage, std::function<void()> func) : _layer(), _func(std::move(func)) {
    if (stage) {// I am assuming the edits always go in the edit target layer
        _layer = stage->GetEditTarget().GetLayer();
    }
}

template void execute_after_draw<UsdFunctionCall>(SdfLayerRefPtr layer, std::function<void()> func);
template void execute_after_draw<UsdFunctionCall>(SdfLayerHandle layer, std::function<void()> func);
template void execute_after_draw<UsdFunctionCall>(UsdStageRefPtr stage, std::function<void()> func);

// Should go in Commands.cpp ???
void ExecuteCommands() {
    CommandStack::get_instance().execute_commands();
}

}// namespace vox