//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "wgsl_skinning_vert.h"
#include <fmt/core.h>

namespace vox {
WGSLSkinningVert::WGSLSkinningVert(const std::string& input, const std::string& output):
_input(input),
_output(output) {
}

void WGSLSkinningVert::operator()(std::string& source, const ShaderMacroCollection& macros) {
    if (macros.contains(HAS_SKIN)) {
        if (macros.contains(HAS_JOINT_TEXTURE)) {
            source += fmt::format("var skinMatrix = {}.Weights_0.x * getJointMatrix(u_jointSampler, {}.Joints_0.x ) +\n", _input, _input);
            source += fmt::format("{}.Weights_0.y * getJointMatrix(u_jointSampler, {}.Joints_0.y ) +\n", _input, _input);
            source += fmt::format("{}.Weights_0.z * getJointMatrix(u_jointSampler, {}.Joints_0.z ) +\n", _input, _input);
            source += fmt::format("{}.Weights_0.w * getJointMatrix(u_jointSampler, {}.Joints_0.w );\n", _input, _input);
        } else {
            source += fmt::format("var skinMatrix = {}.Weights_0.x * u_jointMatrix[ i32( {}.Joints_0.x ) ] +\n", _input, _input);
            source += fmt::format("{}.Weights_0.y * u_jointMatrix[ i32( {}.Joints_0.y ) ] +\n", _input, _input);
            source += fmt::format("{}.Weights_0.z * u_jointMatrix[ i32( {}.Joints_0.z ) ] +\n", _input, _input);
            source += fmt::format("{}.Weights_0.w * u_jointMatrix[ i32( {}.Joints_0.w ) ];\n", _input, _input);
        }
        source += "position = skinMatrix * position;\n";
        if (macros.contains(HAS_NORMAL) && !macros.contains(OMIT_NORMAL)) {
            source += "normal = vec4<f32>( skinMatrix * vec4<f32>( normal, 0.0 ) ).xyz;\n";
            if (macros.contains(HAS_TANGENT) && macros.contains(HAS_NORMAL_TEXTURE)) {
                source += "tangent.xyz = vec4<f32>( skinMatrix * vec4<f32>( tangent.xyz, 0.0 ) ).xyz;\n";
            }
        }

    }
}

}
