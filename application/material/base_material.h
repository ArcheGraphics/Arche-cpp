//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "material.h"
#include "enums/render_face.h"
#include "enums/blend_mode.h"

namespace vox {
class BaseMaterial : public Material {
public:
    /**
     * Is this material transparent.
     * @remarks
     * If material is transparent, transparent blend mode will be affected by `blendMode`, default is `BlendMode.Normal`.
     */
    [[nodiscard]] bool isTransparent() const;

    void setIsTransparent(bool newValue);

    /**
     * Alpha cutoff value.
     * @remarks
     * Fragments with alpha channel lower than cutoff value will be discarded.
     * `0` means no fragment will be discarded.
     */
    [[nodiscard]] float alphaCutoff() const;

    void setAlphaCutoff(float newValue);

    /**
     * Set which face for render.
     */
    [[nodiscard]] const RenderFace &renderFace() const;

    void setRenderFace(const RenderFace &newValue);

    /**
     * Alpha blend mode.
     * @remarks
     * Only take effect when `isTransparent` is `true`.
     */
    [[nodiscard]] const BlendMode &blendMode() const;

    void setBlendMode(const BlendMode &newValue);

    /**
     * Create a BaseMaterial instance.
     */
    BaseMaterial(MTL::Device &device, const std::string &name = "");

private:
    float alpha_cutoff_ = 0.f;
    const std::string alpha_cutoff_prop_;

    RenderFace _renderFace = RenderFace::BACK;
    BlendMode _blendMode = BlendMode::NORMAL;
    bool _isTransparent = false;
};

}// namespace vox