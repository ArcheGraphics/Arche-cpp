//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "cuda_stream.h"
#include "utils/helper_cuda.h"

namespace vox::compute {
CudaStream::CudaStream(CudaDevice &device) {
    checkCudaErrors(
        cudaStreamCreateWithFlags(&m_stream, cudaStreamNonBlocking));
}

CudaStream::~CudaStream() {
    // Make sure there's no pending work before we start tearing down
    checkCudaErrors(cudaStreamSynchronize(m_stream));
}

cudaStream_t CudaStream::get_handle() {
    return m_stream;
}

}// namespace vox::compute