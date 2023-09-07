//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "math/color.h"
#include "material/base_material.h"
#include "texture/texture.h"

namespace vox {
/**
 * Unlit Material.
 */
class UnlitMaterial : public BaseMaterial {
public:
    /**
     * Base color.
     */
    [[nodiscard]] const Color &get_base_color() const;

    void set_base_color(const Color &new_value);

    /**
     * Base texture.
     */
    [[nodiscard]] std::shared_ptr<Texture> get_base_texture() const;

    void set_base_texture(const std::shared_ptr<Texture> &new_value);

    void set_base_texture(const std::shared_ptr<Texture> &new_value, const MTL::SamplerDescriptor &info);

    /**
     * Create a unlit material instance.
     */
    UnlitMaterial(MTL::Device &device, const std::string &name = "");

private:
    Color base_color_ = Color(1, 1, 1, 1);
    const std::string base_color_prop_;

    std::shared_ptr<Texture> base_texture_{nullptr};
    const std::string base_texture_prop_;
};

}// namespace vox
