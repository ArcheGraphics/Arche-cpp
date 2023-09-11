//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <vector>
#include <functional>
#include <memory>
#include <iostream>

namespace vox {
class InstructionWrapper {
public:
    template<typename InstructionT>
    explicit InstructionWrapper(InstructionT &&instruction)
        : _ref(std::make_unique<Storage<InstructionT>>(std::forward<InstructionT>(instruction))) {
        _ref->ShowIt();
    }

    void DoIt() const {
        _ref->DoIt();
    }

    void UndoIt() const {
        _ref->UndoIt();
    }

    void ShowIt() const {
        _ref->ShowIt();
    }

    struct Interface {
        virtual ~Interface() = default;
        virtual void DoIt() = 0;
        virtual void UndoIt() = 0;
        virtual void ShowIt() = 0;
    };

    template<typename InstructionT>
    struct Storage : Interface {
        explicit Storage(InstructionT &&inst) : _data(std::move(inst)) {
        }
        ~Storage() override = default;
        void DoIt() override {
            _data.DoIt();
        }
        void UndoIt() override {
            _data.UndoIt();
        }

        void ShowIt() override {}

        InstructionT _data;
    };

    std::unique_ptr<Interface> _ref;
};

class SdfCommandGroup {

public:
    SdfCommandGroup() = default;
    ~SdfCommandGroup() = default;

    /// Was it recorded
    [[nodiscard]] bool IsEmpty() const;
    void Clear();

    /// Run the commands as an undo
    void DoIt();
    void UndoIt();

    template<typename InstructionT>
    void StoreInstruction(InstructionT);

private:
    std::vector<InstructionWrapper> _instructions;
};

}// namespace vox