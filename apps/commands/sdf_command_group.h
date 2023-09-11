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
        _ref->show_it();
    }

    void do_it() const {
        _ref->do_it();
    }

    void undo_it() const {
        _ref->undo_it();
    }

    void show_it() const {
        _ref->show_it();
    }

    struct Interface {
        virtual ~Interface() = default;
        virtual void do_it() = 0;
        virtual void undo_it() = 0;
        virtual void show_it() = 0;
    };

    template<typename InstructionT>
    struct Storage : Interface {
        explicit Storage(InstructionT &&inst) : _data(std::move(inst)) {
        }
        ~Storage() override = default;

        void do_it() override {
            _data.do_it();
        }

        void undo_it() override {
            _data.undo_it();
        }

        void show_it() override {}

        InstructionT _data;
    };

    std::unique_ptr<Interface> _ref;
};

class SdfCommandGroup {
public:
    SdfCommandGroup() = default;
    ~SdfCommandGroup() = default;

    /// Was it recorded
    [[nodiscard]] bool is_empty() const;
    void clear();

    /// Run the commands as an undo
    void do_it();
    void undo_it();

    template<typename InstructionT>
    void store_instruction(InstructionT);

private:
    std::vector<InstructionWrapper> _instructions;
};

}// namespace vox