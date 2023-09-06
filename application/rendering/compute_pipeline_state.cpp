//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "compute_pipeline_state.h"
#include "shader/shader_data_group.h"
#include "shader/shader.h"
#include "metal_helpers.h"
#include <glog/logging.h>

namespace vox {
ComputePipelineState::ComputePipelineState(MTL::Device *device,
                                           const MTL::ComputePipelineDescriptor &descriptor) : _device(device) {
    MTL::ComputePipelineReflection *_reflection{nullptr};
    NS::Error *error{nullptr};

    _handle = CLONE_METAL_CUSTOM_DELETER(MTL::ComputePipelineState,
                                         _device->newComputePipelineState(&descriptor, MTL::PipelineOptionArgumentInfo,
                                                                          &_reflection, &error));

    if (error != nullptr) {
        LOG(ERROR) << "Error: failed to create Metal pipeline state: "
                   << error->description()->cString(NS::StringEncoding::UTF8StringEncoding);
    }

    _recordVertexLocation(_reflection);
}

void ComputePipelineState::_recordVertexLocation(MTL::ComputePipelineReflection *reflection) {
    auto count = reflection->arguments()->count();
    if (count != 0) {
        for (size_t i = 0; i < count; i++) {
            const MTL::Argument *aug = static_cast<MTL::Argument *>(reflection->arguments()->object(i));
            const auto name = aug->name()->cString(NS::StringEncoding::UTF8StringEncoding);
            const auto location = aug->index();

            ShaderUniform shaderUniform;
            shaderUniform.name = name;
            shaderUniform.propertyId = Shader::getPropertyByName(name)->uniqueId;
            shaderUniform.location = location;
            shaderUniform.type = MTL::FunctionTypeKernel;
            uniformBlock.push_back(shaderUniform);
        }
    }
}

}// namespace vox
