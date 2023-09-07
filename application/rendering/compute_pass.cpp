//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "compute_pass.h"
#include "framework/common/metal_helpers.h"

namespace vox {
ComputePass::ComputePass(MTL::Library &library, Scene *scene, const std::string &kernel) : _library(library),
                                                                                           _scene(scene),
                                                                                           _kernel(kernel),
                                                                                           _resourceCache(library.device()) {
    _pipelineDescriptor = CLONE_METAL_CUSTOM_DELETER(MTL::ComputePipelineDescriptor,
                                                     MTL::ComputePipelineDescriptor::alloc()->init());
}

ResourceCache &ComputePass::resourceCache() {
    return _resourceCache;
}

MTL::Library &ComputePass::library() {
    return _library;
}

uint32_t ComputePass::threadsPerGridX() const {
    return _threadsPerGridX;
}

uint32_t ComputePass::threadsPerGridY() const {
    return _threadsPerGridY;
}

uint32_t ComputePass::threadsPerGridZ() const {
    return _threadsPerGridZ;
}

void ComputePass::setThreadsPerGrid(uint32_t threadsPerGridX,
                                    uint32_t threadsPerGridY,
                                    uint32_t threadsPerGridZ) {
    _threadsPerGridX = threadsPerGridX;
    _threadsPerGridY = threadsPerGridY;
    _threadsPerGridZ = threadsPerGridZ;
}

void ComputePass::attachShaderData(ShaderData *data) {
    auto iter = std::find(_data.begin(), _data.end(), data);
    if (iter == _data.end()) {
        _data.push_back(data);
    } else {
        LOG(ERROR) << "ShaderData already attached." << std::endl;
        ;
    }
}

void ComputePass::detachShaderData(ShaderData *data) {
    auto iter = std::find(_data.begin(), _data.end(), data);
    if (iter != _data.end()) {
        _data.erase(iter);
    }
}

void ComputePass::compute(MTL::ComputeCommandEncoder &commandEncoder) {
    auto compileMacros = ShaderMacroCollection();
    for (auto &shaderData : _data) {
        shaderData->mergeMacro(compileMacros, compileMacros);
    }

    auto function = _resourceCache.requestFunction(_library, _kernel, compileMacros);
    _pipelineDescriptor->setComputeFunction(function);
    auto pipelineState = _resourceCache.requestPipelineState(*_pipelineDescriptor);
    for (auto &shaderData : _data) {
        uploadUniforms(commandEncoder, pipelineState->uniformBlock, *shaderData);
    }
    commandEncoder.setComputePipelineState(&pipelineState->handle());

    auto nWidth = std::min(_threadsPerGridX, static_cast<uint32_t>(pipelineState->handle().threadExecutionWidth()));
    auto nHeight = std::min(_threadsPerGridY, static_cast<uint32_t>(pipelineState->handle().maxTotalThreadsPerThreadgroup() / nWidth));
    commandEncoder.dispatchThreads(MTL::Size(_threadsPerGridX, _threadsPerGridY, _threadsPerGridZ),
                                   MTL::Size(nWidth, nHeight, 1));
}

void ComputePass::uploadUniforms(MTL::ComputeCommandEncoder &commandEncoder,
                                 const std::vector<ShaderUniform> &uniformBlock,
                                 const ShaderData &shaderData) {
    for (size_t i = 0; i < uniformBlock.size(); i++) {
        const auto &uniform = uniformBlock[i];
        auto data = shaderData.getData(uniform.propertyId);
        if (data) {
            process(uniform, data.value(), commandEncoder);
        }
    }
}

void ComputePass::process(const ShaderUniform &uniform, const std::any &a,
                          MTL::ComputeCommandEncoder &encoder) {
    const auto &any_uploader = _scene->computeUploader();

    if (const auto it = any_uploader.find(std::type_index(a.type())); it != any_uploader.cend()) {
        it->second(a, uniform.location, encoder);
    } else {
        LOG(INFO) << "Unregistered type " << std::quoted(a.type().name());
    }
}

}// namespace vox
