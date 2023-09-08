//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "sine_wave_simulation.h"
#include "utils/helper_cuda.h"

namespace vox::compute {
__global__ void sinewave(float *heightMap, unsigned int width,
                         unsigned int height, float time) {
    const float freq = 4.0f;
    const size_t stride = gridDim.x * blockDim.x;

    // Iterate through the entire array in a way that is
    // independent of the grid configuration
    for (size_t tid = blockIdx.x * blockDim.x + threadIdx.x; tid < width * height;
         tid += stride) {
        // Calculate the x, y coordinates
        const size_t y = tid / width;
        const size_t x = tid - y * width;
        // Normalize x, y to [0,1]
        const float u = ((2.0f * x) / width) - 1.0f;
        const float v = ((2.0f * y) / height) - 1.0f;
        // Calculate the new height value
        const float w = 0.5f * sinf(u * freq + time) * cosf(v * freq + time);
        // Store this new height value
        heightMap[tid] = w;
    }
}

SineWaveSimulation::SineWaveSimulation(size_t width, size_t height, CudaDevice &device)
    : m_width(width), m_height(height), device(device) {
    // We don't need large block sizes, since there's not much inter-thread
    // communication
    m_threads = device.get_prop().warpSize;

    // Use the occupancy calculator and fill the gpu as best as we can
    checkCudaErrors(cudaOccupancyMaxActiveBlocksPerMultiprocessor(
        &m_blocks, sinewave, device.get_prop().warpSize, 0));
    m_blocks *= device.get_prop().multiProcessorCount;

    // Go ahead and the clamp the blocks to the minimum needed for this
    // height/width
    m_blocks = std::min(m_blocks, (int)((m_width * m_height + m_threads - 1) / m_threads));
}

void SineWaveSimulation::step_simulation(float time, float *heights, CudaStream &stream) const {
    sinewave<<<m_blocks, m_threads, 0, stream.get_handle()>>>(heights, m_width, m_height, time);
    getLastCudaError("Failed to launch CUDA simulation");
}

}// namespace vox::compute