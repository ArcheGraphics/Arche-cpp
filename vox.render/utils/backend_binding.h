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

#ifndef UTILS_BACKENDBINDING_H_
#define UTILS_BACKENDBINDING_H_

#include <webgpu/webgpu_cpp.h>
#include <dawn/native/DawnNative.h>

struct GLFWwindow;

namespace vox {
class BackendBinding {
public:
    virtual ~BackendBinding() = default;
    
    virtual uint64_t swapChainImplementation() = 0;
    
    virtual wgpu::TextureFormat preferredSwapChainTextureFormat() = 0;
    
    wgpu::Device& device();
    
protected:
    BackendBinding(GLFWwindow* window, wgpu::Device& device);
    
    GLFWwindow* _window = nullptr;
    wgpu::Device& _device;
};

void discoverAdapter(dawn::native::Instance* instance,
                     GLFWwindow* window,
                     wgpu::BackendType type);
std::unique_ptr<BackendBinding> createBinding(wgpu::BackendType type, GLFWwindow* window, wgpu::Device& device);

}  // namespace vox

#endif  // UTILS_BACKENDBINDING_H_
