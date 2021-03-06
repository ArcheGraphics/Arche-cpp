// Copyright 2017 The Dawn Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "backend_binding.h"

#include <dawn/native/NullBackend.h>

#include <memory>

namespace vox {
class NullBinding : public BackendBinding {
public:
    NullBinding(GLFWwindow* window, wgpu::Device& device) : BackendBinding(window, device) {
    }
    
    uint64_t swapChainImplementation() override {
        if (_swapchainImpl.userData == nullptr) {
            _swapchainImpl = dawn::native::null::CreateNativeSwapChainImpl();
        }
        return reinterpret_cast<uint64_t>(&_swapchainImpl);
    }
    wgpu::TextureFormat preferredSwapChainTextureFormat() override {
        return wgpu::TextureFormat::RGBA8Unorm;
    }
    
private:
    DawnSwapChainImplementation _swapchainImpl = {};
};

std::unique_ptr<BackendBinding> createNullBinding(GLFWwindow* window, wgpu::Device& device) {
    return std::make_unique<NullBinding>(window, device);
}

}  // namespace vox
