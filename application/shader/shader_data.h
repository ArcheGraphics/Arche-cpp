//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef shader_data_hpp
#define shader_data_hpp

#include "shader_data_group.h"
#include "shader_macro_collection.h"
#include "shader_property.h"
#include "texture/sampled_texture.h"
#include <any>
#include <unordered_map>

namespace vox {
/**
 * Shader data collection,Correspondence includes shader properties data and macros data.
 */
class ShaderData {
public:
    std::optional<std::any> getData(const std::string &property_name) const;

    std::optional<std::any> getData(const ShaderProperty &property) const;

    std::optional<std::any> getData(uint32_t uniqueID) const;

    void setBufferFunctor(const std::string &property_name,
                          std::function<std::shared_ptr<MTL::Buffer>()> functor);

    void setBufferFunctor(ShaderProperty property,
                          std::function<std::shared_ptr<MTL::Buffer>()> functor);

    void setData(const std::string &property, std::any value);

    void setData(ShaderProperty property, std::any value);

    void setSampledTexure(const std::string &property, const SampledTexturePtr &value);

    void setSampledTexure(ShaderProperty property, const SampledTexturePtr &value);

public:
    /**
     * Enable macro.
     * @param macroName - Shader macro
     */
    void enableMacro(MacroName macroName);

    /**
     * Enable macro.
     * @remarks Name and value will combine one macro, it's equal the macro of "name value".
     * @param macroName - Macro name
     * @param value - Macro value
     */
    void enableMacro(MacroName macroName, std::pair<int, MTL::DataType> value);

    /**
     * Disable macro
     * @param macroName - Macro name
     */
    void disableMacro(MacroName macroName);

    void mergeMacro(const ShaderMacroCollection &macros,
                    ShaderMacroCollection &result) const;

private:
    std::unordered_map<int, std::any> _properties{};
    std::unordered_map<uint32_t, std::function<std::shared_ptr<MTL::Buffer>()>> _shaderBufferFunctors{};
    ShaderMacroCollection _macroCollection;
};

}// namespace vox
#endif /* shader_data_hpp */
