//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef shader_macro_collection_hpp
#define shader_macro_collection_hpp

#include <unordered_map>
#include <Metal/Metal.hpp>
#include "macro_name.h"

namespace vox {
/**
 * Shader macro collection.
 */
struct ShaderMacroCollection {
    static std::unordered_map<MacroName, std::pair<int, MTL::DataType>> defaultValue;

    static std::shared_ptr<MTL::FunctionConstantValues> createDefaultFunction();

    /**
     * Union of two macro collection.
     * @param left - input macro collection
     * @param right - input macro collection
     * @param result - union output macro collection
     */
    static void unionCollection(const ShaderMacroCollection &left, const ShaderMacroCollection &right,
                                ShaderMacroCollection &result);

    size_t hash() const;

private:
    friend class ResourceCache;

    friend class ShaderData;

    std::unordered_map<MacroName, std::pair<int, MTL::DataType>> _value{};
};

}// namespace vox

#endif /* shader_macro_collection_hpp */
