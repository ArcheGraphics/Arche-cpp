//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "gpu_counter.h"

#include "common/logging.h"
#include "common/metal_helpers.h"

namespace vox::compute {
GPUCounter::GPUCounter(MTL::Device &device, uint32_t sampleCount,
                       MTL::CommonCounterSet counterSetName)
    : device{device} {
    auto sampleBufferDesc = make_shared(MTL::CounterSampleBufferDescriptor::alloc()->init());
    sampleBufferDesc->setStorageMode(MTL::StorageModeShared);
    sampleBufferDesc->setSampleCount(sampleCount);
    sampleBufferDesc->setCounterSet(get_counter_set(counterSetName));

    NS::Error *error{nullptr};
    buffer = make_shared(device.newCounterSampleBuffer(sampleBufferDesc.get(), &error));
    if (error != nullptr) {
        LOGE("Error: could not create sample buffer: {}",
             error->description()->cString(NS::StringEncoding::UTF8StringEncoding));
    }
}

MTL::CounterSet *GPUCounter::get_counter_set(MTL::CommonCounterSet counterSetName) {
    auto count = device.counterSets()->count();
    for (int i = 0; i < count; ++i) {
        auto counterSet = static_cast<MTL::CounterSet *>(device.counterSets()->object(i));
        if (counterSetName->isEqual(counterSet->name())) {
//            LOGI("GPU device {} supports the {} counter set.",
//                 device.name()->utf8String(), counterSetName->utf8String());
            return counterSet;
        }
    }

//    LOGE("GPU device {} doesn't support the {} counter set.",
//         device.name()->utf8String(), counterSetName->utf8String());
    return nullptr;
}

bool GPUCounter::is_counter_support(MTL::CounterSet *counterSet, MTL::CommonCounter counterName) {
    auto count = counterSet->counters()->count();
    for (int i = 0; i < count; ++i) {
        auto counter = static_cast<MTL::Counter *>(counterSet->counters()->object(i));
        if (counterName->isEqual(counter->name())) {
//            LOGI("Counter set {} supports the {} counter set.",
//                 counterSet->name()->utf8String(), counterName->utf8String());
            return true;
        }
    }

//    LOGE("Counter set {} doesn't support the {} counter set.",
//         counterSet->name()->utf8String(), counterName->utf8String());
    return false;
}

std::vector<MTL::CounterSamplingPoint> GPUCounter::sampling_boundaries() {
    std::array allBoundaries = {
        MTL::CounterSamplingPointAtStageBoundary,
        MTL::CounterSamplingPointAtDrawBoundary,
        MTL::CounterSamplingPointAtBlitBoundary,
        MTL::CounterSamplingPointAtDispatchBoundary,
        MTL::CounterSamplingPointAtTileDispatchBoundary};

    std::vector<MTL::CounterSamplingPoint> boundaries;
    for (auto boundary : allBoundaries) {
        if (device.supportsCounterSampling(boundary)) {
            // Add the boundary to the return-value array.
            boundaries.push_back(boundary);
        }
    }

    return boundaries;
}

std::shared_ptr<MTL::CounterSampleBuffer> GPUCounter::get_handle() {
    return buffer;
}

void GPUCounter::update_start_times() {
    // Save the current CPU and GPU times as a baseline.
    MTL::Timestamp cpuStartTimestamp = 0;
    MTL::Timestamp gpuStartTimestamp = 0;

    device.sampleTimestamps(&cpuStartTimestamp, &gpuStartTimestamp);
    cpuStart = (double)cpuStartTimestamp;
    gpuStart = (double)gpuStartTimestamp;
}

void GPUCounter::update_final_times() {
    // Update the final times with the current CPU and GPU times.
    MTL::Timestamp cpuFinalTimestamp = 0;
    MTL::Timestamp gpuFinalTimestamp = 0;

    device.sampleTimestamps(&cpuFinalTimestamp, &gpuFinalTimestamp);

    cpuTimeSpan = (double)cpuFinalTimestamp - cpuStart;
    gpuTimeSpan = (double)gpuFinalTimestamp - gpuStart;
}

double GPUCounter::calculate_elapsed_seconds_between(uint32_t begin, uint32_t end) {
    /// Represents the size of the counter sample buffer.
    NS::Range range = NS::Range::Make(begin, end + 1);
    // Convert the contents of the counter sample buffer into the standard data format.
    auto data = buffer->resolveCounterRange(range);
    auto sampleCount = end - begin + 1;

    auto resolvedSampleCount = data->length() / sizeof(MTL::CounterResultTimestamp);
    if (resolvedSampleCount < sampleCount) {
        LOGW("Only {} out of {} timestamps resolved.", resolvedSampleCount, sampleCount);
        return 0;
    }

    // Cast the data's bytes property to the counter's result type.
    auto *timestamps = (MTL::CounterResultTimestamp *)(data->mutableBytes());
    return micro_seconds_between(timestamps[begin].timestamp, timestamps[end].timestamp) / 1000.0;
}

double GPUCounter::absolute_time_in_microseconds(MTL::Timestamp timestamp) const {
    // Convert the GPU time to a value within the range [0.0, 1.0].
    double normalizedGpuTime = ((double)timestamp - gpuStart);
    normalizedGpuTime /= gpuTimeSpan;

    // Convert GPU time to CPU time.
    double nanoseconds = (normalizedGpuTime * cpuTimeSpan);
    nanoseconds += cpuStart;

    double microseconds = nanoseconds / 1000.0;
    return microseconds;
}

double GPUCounter::micro_seconds_between(MTL::Timestamp begin, MTL::Timestamp end) const {
    double timeSpan = (double)end - (double)begin;

    // Convert GPU time to CPU time.
    double nanoseconds = timeSpan / gpuTimeSpan * cpuTimeSpan;

    double microseconds = nanoseconds / 1000.0;
    return microseconds;
}

}// namespace vox::compute