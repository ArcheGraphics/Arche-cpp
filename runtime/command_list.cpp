#include "runtime/rhi/command.h"
#include "runtime/command_list.h"
#include "common/logging.h"

namespace vox::compute {

CommandList::~CommandList() noexcept {
    ASSERT(_committed || empty(),
           "Destructing non-empty command list. "
           "Did you forget to commit?");
}

void CommandList::reserve(size_t command_size, size_t callback_size) noexcept {
    if (command_size) { _commands.reserve(command_size); }
    if (callback_size) { _callbacks.reserve(callback_size); }
}

void CommandList::clear() noexcept {
    _commands.clear();
    _callbacks.clear();
    _committed = false;
}

CommandList &CommandList::append(vox::unique_ptr<Command> &&cmd) noexcept {
    if (cmd) { _commands.emplace_back(std::move(cmd)); }
    return *this;
}

CommandList &CommandList::add_callback(std::function<void()> &&callback) noexcept {
    if (callback) {
        if (_callbacks.empty()) [[likely]] { _callbacks.reserve(2); }
        _callbacks.emplace_back(std::move(callback));
    }
    return *this;
}

CommandList &CommandList::operator<<(std::unique_ptr<Command> &&cmd) noexcept {
    return append(std::move(cmd));
}

CommandList::CallbackContainer CommandList::steal_callbacks() noexcept {
    return std::move(_callbacks);
}

CommandList::CommandContainer CommandList::steal_commands() noexcept {
    return std::move(_commands);
}

CommandList CommandList::create(size_t reserved_command_size, size_t reserved_callback_size) noexcept {
    CommandList list{};
    list.reserve(reserved_command_size, reserved_callback_size);
    return list;
}

CommandList::Commit CommandList::commit() noexcept {
    _committed = true;
    return Commit{std::move(*this)};
}

CommandList::CommandList(CommandList &&another) noexcept
    : _commands{std::move(another._commands)},
      _callbacks{std::move(another._callbacks)},
      _committed{another._committed} { another._committed = false; }

}// namespace vox::compute
