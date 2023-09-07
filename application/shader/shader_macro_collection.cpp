//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "shader_macro_collection.h"
#include "framework/common/helpers.h"
#include "framework/common/metal_helpers.h"

namespace vox {

void ShaderMacroCollection::unionCollection(const ShaderMacroCollection &left, const ShaderMacroCollection &right,
                                            ShaderMacroCollection &result) {
    result._value.insert(left._value.begin(), left._value.end());
    result._value.insert(right._value.begin(), right._value.end());
}

size_t ShaderMacroCollection::hash() const {
    std::size_t hash{0U};
    for (int i = 0; i < MacroName::TOTAL_COUNT; i++) {
        auto iter = _value.find(MacroName(i));
        if (iter != _value.end()) {
            hash_combine(hash, iter->first);
        }
    }
    return hash;
}

}// namespace vox
