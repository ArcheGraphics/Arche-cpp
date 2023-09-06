//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "blend_state.h"

namespace vox {
void BlendState::platformApply(MTL::RenderPipelineDescriptor &pipelineDescriptor,
                               MTL::DepthStencilDescriptor &depthStencilDescriptor,
                               MTL::RenderCommandEncoder &encoder) {
    const auto enabled = targetBlendState.enabled;
    const auto colorBlendOperation = targetBlendState.colorBlendOperation;
    const auto alphaBlendOperation = targetBlendState.alphaBlendOperation;
    const auto sourceColorBlendFactor = targetBlendState.sourceColorBlendFactor;
    const auto destinationColorBlendFactor = targetBlendState.destinationColorBlendFactor;
    const auto sourceAlphaBlendFactor = targetBlendState.sourceAlphaBlendFactor;
    const auto destinationAlphaBlendFactor = targetBlendState.destinationAlphaBlendFactor;
    const auto colorWriteMask = targetBlendState.colorWriteMask;

    if (enabled) {
        pipelineDescriptor.colorAttachments()->object(0)->setBlendingEnabled(true);
    } else {
        pipelineDescriptor.colorAttachments()->object(0)->setBlendingEnabled(false);
    }

    if (enabled) {
        // apply blend factor.
        pipelineDescriptor.colorAttachments()->object(0)->setSourceRGBBlendFactor(sourceColorBlendFactor);
        pipelineDescriptor.colorAttachments()->object(0)->setDestinationRGBBlendFactor(destinationColorBlendFactor);
        pipelineDescriptor.colorAttachments()->object(0)->setSourceAlphaBlendFactor(sourceAlphaBlendFactor);
        pipelineDescriptor.colorAttachments()->object(0)->setDestinationAlphaBlendFactor(destinationAlphaBlendFactor);

        // apply blend operation.
        pipelineDescriptor.colorAttachments()->object(0)->setRgbBlendOperation(colorBlendOperation);
        pipelineDescriptor.colorAttachments()->object(0)->setAlphaBlendOperation(alphaBlendOperation);

        // apply blend color.
        encoder.setBlendColorRed(blendColor.r, blendColor.g, blendColor.b, blendColor.a);
    }

    // apply color mask.
    pipelineDescriptor.colorAttachments()->object(0)->setWriteMask(colorWriteMask);

    // apply alpha to coverage.
    if (alphaToCoverage) {
        pipelineDescriptor.setAlphaToCoverageEnabled(true);
    } else {
        pipelineDescriptor.setAlphaToCoverageEnabled(false);
    }
}
}// namespace vox
