//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "shader/shader_uniform.h"
#include <Metal/Metal.hpp>

namespace vox {
class ComputePipelineState {
public:
    std::vector<ShaderUniform> uniformBlock{};

    ComputePipelineState(MTL::Device *device, const MTL::ComputePipelineDescriptor &descriptor);

    const MTL::ComputePipelineState &handle() const {
        return *_handle;
    }

private:
    /**
     * record the location of uniform/attribute.
     */
    void _recordVertexLocation(MTL::ComputePipelineReflection *reflection);

    MTL::Device *_device;
    std::shared_ptr<MTL::ComputePipelineState> _handle{nullptr};
};

}// namespace vox