//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <Metal/Metal.hpp>

namespace vox::compute {
class GPUCounter {
public:
    GPUCounter(MTL::Device& device, uint32_t sampleCount,
               MTL::CommonCounterSet counterSetName = MTL::CommonCounterSetTimestamp);

    static bool is_counter_support(MTL::CounterSet *counterSet, MTL::CommonCounter counterName);

    MTL::CounterSet *get_counter_set(MTL::CommonCounterSet counterSetName);

    std::vector<MTL::CounterSamplingPoint> sampling_boundaries();

public:
    std::shared_ptr<MTL::CounterSampleBuffer> get_handle();

    void update_start_times();

    void update_final_times();

    double calculate_elapsed_seconds_between(uint32_t begin, uint32_t end);

private:
    [[nodiscard]] double absolute_time_in_microseconds(MTL::Timestamp timestamp) const;

    [[nodiscard]] double micro_seconds_between(MTL::Timestamp begin, MTL::Timestamp end) const;

private:
    MTL::Device& device;
    std::shared_ptr<MTL::CounterSampleBuffer> buffer;

    double cpuStart{};
    double gpuStart{};
    double cpuTimeSpan{};
    double gpuTimeSpan{};
};

}// namespace vox::compute