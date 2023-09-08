//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "cuda_device.h"

namespace vox::compute {
class CudaStream {
public:
    explicit CudaStream(CudaDevice &device);

    ~CudaStream();

    cudaStream_t get_handle();

private:
    cudaStream_t m_stream{};
};
}// namespace vox::compute