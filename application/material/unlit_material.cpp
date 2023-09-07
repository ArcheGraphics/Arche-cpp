//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "material/unlit_material.h"

namespace vox {
const Color &UnlitMaterial::get_base_color() const { return base_color_; }

void UnlitMaterial::set_base_color(const Color &new_value) {
    base_color_ = new_value;
    shader_data.set_data(UnlitMaterial::base_color_prop_, base_color_);
}

std::shared_ptr<Texture> UnlitMaterial::get_base_texture() const { return base_texture_; }

void UnlitMaterial::set_base_texture(const std::shared_ptr<Texture> &new_value) {
    if (new_value) {
        //        BaseMaterial::last_sampler_create_info_.maxLod = static_cast<float>(new_value->get_mipmaps().size());
        //        set_base_texture(new_value, BaseMaterial::last_sampler_create_info_);
    }
}

void UnlitMaterial::set_base_texture(const std::shared_ptr<Texture> &new_value, const MTL::SamplerDescriptor &info) {
    base_texture_ = new_value;
    if (new_value) {
        //        shader_data.set_sampled_texture(base_texture_prop_, new_value->get_vk_image_view(),
        //                                         &device_.get_resource_cache().request_sampler(info));
        shader_data.enable_macro(HAS_BASE_TEXTURE);
    } else {
        shader_data.disable_macro(HAS_BASE_TEXTURE);
    }
}

UnlitMaterial::UnlitMaterial(MTL::Device &device, const std::string &name)
    : BaseMaterial(device, name), base_color_prop_("baseColor"), base_texture_prop_("baseTexture") {
    vertex_source = "";
    fragment_source = "";

    shader_data.enable_macro(OMIT_NORMAL);

    shader_data.set_data(base_color_prop_, base_color_);
}

}// namespace vox
