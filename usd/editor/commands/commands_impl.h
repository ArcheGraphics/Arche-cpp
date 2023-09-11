//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "sdf_command_group.h"
#include <memory>
#include <pxr/usd/usd/stage.h>// For BeginEdition
#include <utility>
#include <vector>

PXR_NAMESPACE_USING_DIRECTIVE

namespace vox {
struct Command {
    virtual ~Command() = default;
    virtual bool do_it() = 0;
    virtual bool undo_it() { return false; }
};

struct SdfLayerCommand : public Command {
    ~SdfLayerCommand() override = default;
    bool do_it() override = 0;
    bool undo_it() override;
    SdfCommandGroup _undoCommands;
};

// Placeholder command for recorder
struct SdfUndoRedoCommand : public SdfLayerCommand {
    bool do_it() override;
    bool undo_it() override;
};

// UsdFunctionCall is a transition command, it internally
// run the function passed to its constructor recording all the sdf event
// and creating a new SdfUndoCommand that will be stored in the stack instead of
// itself
struct UsdFunctionCall : public Command {
    template<typename LayerT>
    UsdFunctionCall(LayerT layer, std::function<void()> func) : _layer(layer), _func(std::move(func)) {}

    ~UsdFunctionCall() override = default;

    /// Undo the last command in the stack
    bool do_it() override;
    bool undo_it() override { return false; }

    SdfLayerHandle _layer;
    std::function<void()> _func;
};

}// namespace vox