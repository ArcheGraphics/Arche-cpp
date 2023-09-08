//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <cuda.h>
#include <curand.h>
#include <curand_kernel.h>
#include "core/cuda_stream.h"
#include "utils/helper_multiprocess.h"

typedef float vec2[2];

namespace vox::compute {
class MonteCarloPiSimulation {
public:
    explicit MonteCarloPiSimulation(size_t num_points, CudaDevice &device);

    ~MonteCarloPiSimulation();

    void step_simulation(float time, CudaStream &stream);
    static void compute_pi_callback(void *args);

    [[nodiscard]] size_t get_num_points() const { return m_numPoints; }

    [[nodiscard]] float get_num_points_in_circle() const { return *m_hostNumPointsInCircle; }

    ShareableHandle &get_position_shareable_handle() { return m_posShareableHandle; }

    ShareableHandle &get_in_circle_shareable_handle() { return m_inCircleShareableHandle; }

private:
    size_t m_numPoints;

    // Pointers to Cuda allocated buffers which are imported and used by vulkan as
    // vertex buffer
    vec2 *m_xyVector;
    float *m_pointsInsideCircle;

    // Pointers to device and host allocated memories storing number of points
    // that are inside the unit circle
    float *m_numPointsInCircle{};
    float *m_hostNumPointsInCircle{};

    int m_blocks{}, m_threads{};

    // Total size of allocations created by cuMemMap Apis. This size is the sum of
    // sizes of m_xyVector and m_pointsInsideCircle buffers.
    size_t m_totalAllocationSize{};

    // Shareable Handles(a file descriptor on Linux and NT Handle on Windows),
    // used for sharing cuda
    // allocated memory with Vulkan
    ShareableHandle m_posShareableHandle{}, m_inCircleShareableHandle{};

    // Track and accumulate total points that have been simulated since start of
    // the sample. The idea is to get a closer approximation to PI with time.
    size_t m_totalPointsInsideCircle;
    size_t m_totalPointsSimulated;

    void setup_simulation_allocations(CudaDevice &device);
    void cleanup_simulation_allocations();
};
}// namespace vox::compute