//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <functional>
#include <utility>
#include <vector>
#include "commands_impl.h"
#include "sdf_command_group.h"
#include "sdf_command_group_recorder.h"
#include "undo_layer_state_delegate.h"
#include "command_stack.h"

namespace vox {
/// A SdfUndoRedoRecorder creates an object on the stack which will start recording all the usd commands
/// and stops when it is destroyed.
/// It will also push the recorder commands on the stack
class SdfUndoRedoRecorder final {
public:
    explicit SdfUndoRedoRecorder(SdfLayerRefPtr layer)
        : _editedCommand(nullptr), _layer(std::move(layer)) {
    }
    ~SdfUndoRedoRecorder();

    void StartRecording();
    void StopRecording();

private:
    SdfUndoRedoCommand *_editedCommand;
    SdfLayerRefPtr _layer;
    SdfLayerStateDelegateBaseRefPtr _previousDelegate;
};

}// namespace vox