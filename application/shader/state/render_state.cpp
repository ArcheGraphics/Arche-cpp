//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "render_state.h"

namespace vox {
void RenderState::apply(MTL::RenderPipelineDescriptor &pipelineDescriptor,
                        MTL::DepthStencilDescriptor &depthStencilDescriptor,
                        MTL::RenderCommandEncoder &encoder) {
    blendState.apply(pipelineDescriptor, depthStencilDescriptor, encoder);
    depthState.apply(pipelineDescriptor, depthStencilDescriptor, encoder);
    stencilState.apply(pipelineDescriptor, depthStencilDescriptor, encoder);
    rasterState.apply(pipelineDescriptor, depthStencilDescriptor, encoder);
}
}// namespace vox
