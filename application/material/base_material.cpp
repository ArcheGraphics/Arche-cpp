//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "base_material.h"

namespace vox {
bool BaseMaterial::isTransparent() const {
    return _isTransparent;
}

void BaseMaterial::setIsTransparent(bool newValue) {
    if (newValue == _isTransparent) {
        return;
    }
    _isTransparent = newValue;

    auto &depthState = renderState.depthState;
    auto &targetBlendState = renderState.blendState.targetBlendState;

    if (newValue) {
        targetBlendState.enabled = true;
        depthState.writeEnabled = false;
        renderQueueType = RenderQueueType::TRANSPARENT;
    } else {
        targetBlendState.enabled = false;
        depthState.writeEnabled = true;
        renderQueueType = alpha_cutoff_ > 0 ? RenderQueueType::ALPHA_TEST : RenderQueueType::OPAQUE;
    }
}

float BaseMaterial::alphaCutoff() const {
    return alpha_cutoff_;
}

void BaseMaterial::setAlphaCutoff(float newValue) {
    shader_data.set_data(BaseMaterial::alpha_cutoff_prop_, newValue);

    if (newValue > 0) {
        shader_data.enable_macro(NEED_ALPHA_CUTOFF);
        renderQueueType = _isTransparent ? RenderQueueType::TRANSPARENT : RenderQueueType::ALPHA_TEST;
    } else {
        shader_data.enable_macro(NEED_ALPHA_CUTOFF);
        renderQueueType = _isTransparent ? RenderQueueType::TRANSPARENT : RenderQueueType::OPAQUE;
    }
}

const RenderFace &BaseMaterial::renderFace() const {
    return _renderFace;
}

void BaseMaterial::setRenderFace(const RenderFace &newValue) {
    _renderFace = newValue;

    switch (newValue) {
        case RenderFace::FRONT:
            renderState.rasterState.cullMode = MTL::CullModeBack;
            break;
        case RenderFace::BACK:
            renderState.rasterState.cullMode = MTL::CullModeFront;
            break;
        case RenderFace::DOUBLE:
            renderState.rasterState.cullMode = MTL::CullModeNone;
            break;
    }
}

const BlendMode &BaseMaterial::blendMode() const {
    return _blendMode;
}

void BaseMaterial::setBlendMode(const BlendMode &newValue) {
    _blendMode = newValue;

    auto &target = renderState.blendState.targetBlendState;

    switch (newValue) {
        case BlendMode::NORMAL:
            target.sourceColorBlendFactor = MTL::BlendFactorSourceAlpha;
            target.destinationColorBlendFactor = MTL::BlendFactorOneMinusSourceAlpha;
            target.sourceAlphaBlendFactor = MTL::BlendFactorOne;
            target.destinationAlphaBlendFactor = MTL::BlendFactorOneMinusSourceAlpha;
            target.alphaBlendOperation = MTL::BlendOperationAdd;
            target.colorBlendOperation = MTL::BlendOperationAdd;
            break;
        case BlendMode::ADDITIVE:
            target.sourceColorBlendFactor = MTL::BlendFactorSourceAlpha;
            target.destinationColorBlendFactor = MTL::BlendFactorOne;
            target.sourceAlphaBlendFactor = MTL::BlendFactorOne;
            target.destinationAlphaBlendFactor = MTL::BlendFactorOneMinusSourceAlpha;
            target.alphaBlendOperation = MTL::BlendOperationAdd;
            target.colorBlendOperation = MTL::BlendOperationAdd;
            break;
    }
}

BaseMaterial::BaseMaterial(MTL::Device &device, const std::string &name)
    : Material(device),
      alpha_cutoff_prop_("u_alphaCutoff") {
    setBlendMode(BlendMode::NORMAL);
    shader_data.set_data(alpha_cutoff_prop_, alpha_cutoff_);
}

}// namespace vox
