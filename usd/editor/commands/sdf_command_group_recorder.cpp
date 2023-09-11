//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <iostream>
#include <utility>
#include "sdf_command_group_recorder.h"
#include "undo_layer_state_delegate.h"

namespace vox {
SdfCommandGroupRecorder::SdfCommandGroupRecorder(SdfCommandGroup &undoCommands, const SdfLayerRefPtr &layer)
    : _undoCommands(undoCommands), _layers({layer}) {
    set_undo_state_delegates();
}

SdfCommandGroupRecorder::SdfCommandGroupRecorder(SdfCommandGroup &undoCommands, SdfLayerHandleVector layers)
    : _undoCommands(undoCommands), _layers(std::move(layers)) {
    set_undo_state_delegates();
}

SdfCommandGroupRecorder::~SdfCommandGroupRecorder() {
    unset_undo_state_delegates();
}

void SdfCommandGroupRecorder::set_undo_state_delegates() {
    if (_undoCommands.is_empty()) {
        auto stateDelegate = UndoRedoLayerStateDelegate::create(_undoCommands);
        for (const auto &layer : _layers) {
            if (layer) {
                _previousDelegates.emplace_back(layer->GetStateDelegate());
                layer->SetStateDelegate(stateDelegate);
            } else {
                _previousDelegates.emplace_back();
            }
        }
    }
}

void SdfCommandGroupRecorder::unset_undo_state_delegates() {
    if (!_layers.empty() && !_previousDelegates.empty()) {
        auto previousDelegateIt = _previousDelegates.begin();
        for (const auto &layer : _layers) {
            if (layer) {
                layer->SetStateDelegate(*previousDelegateIt);
            }
            previousDelegateIt++;
        }
    }
}

}// namespace vox