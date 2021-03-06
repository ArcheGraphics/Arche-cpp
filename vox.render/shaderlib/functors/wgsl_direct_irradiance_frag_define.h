//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef wgsl_direct_irradiance_frag_define_hpp
#define wgsl_direct_irradiance_frag_define_hpp

#include "shaderlib/wgsl_encoder.h"

namespace vox {
class WGSLDirectIrradianceFragDefine {
public:
    WGSLDirectIrradianceFragDefine(const std::string& outputStructName);
    
    void operator()(WGSLEncoder& encoder,
                    const ShaderMacroCollection& macros, size_t counterIndex);
    
    void setParamName(const std::string& name);
    
    const std::string& paramName() const;
    
private:
    std::string _paramName{};
    std::string _outputStructName{};
};

}
#endif /* wgsl_direct_irradiance_frag_define_hpp */
