//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "stencil_state.h"

namespace vox {
void StencilState::platformApply(MTL::RenderPipelineDescriptor &pipelineDescriptor,
                                 MTL::DepthStencilDescriptor &depthStencilDescriptor,
                                 MTL::RenderCommandEncoder &encoder) {
    if (enabled) {
        // apply stencil func.
        depthStencilDescriptor.frontFaceStencil()->setStencilCompareFunction(compareFunctionFront);
        depthStencilDescriptor.frontFaceStencil()->setReadMask(mask);

        depthStencilDescriptor.backFaceStencil()->setStencilCompareFunction(compareFunctionBack);
        depthStencilDescriptor.backFaceStencil()->setReadMask(mask);

        encoder.setStencilReferenceValue(referenceValue);
    }

    // apply stencil operation.
    depthStencilDescriptor.frontFaceStencil()->setStencilFailureOperation(failOperationFront);
    depthStencilDescriptor.frontFaceStencil()->setDepthFailureOperation(zFailOperationFront);
    depthStencilDescriptor.frontFaceStencil()->setDepthStencilPassOperation(passOperationFront);

    depthStencilDescriptor.backFaceStencil()->setStencilFailureOperation(failOperationBack);
    depthStencilDescriptor.backFaceStencil()->setDepthFailureOperation(zFailOperationBack);
    depthStencilDescriptor.backFaceStencil()->setDepthStencilPassOperation(passOperationBack);

    // apply write mask.
    depthStencilDescriptor.frontFaceStencil()->setWriteMask(writeMask);
    depthStencilDescriptor.backFaceStencil()->setWriteMask(writeMask);
}
}// namespace vox
