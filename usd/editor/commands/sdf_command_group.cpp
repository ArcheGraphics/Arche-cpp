//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <memory>
#include <iostream>
#include <ranges>
#include "sdf_command_group.h"
#include "sdf_layer_instructions.h"

namespace vox {
bool SdfCommandGroup::is_empty() const { return _instructions.empty(); }

void SdfCommandGroup::clear() { _instructions.clear(); }

template<typename InstructionT>
void SdfCommandGroup::store_instruction(InstructionT inst) {
    // TODO: specialize by InstructionT type to compact the instructions in the command,
    // typically we don't want to store thousand of setField instruction where only the last one matters
    // One optim would be to look for the previous instruction, check if it is a setfield on the same path, same layer ?
    // Update the latest instruction instead of inserting a new instruction
    // As StoreInstruction is templatized, it is possible to specialize it.
    _instructions.emplace_back(std::move(inst));
}

template void SdfCommandGroup::store_instruction<UndoRedoSetField>(UndoRedoSetField inst);
template void SdfCommandGroup::store_instruction<UndoRedoSetFieldDictValueByKey>(UndoRedoSetFieldDictValueByKey inst);
template void SdfCommandGroup::store_instruction<UndoRedoSetTimeSample>(UndoRedoSetTimeSample inst);
template void SdfCommandGroup::store_instruction<UndoRedoCreateSpec>(UndoRedoCreateSpec inst);
template void SdfCommandGroup::store_instruction<UndoRedoDeleteSpec>(UndoRedoDeleteSpec inst);
template void SdfCommandGroup::store_instruction<UndoRedoMoveSpec>(UndoRedoMoveSpec inst);
template void SdfCommandGroup::store_instruction<UndoRedoPushChild<TfToken>>(UndoRedoPushChild<TfToken> inst);
template void SdfCommandGroup::store_instruction<UndoRedoPushChild<SdfPath>>(UndoRedoPushChild<SdfPath> inst);
template void SdfCommandGroup::store_instruction<UndoRedoPopChild<TfToken>>(UndoRedoPopChild<TfToken> inst);
template void SdfCommandGroup::store_instruction<UndoRedoPopChild<SdfPath>>(UndoRedoPopChild<SdfPath> inst);

// Call all the functions stored in _commands in reverse order
void SdfCommandGroup::undo_it() {
    SdfChangeBlock block;
    for (auto cmd = _instructions.rbegin(); cmd != _instructions.rend(); ++cmd) {
        cmd->undo_it();
    }
}

void SdfCommandGroup::do_it() {
    SdfChangeBlock block;
    for (auto &cmd : _instructions) {
        cmd.do_it();
    }
}

}// namespace vox