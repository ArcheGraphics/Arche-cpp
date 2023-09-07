//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "resource_cache.h"
#include <Metal/Metal.hpp>
#include "framework/common/helpers.h"
#include "framework/common/metal_helpers.h"

//MARK: - ResourceCache
namespace vox {
ResourceCache::ResourceCache(MTL::Device *device) : _device{device} {
}

RenderPipelineState *
ResourceCache::requestPipelineState(const MTL::RenderPipelineDescriptor &descriptor) {
    size_t hash = descriptor.hash();

    auto iter = _state.renderPipelineStates.find(hash);
    if (iter == _state.renderPipelineStates.end()) {
        auto pipelineState = std::make_unique<RenderPipelineState>(_device, descriptor);
        _state.renderPipelineStates[hash] = std::move(pipelineState);
        return _state.renderPipelineStates[hash].get();
    } else {
        return iter->second.get();
    }
}

ComputePipelineState *
ResourceCache::requestPipelineState(const MTL::ComputePipelineDescriptor &descriptor) {
    size_t hash = descriptor.hash();

    auto iter = _state.computePipelineStates.find(hash);
    if (iter == _state.computePipelineStates.end()) {
        auto pipelineState = std::make_unique<ComputePipelineState>(_device, descriptor);
        _state.computePipelineStates[hash] = std::move(pipelineState);
        return _state.computePipelineStates[hash].get();
    } else {
        return iter->second.get();
    }
}

MTL::DepthStencilState *
ResourceCache::requestDepthStencilState(const MTL::DepthStencilDescriptor &descriptor) {
    size_t hash = descriptor.hash();

    auto iter = _state.depthStencilStates.find(hash);
    if (iter == _state.depthStencilStates.end()) {
        auto depthStencilState = CLONE_METAL_CUSTOM_DELETER(MTL::DepthStencilState,
                                                            _device->newDepthStencilState(&descriptor));
        _state.depthStencilStates[hash] = std::move(depthStencilState);
        return _state.depthStencilStates[hash].get();
    } else {
        return iter->second.get();
    }
}

MTL::Function *ResourceCache::requestFunction(MTL::Library &library,
                                              const std::string &source,
                                              const ShaderMacroCollection &macroInfo) {
    std::size_t hash{0U};
    hash_combine(hash, std::hash<std::string>{}(source));
    hash_combine(hash, macroInfo.hash());

    auto iter = _state.functions.find(hash);
    if (iter == _state.functions.end()) {
        NS::Error *error;
        auto functionConstants = _makeFunctionConstants(macroInfo);
        auto shader =
            CLONE_METAL_CUSTOM_DELETER(MTL::Function,
                                       library.newFunction(NS::String::string(source.c_str(),
                                                                              NS::StringEncoding::UTF8StringEncoding),
                                                           functionConstants.get(), &error));
        _state.functions[hash] = std::move(shader);
        return _state.functions[hash].get();
    } else {
        return iter->second.get();
    }
}

std::shared_ptr<MTL::FunctionConstantValues>
ResourceCache::_makeFunctionConstants(const ShaderMacroCollection &macroInfo) {
    auto functionConstants = ShaderMacroCollection::createDefaultFunction();
    std::for_each(macroInfo._value.begin(), macroInfo._value.end(),
                  [&](const std::pair<MacroName, std::pair<int, MTL::DataType>> &info) {
                      if (info.second.second == MTL::DataTypeBool) {
                          bool property;
                          if (info.second.first == 1) {
                              property = true;
                          } else {
                              property = false;
                          }
                          functionConstants->setConstantValue(&property, MTL::DataTypeBool, info.first);
                      } else {
                          auto &property = info.second.first;
                          functionConstants->setConstantValue(&property, info.second.second, info.first);
                      }
                  });
    return functionConstants;
}

}// namespace vox
