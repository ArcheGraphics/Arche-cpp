//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "shader/shader_uniform.h"
#include <Metal/Metal.hpp>
#include <optional>

namespace vox {
class RenderPipelineState {
public:
    std::vector<ShaderUniform> uniformBlock{};

    RenderPipelineState(MTL::Device *device, const MTL::RenderPipelineDescriptor &descriptor);

    const MTL::RenderPipelineState &handle() const {
        return *_handle;
    }

private:
    /**
     * record the location of uniform/attribute.
     */
    void _recordVertexLocation(MTL::RenderPipelineReflection *reflection);

    MTL::Device *_device;
    std::shared_ptr<MTL::RenderPipelineState> _handle{nullptr};
};

}// namespace vox