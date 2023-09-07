//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "shader_macro_collection.h"
#include "framework/common/metal_helpers.h"
#include <unordered_map>
#include <string>

namespace vox {
/**
 * Shader data collection,Correspondence includes shader properties data and macros data.
 */
class ShaderData {
public:
    explicit ShaderData(MTL::Device &device);

    template<typename T>
    void set_data(const std::string &property_name, T &value) {
        auto iter = shader_buffers_.find(property_name);
        if (iter == shader_buffers_.end()) {
            auto buffer = CLONE_METAL_CUSTOM_DELETER(MTL::Buffer, device_.newBuffer(&value, sizeof(T),
                                                                                    MTL::ResourceStorageModeShared));
            shader_buffers_.insert(std::make_pair(property_name, buffer));
        }
        iter = shader_buffers_.find(property_name);
        memcpy(iter->second->contents(), &value, sizeof(T));
    }

    template<typename T>
    void set_data(const std::string &property_name, std::vector<T> &value) {
        auto iter = shader_buffers_.find(property_name);
        if (iter == shader_buffers_.end()) {
            auto buffer = CLONE_METAL_CUSTOM_DELETER(MTL::Buffer, device_.newBuffer(&value, sizeof(T) * value.size(),
                                                                                    MTL::ResourceStorageModeShared));
            shader_buffers_.insert(std::make_pair(property_name, buffer));
        }
        iter = shader_buffers_.find(property_name);
        memcpy(iter->second->contents(), &value, sizeof(T) * value.size());
    }

    template<typename T, size_t N>
    void set_data(const std::string &property_name, std::array<T, N> &value) {
        auto iter = shader_buffers_.find(property_name);
        if (iter == shader_buffers_.end()) {
            auto buffer = CLONE_METAL_CUSTOM_DELETER(MTL::Buffer, device_.newBuffer(&value, sizeof(T) * N,
                                                                                    MTL::ResourceStorageModeShared));
            shader_buffers_.insert(std::make_pair(property_name, buffer));
        }
        iter = shader_buffers_.find(property_name);
        memcpy(iter->second->contents(), &value, sizeof(T) * N);
    }

public:
    /**
     * Enable macro.
     * @param macroName - Shader macro
     */
    void enable_macro(MacroName macroName);

    /**
     * Enable macro.
     * @remarks Name and value will combine one macro, it's equal the macro of "name value".
     * @param macroName - Macro name
     * @param value - Macro value
     */
    void enable_macro(MacroName macroName, std::pair<int, MTL::DataType> value);

    /**
     * Disable macro
     * @param macroName - Macro name
     */
    void disable_macro(MacroName macroName);

    void merge_macro(const ShaderMacroCollection &macros,
                     ShaderMacroCollection &result) const;

private:
    MTL::Device &device_;

    std::unordered_map<std::string, std::shared_ptr<MTL::Buffer>> shader_buffers_{};

    ShaderMacroCollection _macroCollection;
};

}// namespace vox