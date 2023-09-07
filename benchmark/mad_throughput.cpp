//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "mad_throughput.h"
#include "compute/buffer_utils.h"
#include "compute/status_util.h"
#include "compute/data_type_util.h"
#include "framework/common/metal_helpers.h"
#include "framework/common/filesystem.h"
#include <spdlog/fmt/fmt.h>
#include <gtest/gtest.h>

namespace vox::benchmark {
static void throughput(::benchmark::State &state,
                       std::shared_ptr<MTL::CommandQueue> queue,
                       std::shared_ptr<MTL::Function> function,
                       size_t num_element, int loop_count, compute::DataType data_type) {
    auto *device = queue->device();
    //===-------------------------------------------------------------------===/
    // Create buffers
    //===-------------------------------------------------------------------===/
    const size_t src0_size = num_element * get_size(data_type);
    const size_t src1_size = num_element * get_size(data_type);
    const size_t dst_size = num_element * get_size(data_type);

    auto src0_buffer = CLONE_METAL_CUSTOM_DELETER(MTL::Buffer, device->newBuffer(src0_size, MTL::ResourceStorageModePrivate));
    auto src1_buffer = CLONE_METAL_CUSTOM_DELETER(MTL::Buffer, device->newBuffer(src1_size, MTL::ResourceStorageModePrivate));
    auto dst_buffer = CLONE_METAL_CUSTOM_DELETER(MTL::Buffer, device->newBuffer(dst_size, MTL::ResourceStorageModePrivate));

    //===-------------------------------------------------------------------===/
    // Set source buffer data
    //===-------------------------------------------------------------------===/
    auto getSrc0 = [](size_t i) {
        float v = float((i % 9) + 1) * 0.1f;
        return v;
    };
    auto getSrc1 = [](size_t i) {
        float v = float((i % 5) + 1) * 1.f;
        return v;
    };

    if (data_type == compute::DataType::fp16) {
        compute::set_device_buffer_via_staging_buffer(
            *device, *src0_buffer, src0_size, [&](void *ptr, size_t num_bytes) {
                auto *src_float_buffer = reinterpret_cast<uint16_t *>(ptr);
                for (size_t i = 0; i < num_element; i++) {
                    src_float_buffer[i] = compute::fp16(getSrc0(i)).get_value();
                }
            });

        compute::set_device_buffer_via_staging_buffer(
            *device, *src1_buffer, src1_size, [&](void *ptr, size_t num_bytes) {
                auto *src_float_buffer = reinterpret_cast<uint16_t *>(ptr);
                for (size_t i = 0; i < num_element; i++) {
                    src_float_buffer[i] = compute::fp16(getSrc1(i)).get_value();
                }
            });
    } else if (data_type == compute::DataType::fp32) {
        compute::set_device_buffer_via_staging_buffer(
            *device, *src0_buffer, src0_size, [&](void *ptr, size_t num_bytes) {
                auto *src_float_buffer = reinterpret_cast<float *>(ptr);
                for (size_t i = 0; i < num_element; i++) {
                    src_float_buffer[i] = getSrc0(i);
                }
            });

        compute::set_device_buffer_via_staging_buffer(
            *device, *src1_buffer, src1_size, [&](void *ptr, size_t num_bytes) {
                auto *src_float_buffer = reinterpret_cast<float *>(ptr);
                for (size_t i = 0; i < num_element; i++) {
                    src_float_buffer[i] = getSrc1(i);
                }
            });
    }

    //===-------------------------------------------------------------------===/
    // Dispatch
    //===-------------------------------------------------------------------===/
    NS::Error *error{nullptr};
    auto pso = CLONE_METAL_CUSTOM_DELETER(MTL::ComputePipelineState, device->newComputePipelineState(function.get(), &error));
    auto w = pso->threadExecutionWidth();
    if (error != nullptr) {
        LOGE("Error: could not create pso: {}",
             error->description()->cString(NS::StringEncoding::UTF8StringEncoding));
    }

    {
        auto commandBuffer = CLONE_METAL_CUSTOM_DELETER(MTL::CommandBuffer, queue->commandBuffer());
        auto encoder = commandBuffer->computeCommandEncoder();

        encoder->setComputePipelineState(pso.get());
        encoder->setBuffer(src0_buffer.get(), 0, 0);
        encoder->setBuffer(src1_buffer.get(), 0, 1);
        encoder->setBuffer(dst_buffer.get(), 0, 2);
        encoder->dispatchThreads({(uint32_t)num_element / 4, 1, 1}, {w, 1, 1});

        encoder->endEncoding();
        commandBuffer->commit();
        commandBuffer->waitUntilCompleted();
    }

    //===-------------------------------------------------------------------===/
    // Verify destination buffer data
    //===-------------------------------------------------------------------===/

    if (data_type == compute::DataType::fp16) {
        compute::get_device_buffer_via_staging_buffer(
            *device, *dst_buffer, dst_size, [&](void *ptr, size_t num_bytes) {
                auto *dst_float_buffer = reinterpret_cast<uint16_t *>(ptr);
                for (size_t i = 0; i < num_element; i++) {
                    float limit = getSrc1(i) * (1.f / (1.f - getSrc0(i)));
                    BM_CHECK_FLOAT_EQ(compute::fp16(dst_float_buffer[i]).to_float(), limit, 0.5f)
                        << "destination buffer element #" << i
                        << " has incorrect value: expected to be " << limit
                        << " but found " << compute::fp16(dst_float_buffer[i]).to_float();
                }
            });
    } else if (data_type == compute::DataType::fp32) {
        compute::get_device_buffer_via_staging_buffer(
            *device, *dst_buffer, dst_size, [&](void *ptr, size_t num_bytes) {
                auto *dst_float_buffer = reinterpret_cast<float *>(ptr);
                for (size_t i = 0; i < num_element; i++) {
                    float limit = getSrc1(i) * (1.f / (1.f - getSrc0(i)));
                    BM_CHECK_FLOAT_EQ(dst_float_buffer[i], limit, 0.01f)
                        << "destination buffer element #" << i
                        << " has incorrect value: expected to be " << limit
                        << " but found " << dst_float_buffer[i];
                }
            });
    }

    //===-------------------------------------------------------------------===/
    // Benchmarking
    //===-------------------------------------------------------------------===/

    {
        for (auto _ : state) {
            auto commandBuffer = CLONE_METAL_CUSTOM_DELETER(MTL::CommandBuffer, queue->commandBuffer());
            auto encoder = CLONE_METAL_CUSTOM_DELETER(MTL::ComputeCommandEncoder, commandBuffer->computeCommandEncoder());

            encoder->setComputePipelineState(pso.get());
            encoder->setBuffer(src0_buffer.get(), 0, 0);
            encoder->setBuffer(src1_buffer.get(), 0, 1);
            encoder->setBuffer(dst_buffer.get(), 0, 2);
            encoder->dispatchThreads({(uint32_t)num_element / 4, 1, 1}, {w, 1, 1});

            encoder->endEncoding();

            auto start_time = std::chrono::high_resolution_clock::now();

            commandBuffer->commit();
            commandBuffer->waitUntilCompleted();

            auto end_time = std::chrono::high_resolution_clock::now();
            auto elapsed_seconds = std::chrono::duration_cast<std::chrono::duration<double>>(end_time - start_time);

            state.SetIterationTime(elapsed_seconds.count());
        }
        double numOperation = double(num_element) * 2. /*fma*/ *
                              10. /*10 elements per loop iteration*/ *
                              double(loop_count);
        state.counters["FLOps"] =
            ::benchmark::Counter(numOperation,
                                 ::benchmark::Counter::kIsIterationInvariant |
                                     ::benchmark::Counter::kIsRate,
                                 ::benchmark::Counter::kIs1000);
    }
}

void MADThroughPut::register_benchmarks(std::shared_ptr<MTL::CommandQueue> &queue) {
    const char *gpu_name = queue->device()->name()->cString(NS::UTF8StringEncoding);

    auto raw_source = fs::read_shader("compute/mad_throughput.metal");
    auto source = NS::String::string(raw_source.c_str(), NS::UTF8StringEncoding);
    NS::Error *error{nullptr};
    auto option = CLONE_METAL_CUSTOM_DELETER(MTL::CompileOptions, MTL::CompileOptions::alloc()->init());
    _library = CLONE_METAL_CUSTOM_DELETER(MTL::Library, queue->device()->newLibrary(source, option.get(), &error));
    if (error != nullptr) {
        LOGE("Error: could not load Metal shader library: {}",
             error->description()->cString(NS::StringEncoding::UTF8StringEncoding));
    }

    const size_t num_element = 1024 * 1024;
    const int min_loop_count = 100000;
    const int max_loop_count = min_loop_count * 2;

    for (int loop_count = min_loop_count; loop_count <= max_loop_count;
         loop_count += min_loop_count) {
        error->release();
        error = nullptr;

        std::string test_name = fmt::format("{}/{}/{}/{}", gpu_name, "mad_throughput", num_element, loop_count);

        auto constantValue = CLONE_METAL_CUSTOM_DELETER(MTL::FunctionConstantValues, MTL::FunctionConstantValues::alloc()->init());
        constantValue->setConstantValue(&loop_count, MTL::DataTypeInt, NS::UInteger(0));
        auto functionName = NS::String::string("mad_throughput", NS::UTF8StringEncoding);
        auto function = CLONE_METAL_CUSTOM_DELETER(MTL::Function, _library->newFunction(functionName, constantValue.get(), &error));
        if (error != nullptr) {
            LOGE("Error: could not create function: {}",
                 error->description()->cString(NS::StringEncoding::UTF8StringEncoding));
        }

        ::benchmark::RegisterBenchmark(test_name, throughput, queue, function,
                                       num_element, loop_count, compute::DataType::fp32)
            ->UseManualTime()
            ->Unit(::benchmark::kMicrosecond)
            ->MinTime(std::numeric_limits<float>::epsilon());// use cache make calculation fast after warmup
    }
}

}// namespace vox::benchmark