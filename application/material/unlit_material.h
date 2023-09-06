//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "base_material.h"
#include "color.h"
#include "vector4.h"
#include "texture/sampled_texture2d.h"

namespace vox {
/**
 * Unlit Material.
 */
class UnlitMaterial : public BaseMaterial {
public:
    /**
     * Base color.
     */
    Color baseColor() const;

    void setBaseColor(const Color &newValue);

    /**
     * Base texture.
     */
    SampledTexture2DPtr baseTexture() const;

    void setBaseTexture(const SampledTexture2DPtr &newValue);

    /**
     * Tiling and offset of main textures.
     */
    Vector4F tilingOffset() const;

    void setTilingOffset(const Vector4F &newValue);

    /**
     * Create a unlit material instance.
     */
    explicit UnlitMaterial();

private:
    ShaderProperty _baseColorProp;
    ShaderProperty _baseTextureProp;
    ShaderProperty _tilingOffsetProp;
};

}// namespace vox