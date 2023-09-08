//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "core/cuda_stream.h"

namespace vox::compute {
class SineWaveSimulation {
public:
    SineWaveSimulation(size_t width, size_t height, CudaDevice &device);
    void step_simulation(float time, float *heightMap, CudaStream &stream) const;

    [[nodiscard]] CudaDevice &get_device() const { return device; }
    [[nodiscard]] size_t get_width() const { return m_width; }
    [[nodiscard]] size_t get_height() const { return m_height; }

private:
    CudaDevice &device;
    size_t m_width, m_height;
    int m_blocks, m_threads;
};

}// namespace vox::compute