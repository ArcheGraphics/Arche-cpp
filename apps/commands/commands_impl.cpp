//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "command_stack.h"
#include "commands_impl.h"
#include "sdf_command_group.h"
#include "sdf_command_group_recorder.h"
#include "sdf_undo_redo_recorder.h"
#include "undo_layer_state_delegate.h"
#include <functional>
#include <vector>

namespace vox {
bool SdfLayerCommand::UndoIt() {
    _undoCommands.UndoIt();
    return false;
}

bool SdfUndoRedoCommand::UndoIt() {
    _undoCommands.UndoIt();
    return false;
}

bool SdfUndoRedoCommand::DoIt() {
    _undoCommands.DoIt();
    return true;
}
namespace {
SdfUndoRedoRecorder *undoRedoRecorder = nullptr;
}// namespace

void BeginEdition(const SdfLayerRefPtr &layer) {
    if (layer) {
        // TODO: check there is no undoRedoRecorder alive
        undoRedoRecorder = new SdfUndoRedoRecorder(layer);
        undoRedoRecorder->StartRecording();
    }
}
///
void BeginEdition(const UsdStageRefPtr &stage) {
    // Get layer
    if (stage) {
        BeginEdition(stage->GetEditTarget().GetLayer());
    }
}

void EndEdition() {
    if (undoRedoRecorder) {
        undoRedoRecorder->StopRecording();
        delete undoRedoRecorder;
        undoRedoRecorder = nullptr;
    }
}

}// namespace vox
