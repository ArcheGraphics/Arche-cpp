//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "render_pipeline_state.h"
#include "framework/common/metal_helpers.h"
#include "shader/shader.h"

namespace vox {
RenderPipelineState::RenderPipelineState(MTL::Device *device, const MTL::RenderPipelineDescriptor &descriptor) : _device(device) {
    MTL::RenderPipelineReflection *_reflection{nullptr};
    NS::Error *error{nullptr};

    _handle = CLONE_METAL_CUSTOM_DELETER(MTL::RenderPipelineState,
                                         _device->newRenderPipelineState(&descriptor, MTL::PipelineOptionArgumentInfo,
                                                                         &_reflection, &error));

    if (error != nullptr) {
        LOG(ERROR) << "Error: failed to create Metal pipeline state: "
                   << error->description()->cString(NS::StringEncoding::UTF8StringEncoding);
    }

    _recordVertexLocation(_reflection);
}

void RenderPipelineState::_recordVertexLocation(MTL::RenderPipelineReflection *reflection) {
    auto count = reflection->vertexArguments()->count();
    if (count != 0) {
        for (size_t i = 0; i < count; i++) {
            const MTL::Argument *aug = static_cast<MTL::Argument *>(reflection->vertexArguments()->object(i));
            const auto name = aug->name()->cString(NS::StringEncoding::UTF8StringEncoding);
            const auto location = aug->index();
            const auto group = Shader::getShaderPropertyGroup(name);

            ShaderUniform shaderUniform;
            shaderUniform.name = name;
            shaderUniform.propertyId = Shader::getPropertyByName(name)->uniqueId;
            shaderUniform.location = location;
            shaderUniform.type = MTL::FunctionTypeVertex;
            _groupingUniform(shaderUniform, group);
        }
    }

    count = reflection->fragmentArguments()->count();
    if (count != 0) {
        for (size_t i = 0; i < count; i++) {
            const MTL::Argument *aug = static_cast<MTL::Argument *>(reflection->fragmentArguments()->object(i));
            const auto name = aug->name()->cString(NS::StringEncoding::UTF8StringEncoding);
            const auto location = aug->index();
            const auto group = Shader::getShaderPropertyGroup(name);

            ShaderUniform shaderUniform;
            shaderUniform.name = name;
            shaderUniform.propertyId = Shader::getPropertyByName(name)->uniqueId;
            shaderUniform.location = location;
            shaderUniform.type = MTL::FunctionTypeFragment;
            _groupingUniform(shaderUniform, group);
        }
    }
}

void RenderPipelineState::_groupingUniform(const ShaderUniform &uniform,
                                           const std::optional<ShaderDataGroup> &group) {
    if (group != std::nullopt) {
        switch (group.value()) {
            case ShaderDataGroup::Scene:
                sceneUniformBlock.push_back(uniform);
                break;
            case ShaderDataGroup::Camera:
                cameraUniformBlock.push_back(uniform);
                break;
            case ShaderDataGroup::Renderer:
                rendererUniformBlock.push_back(uniform);
                break;
            case ShaderDataGroup::Material:
                materialUniformBlock.push_back(uniform);
                break;
            default:
                break;
        }
    } else {
        // std::cerr << "Unknown uniform group" << std::endl;
    }
}

}// namespace vox
