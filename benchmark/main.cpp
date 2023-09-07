//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <benchmark/benchmark.h>
#include <Metal/Metal.hpp>
#include "framework/common/metal_helpers.h"
#include "mad_throughput.h"

using namespace vox::compute;

int main(int argc, char **argv) {
    ::benchmark::Initialize(&argc, argv);
    auto device = CLONE_METAL_CUSTOM_DELETER(MTL::Device, MTL::CreateSystemDefaultDevice());
    auto queue = CLONE_METAL_CUSTOM_DELETER(MTL::CommandQueue, device->newCommandQueue());

    size_t device_index = 0;
    auto app = std::make_unique<vox::benchmark::MADThroughPut>();
    app->register_benchmarks(queue);

    ::benchmark::RunSpecifiedBenchmarks();
}