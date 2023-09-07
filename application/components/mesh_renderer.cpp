//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "mesh_renderer.h"
#include "mesh/mesh.h"
#include "ecs/entity.h"
#include "shader/shader_common.h"

namespace vox {
MeshRenderer::MeshRenderer(Entity *entity) : Renderer(entity) {
}

void MeshRenderer::setMesh(const MeshPtr &newValue) {
    auto &lastMesh = _mesh;
    if (lastMesh != newValue) {
        if (lastMesh != nullptr) {
            _meshUpdateFlag.reset();
        }
        if (newValue != nullptr) {
            _meshUpdateFlag = newValue->registerUpdateFlag();
        }
        _mesh = newValue;
    }
}

MeshPtr MeshRenderer::mesh() {
    return _mesh;
}

void MeshRenderer::render(std::vector<RenderElement> &opaqueQueue,
                          std::vector<RenderElement> &alphaTestQueue,
                          std::vector<RenderElement> &transparentQueue) {
    if (_mesh != nullptr) {
        if (_meshUpdateFlag->flag_) {
            const auto &vertexDescriptor = _mesh->vertexDescriptor();

            shader_data_.disable_macro(HAS_UV);
            shader_data_.disable_macro(HAS_NORMAL);
            shader_data_.disable_macro(HAS_TANGENT);
            shader_data_.disable_macro(HAS_VERTEXCOLOR);

            if (vertexDescriptor->attributes()->object(Attributes::UV_0)->format() != MTL::VertexFormatInvalid) {
                shader_data_.enable_macro(HAS_UV);
            }
            if (vertexDescriptor->attributes()->object(Attributes::Normal)->format() != MTL::VertexFormatInvalid) {
                shader_data_.enable_macro(HAS_NORMAL);
            }
            if (vertexDescriptor->attributes()->object(Attributes::Tangent)->format() != MTL::VertexFormatInvalid) {
                shader_data_.enable_macro(HAS_TANGENT);
            }
            if (vertexDescriptor->attributes()->object(Attributes::Color_0)->format() != MTL::VertexFormatInvalid) {
                shader_data_.enable_macro(HAS_VERTEXCOLOR);
            }
            _meshUpdateFlag->flag_ = false;
        }

        auto &subMeshes = _mesh->subMeshes();
        for (size_t i = 0; i < subMeshes.size(); i++) {
            MaterialPtr material;
            if (i < materials_.size()) {
                material = materials_[i];
            } else {
                material = nullptr;
            }
            if (material != nullptr) {
                RenderElement element(this, _mesh, &subMeshes[i], material);
                push_primitive(element, opaqueQueue, alphaTestQueue, transparentQueue);
            }
        }
    }
}

void MeshRenderer::update_bounds(BoundingBox3F &worldBounds) {
    if (_mesh != nullptr) {
        const auto localBounds = _mesh->bounds;
        const auto worldMatrix = entity_->transform->get_world_matrix();
        worldBounds = localBounds.transform(worldMatrix);
    } else {
        worldBounds.lower_corner = Point3F(0, 0, 0);
        worldBounds.upper_corner = Point3F(0, 0, 0);
    }
}

}// namespace vox
