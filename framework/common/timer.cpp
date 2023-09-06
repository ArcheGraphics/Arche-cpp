//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "timer.h"

namespace vox {
Timer::Timer() : start_time{Clock::now()},
                 previous_tick{Clock::now()} {
}

void Timer::start() {
    if (!running) {
        running = true;
        start_time = Clock::now();
    }
}

void Timer::lap() {
    lapping = true;
    lap_time = Clock::now();
}

bool Timer::is_running() const {
    return running;
}
}// namespace vox
