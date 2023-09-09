//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <Metal/Metal.hpp>
#include <benchmark/benchmark.h>

namespace vox {
enum class LatencyMeasureMode {
    // time spent from queue submit to returning from queue wait
    kSystemSubmit,
    // Timestamp difference measured on GPU
    kGpuTimestamp,
};

class BenchmarkAPI {
public:
    // Registers all Vulkan benchmarks for the current benchmark binary.
    //
    // The |overhead_seconds| field in |latency_measure| should subtracted from the
    // latency measured by the registered benchmarks for
    // LatencyMeasureMode::kSystemDispatch.
    virtual void register_benchmarks(std::shared_ptr<MTL::CommandQueue> &queue, LatencyMeasureMode mode) = 0;
};
}// namespace vox