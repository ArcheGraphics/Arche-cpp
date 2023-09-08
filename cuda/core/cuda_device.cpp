//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "cuda_device.h"
#include "utils/helper_cuda.h"
#include "common/logging.h"

namespace vox::compute {
CudaDevice::CudaDevice(uint8_t *vkDeviceUUID, size_t UUID_SIZE) {
    int current_device = 0;
    int device_count = 0;
    int devices_prohibited = 0;

    cudaDeviceProp deviceProp{};
    checkCudaErrors(cudaGetDeviceCount(&device_count));

    if (device_count == 0) {
        LOGE("CUDA error: no devices supporting CUDA.");
    }

    // Find the GPU which is selected by Vulkan
    while (current_device < device_count) {
        cudaGetDeviceProperties(&deviceProp, current_device);

        if ((deviceProp.computeMode != cudaComputeModeProhibited)) {
            // Compare the cuda device UUID with vulkan UUID
            int ret = memcmp((void *)&deviceProp.uuid, vkDeviceUUID, UUID_SIZE);
            if (ret == 0) {
                checkCudaErrors(cudaSetDevice(current_device));
                checkCudaErrors(cudaGetDeviceProperties(&deviceProp, current_device));
                LOGI("GPU Device {}: {} with compute capability {}.{}",
                     current_device, deviceProp.name, deviceProp.major, deviceProp.minor);
                cuda_device = current_device;
            }

        } else {
            devices_prohibited++;
        }

        current_device++;
    }

    if (devices_prohibited == device_count) {
        LOGE("CUDA error:  No Vulkan-CUDA Interop capable GPU found.")
    }

    checkCudaErrors(cudaSetDevice(cuda_device));
    checkCudaErrors(cudaGetDeviceProperties(&prop, cuda_device));
}

const cudaDeviceProp &CudaDevice::get_prop() const {
    return prop;
}

}// namespace vox::compute