//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <benchmark/benchmark.h>
#include <Metal/Metal.hpp>
#include "common/metal_helpers.h"
#include "mad_throughput.h"

int main(int argc, char **argv) {
    ::benchmark::Initialize(&argc, argv);
    auto device = vox::make_shared(MTL::CreateSystemDefaultDevice());
    auto queue = vox::make_shared(device->newCommandQueue());

    vox::LatencyMeasureMode mode = vox::LatencyMeasureMode::kGpuTimestamp;
    auto app = std::make_unique<vox::benchmark::MADThroughPut>();
    app->register_benchmarks(queue, mode);

    ::benchmark::RunSpecifiedBenchmarks();
}