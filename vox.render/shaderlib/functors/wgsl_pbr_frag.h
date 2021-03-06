//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef pbr_frag_hpp
#define pbr_frag_hpp

#include "shaderlib/wgsl_encoder.h"

namespace vox {
class WGSLPbrFrag {
public:
    WGSLPbrFrag(const std::string& input, const std::string& output, bool is_metallic_workflow);

    void operator()(std::string& source, const ShaderMacroCollection& macros);
    
private:
    const std::string _input;
    const std::string _output;
    bool _is_metallic_workflow;
};

}
#endif /* pbr_frag_hpp */
