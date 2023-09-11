//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <pxr/usd/sdf/layer.h>
#include <pxr/usd/sdf/abstractData.h>
#include "sdf_command_group.h"

PXR_NAMESPACE_USING_DIRECTIVE

namespace vox {
///
/// Scoped sdf commands recorder
///
class SdfCommandGroupRecorder final {
public:
    /// RAII object for recording Sdf "instructions" on one or multiple layers
    SdfCommandGroupRecorder(SdfCommandGroup &undoCommands, const SdfLayerRefPtr &layer);
    SdfCommandGroupRecorder(SdfCommandGroup &undoCommands, SdfLayerHandleVector layers);
    ~SdfCommandGroupRecorder();

private:
    void set_undo_state_delegates();
    void unset_undo_state_delegates();

    // The _undoCommands is used to make sure we don't change the dele
    SdfCommandGroup &_undoCommands;

    // We keep the _previousDelegates of the _layers to restore them when the object is destroyed
    SdfLayerHandleVector _layers;
    SdfLayerStateDelegateBaseRefPtrVector _previousDelegates;
};

}// namespace vox