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

CommandStack &CommandStack::GetInstance() {
    if (!instance) {
        instance = new CommandStack();
    }
    return *instance;
}

CommandStack::CommandStack() = default;
CommandStack::~CommandStack() {
    delete instance;
}

void CommandStack::ExecuteCommands() {
    if (lastCmd) {
        if (lastCmd->DoIt()) {
            _PushCommand(lastCmd);
        } else {
            delete lastCmd;
        }
        lastCmd = nullptr;// Reset the command
    }
}

void CommandStack::_PushCommand(Command *cmd) {
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
    bool DoIt() override;
    bool UndoIt() override { return false; }
};

struct RedoCommand : public Command {

    RedoCommand() = default;
    ~RedoCommand() override = default;

    /// Undo the last command in the stack
    bool DoIt() override;
    bool UndoIt() override { return false; }
};

struct ClearUndoRedoCommand : public Command {

    ClearUndoRedoCommand() = default;
    ~ClearUndoRedoCommand() override = default;

    /// Undo the last command in the stack
    bool DoIt() override;
    bool UndoIt() override { return false; }
};

// EditorUndo Command
bool UndoCommand::DoIt() {
    CommandStack &commandStack = CommandStack::GetInstance();
    // TODO : move into stacK ??
    if (commandStack.undoStackPos > 0) {
        commandStack.undoStackPos--;
        commandStack.undoStack[commandStack.undoStackPos]->UndoIt();
    }
    return false;// Should never be stored in the stack
}
template void ExecuteAfterDraw<UndoCommand>();

/// Undo the last command in the stack
/// Editor Redo command
bool RedoCommand::DoIt() {
    // TODO : move into stacK ??
    CommandStack &commandStack = CommandStack::GetInstance();
    if (commandStack.undoStackPos < commandStack.undoStack.size()) {
        commandStack.undoStack[commandStack.undoStackPos]->DoIt();
        commandStack.undoStackPos++;
    }

    return false;// Should never be stored in the stack
}

template void ExecuteAfterDraw<RedoCommand>();

/// Undo the last command in the stack
bool ClearUndoRedoCommand::DoIt() {
    CommandStack &commandStack = CommandStack::GetInstance();
    commandStack.undoStackPos = 0;
    commandStack.undoStack.clear();
    delete commandStack.lastCmd;
    commandStack.lastCmd = nullptr;
    return false;// Should never be stored in the stack
}
template void ExecuteAfterDraw<ClearUndoRedoCommand>();

/// Undo the last command in the stack
bool UsdFunctionCall::DoIt() {
    CommandStack &commandStack = CommandStack::GetInstance();
    auto *command = new SdfUndoRedoCommand();
    {
        SdfCommandGroupRecorder recorder(command->_undoCommands, _layer);
        _func();
    }
    // Push this SdfUndoRedoCommand command on the stack
    commandStack._PushCommand(command);

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

template void ExecuteAfterDraw<UsdFunctionCall>(SdfLayerRefPtr layer, std::function<void()> func);
template void ExecuteAfterDraw<UsdFunctionCall>(SdfLayerHandle layer, std::function<void()> func);
template void ExecuteAfterDraw<UsdFunctionCall>(UsdStageRefPtr stage, std::function<void()> func);

// Should go in Commands.cpp ???
void ExecuteCommands() {
    CommandStack::GetInstance().ExecuteCommands();
}

}// namespace vox