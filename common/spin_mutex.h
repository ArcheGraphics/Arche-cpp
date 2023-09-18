//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <atomic>
#include <thread>

namespace vox {

class spin_mutex {

private:
    std::atomic_flag _flag;// ATOMIC_FLAG_INIT is not needed as per C++20

public:
    spin_mutex() noexcept = default;

    void lock() noexcept {
        while (_flag.test_and_set(std::memory_order::acquire)) {// acquire lock
            std::this_thread::yield();
        }
    }
    bool try_lock() noexcept {
        return !_flag.test_and_set(std::memory_order::acquire);
    }
    void unlock() noexcept {
        _flag.clear(std::memory_order::release);// release lock
    }
};

}// namespace vox

