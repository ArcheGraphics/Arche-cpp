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
            _meshUpdateFlag->destroy();
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

void MeshRenderer::_render(std::vector<RenderElement> &opaqueQueue,
                           std::vector<RenderElement> &alphaTestQueue,
                           std::vector<RenderElement> &transparentQueue) {
    if (_mesh != nullptr) {
        if (_meshUpdateFlag->flag) {
            const auto &vertexDescriptor = _mesh->vertexDescriptor();

            shaderData.disableMacro(HAS_UV);
            shaderData.disableMacro(HAS_NORMAL);
            shaderData.disableMacro(HAS_TANGENT);
            shaderData.disableMacro(HAS_VERTEXCOLOR);

            if (vertexDescriptor->attributes()->object(Attributes::UV_0)->format() != MTL::VertexFormatInvalid) {
                shaderData.enableMacro(HAS_UV);
            }
            if (vertexDescriptor->attributes()->object(Attributes::Normal)->format() != MTL::VertexFormatInvalid) {
                shaderData.enableMacro(HAS_NORMAL);
            }
            if (vertexDescriptor->attributes()->object(Attributes::Tangent)->format() != MTL::VertexFormatInvalid) {
                shaderData.enableMacro(HAS_TANGENT);
            }
            if (vertexDescriptor->attributes()->object(Attributes::Color_0)->format() != MTL::VertexFormatInvalid) {
                shaderData.enableMacro(HAS_VERTEXCOLOR);
            }
            _meshUpdateFlag->flag = false;
        }

        auto &subMeshes = _mesh->subMeshes();
        for (size_t i = 0; i < subMeshes.size(); i++) {
            MaterialPtr material;
            if (i < _materials.size()) {
                material = _materials[i];
            } else {
                material = nullptr;
            }
            if (material != nullptr) {
                RenderElement element(this, _mesh, &subMeshes[i], material);
                pushPrimitive(element, opaqueQueue, alphaTestQueue, transparentQueue);
            }
        }
    }
}

void MeshRenderer::_updateBounds(BoundingBox3F &worldBounds) {
    if (_mesh != nullptr) {
        const auto localBounds = _mesh->bounds;
        const auto worldMatrix = _entity->transform->worldMatrix();
        worldBounds = localBounds.transform(worldMatrix);
    } else {
        worldBounds.lowerCorner = Point3F(0, 0, 0);
        worldBounds.upperCorner = Point3F(0, 0, 0);
    }
}

}// namespace vox
