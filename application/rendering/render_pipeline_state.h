//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef render_pipeline_state_hpp
#define render_pipeline_state_hpp

#include "shader/shader_uniform.h"
#include "shader/shader_data_group.h"
#include <Metal/Metal.hpp>
#include <optional>

namespace vox {
class RenderPipelineState {
public:
    std::vector<ShaderUniform> sceneUniformBlock{};
    std::vector<ShaderUniform> cameraUniformBlock{};
    std::vector<ShaderUniform> rendererUniformBlock{};
    std::vector<ShaderUniform> materialUniformBlock{};

    RenderPipelineState(MTL::Device *device, const MTL::RenderPipelineDescriptor &descriptor);

    const MTL::RenderPipelineState &handle() const {
        return *_handle;
    }

private:
    /**
     * record the location of uniform/attribute.
     */
    void _recordVertexLocation(MTL::RenderPipelineReflection *reflection);

    void _groupingUniform(const ShaderUniform &uniform,
                          const std::optional<ShaderDataGroup> &group);

    MTL::Device *_device;
    std::shared_ptr<MTL::RenderPipelineState> _handle{nullptr};
};

}// namespace vox

#endif /* render_pipeline_state_hpp */
