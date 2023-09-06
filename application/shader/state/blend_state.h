//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "renderTarget_blend_state.h"
#include "color.h"
#include <Metal/Metal.hpp>

namespace vox {
/**
 * Blend state.
 */
struct BlendState {
    /** The blend state of the render target. */
    RenderTargetBlendState targetBlendState = RenderTargetBlendState();
    /** Constant blend color. */
    Color blendColor = Color(0, 0, 0, 0);
    /** Whether to use (Alpha-to-Coverage) technology. */
    bool alphaToCoverage = false;

    /**
     * Apply the current blend state by comparing with the last blend state.
     */
    void apply(MTL::RenderPipelineDescriptor &pipelineDescriptor,
               MTL::DepthStencilDescriptor &depthStencilDescriptor,
               MTL::RenderCommandEncoder &encoder) {
        platformApply(pipelineDescriptor, depthStencilDescriptor, encoder);
    }

    void platformApply(MTL::RenderPipelineDescriptor &pipelineDescriptor,
                       MTL::DepthStencilDescriptor &depthStencilDescriptor,
                       MTL::RenderCommandEncoder &encoder);
};

}// namespace vox