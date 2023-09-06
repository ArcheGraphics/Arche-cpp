//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "depth_state.h"

namespace vox {
void DepthState::platformApply(MTL::RenderPipelineDescriptor &pipelineDescriptor,
                               MTL::DepthStencilDescriptor &depthStencilDescriptor,
                               MTL::RenderCommandEncoder &encoder) {
    if (enabled) {
        // apply compare func.
        depthStencilDescriptor.setDepthCompareFunction(compareFunction);

        // apply write enabled.
        depthStencilDescriptor.setDepthWriteEnabled(writeEnabled);
    }
}
}// namespace vox
