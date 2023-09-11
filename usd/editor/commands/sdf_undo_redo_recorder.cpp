//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "sdf_undo_redo_recorder.h"

namespace vox {
SdfUndoRedoRecorder::~SdfUndoRedoRecorder() {
    if (_layer && _previousDelegate) {
        _layer->SetStateDelegate(_previousDelegate);
    }
    if (_editedCommand) {
        CommandStack::get_instance()._push_command(_editedCommand);
        _editedCommand = nullptr;
    }
}

void SdfUndoRedoRecorder::start_recording() {
    if (_layer) {
        _previousDelegate = _layer->GetStateDelegate();
        if (!_editedCommand) {
            _editedCommand = new SdfUndoRedoCommand();
        }
        // Install undo/redo delegate
        _layer->SetStateDelegate(UndoRedoLayerStateDelegate::create(_editedCommand->_undoCommands));
    }
}

void SdfUndoRedoRecorder::stop_recording() {
    if (_layer && _previousDelegate) {
        _layer->SetStateDelegate(_previousDelegate);
    }
}

}// namespace vox