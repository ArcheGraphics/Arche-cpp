//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "components/renderer.h"

#include "ecs/components_manager.h"
#include "ecs/entity.h"
#include "ecs/scene.h"
#include "material/material.h"

namespace vox {
size_t Renderer::get_material_count() { return materials_.size(); }

BoundingBox3F Renderer::get_bounds() {
    auto &change_flag = transform_change_flag_;
    if (change_flag->flag_) {
        update_bounds(bounds_);
        change_flag->flag_ = false;
    }
    return bounds_;
}

Renderer::Renderer(Entity *entity)
    : Component(entity),
      shader_data_(entity->get_scene()->get_device()),
      transform_change_flag_(entity->transform->register_world_change_flag()),
      renderer_property_("rendererData") {}

void Renderer::on_enable() { ComponentsManager::get_singleton().add_renderer(this); }

void Renderer::on_disable() { ComponentsManager::get_singleton().remove_renderer(this); }

Renderer::MaterialPtr Renderer::get_instance_material(size_t index) {
    const auto &materials = materials_;
    if (materials.size() > index) {
        const auto &material = materials[index];
        if (material != nullptr) {
            if (materials_instanced_[index]) {
                return material;
            } else {
                return create_instance_material(material, index);
            }
        }
    }
    return nullptr;
}

Renderer::MaterialPtr Renderer::get_material(size_t index) { return materials_[index]; }

void Renderer::set_material(const MaterialPtr &material) {
    size_t index = 0;

    if (index >= materials_.size()) {
        materials_.reserve(index + 1);
        for (size_t i = materials_.size(); i <= index; i++) {
            materials_.push_back(nullptr);
        }
    }

    const auto &internal_material = materials_[index];
    if (internal_material != material) {
        materials_[index] = material;
        if (index < materials_instanced_.size()) {
            materials_instanced_[index] = false;
        }
    }
}

void Renderer::set_material(size_t index, const MaterialPtr &material) {
    if (index >= materials_.size()) {
        materials_.reserve(index + 1);
        for (size_t i = materials_.size(); i <= index; i++) {
            materials_.push_back(nullptr);
        }
    }

    const auto &internal_material = materials_[index];
    if (internal_material != material) {
        materials_[index] = material;
        if (index < materials_instanced_.size()) {
            materials_instanced_[index] = false;
        }
    }
}

std::vector<Renderer::MaterialPtr> Renderer::get_instance_materials() {
    for (size_t i = 0; i < materials_.size(); i++) {
        if (!materials_instanced_[i]) {
            create_instance_material(materials_[i], i);
        }
    }
    return materials_;
}

std::vector<Renderer::MaterialPtr> Renderer::get_materials() { return materials_; }

void Renderer::set_materials(const std::vector<MaterialPtr> &materials) {
    size_t count = materials.size();
    if (materials_.size() != count) {
        materials_.reserve(count);
        for (size_t i = materials_.size(); i < count; i++) {
            materials_.push_back(nullptr);
        }
    }
    if (!materials_instanced_.empty()) {
        materials_instanced_.clear();
    }

    for (size_t i = 0; i < count; i++) {
        const auto &internal_material = materials_[i];
        const auto &material = materials[i];
        if (internal_material != material) {
            materials_[i] = material;
        }
    }
}

void Renderer::push_primitive(const RenderElement &element,
                              std::vector<RenderElement> &opaque_queue,
                              std::vector<RenderElement> &alpha_test_queue,
                              std::vector<RenderElement> &transparent_queue) {
    const auto kRenderQueueType = element.material->render_queue_;

    if (kRenderQueueType > (RenderQueueType::TRANSPARENT + RenderQueueType::ALPHA_TEST) >> 1) {
        transparent_queue.push_back(element);
    } else if (kRenderQueueType > (RenderQueueType::ALPHA_TEST + RenderQueueType::OPAQUE) >> 1) {
        alpha_test_queue.push_back(element);
    } else {
        opaque_queue.push_back(element);
    }
}

void Renderer::set_distance_for_sort(float dist) { distance_for_sort_ = dist; }

float Renderer::get_distance_for_sort() const { return distance_for_sort_; }

void Renderer::update_shader_data() {
    auto world_matrix = get_entity()->transform->get_world_matrix();
    normal_matrix_ = world_matrix.inverse();
    normal_matrix_ = normal_matrix_.transposed();

    renderer_data_.local_mat = get_entity()->transform->get_local_matrix();
    renderer_data_.model_mat = world_matrix;
    renderer_data_.normal_mat = normal_matrix_;
    shader_data_.set_data(Renderer::renderer_property_, renderer_data_);
}

Renderer::MaterialPtr Renderer::create_instance_material(const Renderer::MaterialPtr &material, size_t index) { return nullptr; }

}// namespace vox
