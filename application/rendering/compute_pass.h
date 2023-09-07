//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "resource_cache.h"
#include "ecs/scene.h"

namespace vox {
class ComputePass {
public:
    ComputePass(MTL::Library &library, Scene *scene,
                const std::string &kernel);

    ComputePass(const ComputePass &) = delete;

    ComputePass(ComputePass &&) = delete;

    virtual ~ComputePass() = default;

    ComputePass &operator=(const ComputePass &) = delete;

    ComputePass &operator=(ComputePass &&) = delete;

    MTL::Library &library();

    ResourceCache &resourceCache();

public:
    uint32_t threadsPerGridX() const;
    uint32_t threadsPerGridY() const;
    uint32_t threadsPerGridZ() const;

    void setThreadsPerGrid(uint32_t threadsPerGridX,
                           uint32_t threadsPerGridY = 1,
                           uint32_t threadsPerGridZ = 1);

    void attachShaderData(ShaderData *data);

    void detachShaderData(ShaderData *data);

    /**
     * @brief Compute virtual function
     * @param commandEncoder CommandEncoder to use to record compute commands
     */
    virtual void compute(MTL::ComputeCommandEncoder &commandEncoder);

protected:
    MTL::Library &_library;
    Scene *_scene{nullptr};
    std::string _kernel;

    uint32_t _threadsPerGridX = 1;
    uint32_t _threadsPerGridY = 1;
    uint32_t _threadsPerGridZ = 1;

    std::vector<ShaderData *> _data{};

    std::shared_ptr<MTL::ComputePipelineDescriptor> _pipelineDescriptor;

    /**
     * Upload constant shader data in shader uniform block.
     * @param uniformBlock - shader Uniform block
     * @param shaderData - shader data
     */
    void uploadUniforms(MTL::ComputeCommandEncoder &commandEncoder,
                        const std::vector<ShaderUniform> &uniformBlock,
                        const ShaderData &shaderData);

    void process(const ShaderUniform &uniform, const std::any &a, MTL::ComputeCommandEncoder &encoder);

private:
    ResourceCache _resourceCache;
};

}// namespace vox