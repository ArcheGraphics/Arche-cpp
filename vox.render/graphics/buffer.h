//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef buffer_hpp
#define buffer_hpp

#include <webgpu/webgpu_cpp.h>
#include <vector>

namespace vox {
class Buffer {
public:
    /**
     * Create Buffer.
     * @param device - Device
     * @param byteLength - Byte length
     * @param bufferUsage - Buffer usage
     */
    Buffer(wgpu::Device& device, uint64_t byteLength, wgpu::BufferUsage bufferUsage);
    
    /**
     * Create Buffer.
     * @param device - Device
     * @param data - Byte
     * @param bufferUsage - Buffer usage
     */
    Buffer(wgpu::Device& device, const void* data,
           uint64_t size, wgpu::BufferUsage bufferUsage);
    
    void uploadData(wgpu::Device& device, const void* data,
                    uint64_t size, uint64_t bufferOffset = 0);
    
    Buffer(Buffer const& other) = default;
    
    Buffer(Buffer&& other) = default;
    
    Buffer& operator=(Buffer const& other) = default;
    
    Buffer& operator=(Buffer&& other) = default;
    
    const wgpu::Buffer& handle() const;
    
    uint64_t size() const;

private:
    wgpu::Buffer _nativeBuffer;
    uint64_t _size{};
};

}

#endif /* buffer_hpp */
