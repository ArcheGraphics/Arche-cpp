//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "texture/texture_manager.h"

namespace vox {
TextureManager *TextureManager::get_singleton_ptr() { return ms_singleton; }

TextureManager &TextureManager::get_singleton() {
    assert(ms_singleton);
    return (*ms_singleton);
}

TextureManager::TextureManager(MTL::Device &device)
    : device_(device) {
}

std::shared_ptr<Texture> TextureManager::load_texture(const std::string &file) {
    auto iter = image_pool_.find(file);
    if (iter != image_pool_.end()) {
        return iter->second;
    } else {
        auto image = vox::Texture::load(file);
        image->create_image(device_);
        upload_texture(image.get());
        image_pool_.insert(std::make_pair(file, image));
        return image;
    }
}

std::shared_ptr<Texture> TextureManager::load_texture_array(const std::string &file) {
    auto iter = image_pool_.find(file);
    if (iter != image_pool_.end()) {
        return iter->second;
    } else {
        auto image = vox::Texture::load(file);
        image->create_image(device_, MTL::TextureType2DArray);
        upload_texture(image.get());
        image_pool_.insert(std::make_pair(file, image));
        return image;
    }
}

std::shared_ptr<Texture> TextureManager::load_texture_cubemap(const std::string &file) {
    auto iter = image_pool_.find(file);
    if (iter != image_pool_.end()) {
        return iter->second;
    } else {
        auto image = vox::Texture::load(file);
        image->create_image(device_, MTL::TextureTypeCube);
        upload_texture(image.get());
        image_pool_.insert(std::make_pair(file, image));
        return image;
    }
}

void TextureManager::upload_texture(vox::Texture *image) {
    // todo
}

void TextureManager::collect_garbage() {
    for (auto &image : image_pool_) {
        if (image.second.use_count() == 1) {
            image.second.reset();
        }
    }
}

}// namespace vox
