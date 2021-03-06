//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef blend_shape_input_hpp
#define blend_shape_input_hpp

#include "shaderlib/wgsl_encoder.h"

namespace vox {
class WGSLBlendShapeInput {
public:
    WGSLBlendShapeInput(const std::string& inputStructName);
    
    void operator()(WGSLEncoder& encoder,
                    const ShaderMacroCollection& macros, size_t counterIndex);
    
private:
    std::string _inputStructName{};
    size_t _counterIndex{};
};

}

#endif /* blend_shape_input_hpp */
