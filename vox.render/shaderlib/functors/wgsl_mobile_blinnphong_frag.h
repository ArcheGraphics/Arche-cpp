//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef wgsl_mobile_blinnphong_frag_hpp
#define wgsl_mobile_blinnphong_frag_hpp

#include "shaderlib/wgsl_encoder.h"

namespace vox {
class WGSLMobileBlinnphongFrag {
public:
    WGSLMobileBlinnphongFrag(const std::string& input, const std::string& output);

    void operator()(std::string& source, const ShaderMacroCollection& macros);
    
private:
    const std::string _input;
    const std::string _output;
};

}
#endif /* wgsl_mobile_blinnphong_frag_hpp */
