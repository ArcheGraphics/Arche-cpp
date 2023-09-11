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
bool SdfLayerCommand::undo_it() {
    _undoCommands.undo_it();
    return false;
}

bool SdfUndoRedoCommand::undo_it() {
    _undoCommands.undo_it();
    return false;
}

bool SdfUndoRedoCommand::do_it() {
    _undoCommands.do_it();
    return true;
}
namespace {
SdfUndoRedoRecorder *undoRedoRecorder = nullptr;
}// namespace

void begin_edition(const SdfLayerRefPtr &layer) {
    if (layer) {
        // TODO: check there is no undoRedoRecorder alive
        undoRedoRecorder = new SdfUndoRedoRecorder(layer);
        undoRedoRecorder->start_recording();
    }
}
///
void begin_edition(const UsdStageRefPtr &stage) {
    // Get layer
    if (stage) {
        begin_edition(stage->GetEditTarget().GetLayer());
    }
}

void end_edition() {
    if (undoRedoRecorder) {
        undoRedoRecorder->stop_recording();
        delete undoRedoRecorder;
        undoRedoRecorder = nullptr;
    }
}

}// namespace vox
