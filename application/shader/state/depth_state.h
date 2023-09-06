//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <Metal/Metal.hpp>

namespace vox {
/**
 * Depth state.
 */
struct DepthState {
    /** Whether to enable the depth test. */
    bool enabled = true;
    /** Whether the depth value can be written.*/
    bool writeEnabled = true;
    /** Depth comparison function. */
    MTL::CompareFunction compareFunction = MTL::CompareFunctionLess;

    /**
     * Apply the current depth state by comparing with the last depth state.
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