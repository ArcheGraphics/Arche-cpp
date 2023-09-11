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
bool SdfCommandGroup::IsEmpty() const { return _instructions.empty(); }

void SdfCommandGroup::Clear() { _instructions.clear(); }

template<typename InstructionT>
void SdfCommandGroup::StoreInstruction(InstructionT inst) {
    // TODO: specialize by InstructionT type to compact the instructions in the command,
    // typically we don't want to store thousand of setField instruction where only the last one matters
    // One optim would be to look for the previous instruction, check if it is a setfield on the same path, same layer ?
    // Update the latest instruction instead of inserting a new instruction
    // As StoreInstruction is templatized, it is possible to specialize it.
    _instructions.emplace_back(std::move(inst));
}

template void SdfCommandGroup::StoreInstruction<UndoRedoSetField>(UndoRedoSetField inst);
template void SdfCommandGroup::StoreInstruction<UndoRedoSetFieldDictValueByKey>(UndoRedoSetFieldDictValueByKey inst);
template void SdfCommandGroup::StoreInstruction<UndoRedoSetTimeSample>(UndoRedoSetTimeSample inst);
template void SdfCommandGroup::StoreInstruction<UndoRedoCreateSpec>(UndoRedoCreateSpec inst);
template void SdfCommandGroup::StoreInstruction<UndoRedoDeleteSpec>(UndoRedoDeleteSpec inst);
template void SdfCommandGroup::StoreInstruction<UndoRedoMoveSpec>(UndoRedoMoveSpec inst);
template void SdfCommandGroup::StoreInstruction<UndoRedoPushChild<TfToken>>(UndoRedoPushChild<TfToken> inst);
template void SdfCommandGroup::StoreInstruction<UndoRedoPushChild<SdfPath>>(UndoRedoPushChild<SdfPath> inst);
template void SdfCommandGroup::StoreInstruction<UndoRedoPopChild<TfToken>>(UndoRedoPopChild<TfToken> inst);
template void SdfCommandGroup::StoreInstruction<UndoRedoPopChild<SdfPath>>(UndoRedoPopChild<SdfPath> inst);

// Call all the functions stored in _commands in reverse order
void SdfCommandGroup::UndoIt() {
    SdfChangeBlock block;
    for (auto &_instruction : std::ranges::reverse_view(_instructions)) {
        _instruction.UndoIt();
    }
}

void SdfCommandGroup::DoIt() {
    SdfChangeBlock block;
    for (auto &cmd : _instructions) {
        cmd.DoIt();
    }
}

}// namespace vox