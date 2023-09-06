//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "renderer.h"
#include "entity.h"
#include "scene.h"
#include "shader/shader.h"
#include "material/material.h"

namespace vox {
size_t Renderer::materialCount() {
    return _materials.size();
}

BoundingBox3F Renderer::bounds() {
    auto &changeFlag = _transformChangeFlag;
    if (changeFlag->flag) {
        _updateBounds(_bounds);
        changeFlag->flag = false;
    }
    return _bounds;
}

Renderer::Renderer(Entity *entity) : Component(entity),
                                     _transformChangeFlag(entity->transform->registerWorldChangeFlag()),
                                     _localMatrixProperty(Shader::createProperty("u_localMat", ShaderDataGroup::Renderer)),
                                     _worldMatrixProperty(Shader::createProperty("u_modelMat", ShaderDataGroup::Renderer)),
                                     _mvMatrixProperty(Shader::createProperty("u_MVMat", ShaderDataGroup::Renderer)),
                                     _mvpMatrixProperty(Shader::createProperty("u_MVPMat", ShaderDataGroup::Renderer)),
                                     _mvInvMatrixProperty(Shader::createProperty("u_MVInvMat", ShaderDataGroup::Renderer)),
                                     _normalMatrixProperty(Shader::createProperty("u_normalMat", ShaderDataGroup::Renderer)) {
}

void Renderer::_onEnable() {
    auto &componentsManager = entity()->scene()->_componentsManager;
    componentsManager.addRenderer(this);
}

void Renderer::_onDisable() {
    auto &componentsManager = entity()->scene()->_componentsManager;
    componentsManager.removeRenderer(this);
}

void Renderer::_onDestroy() {
    auto &flag = _transformChangeFlag;
    if (flag != nullptr) {
        flag->destroy();
        _transformChangeFlag.reset();
    }
}

MaterialPtr Renderer::getInstanceMaterial(size_t index) {
    const auto &materials = _materials;
    if (materials.size() > index) {
        const auto &material = materials[index];
        if (material != nullptr) {
            if (_materialsInstanced[index]) {
                return material;
            } else {
                return _createInstanceMaterial(material, index);
            }
        }
    }
    return nullptr;
}

MaterialPtr Renderer::getMaterial(size_t index) {
    return _materials[index];
}

void Renderer::setMaterial(MaterialPtr material) {
    size_t index = 0;

    if (index >= _materials.size()) {
        _materials.reserve(index + 1);
        for (size_t i = _materials.size(); i <= index; i++) {
            _materials.push_back(nullptr);
        }
    }

    const auto &internalMaterial = _materials[index];
    if (internalMaterial != material) {
        _materials[index] = material;
        if (index < _materialsInstanced.size()) {
            _materialsInstanced[index] = false;
        }
    }
}

void Renderer::setMaterial(size_t index, MaterialPtr material) {
    if (index >= _materials.size()) {
        _materials.reserve(index + 1);
        for (size_t i = _materials.size(); i <= index; i++) {
            _materials.push_back(nullptr);
        }
    }

    const auto &internalMaterial = _materials[index];
    if (internalMaterial != material) {
        _materials[index] = material;
        if (index < _materialsInstanced.size()) {
            _materialsInstanced[index] = false;
        }
    }
}

std::vector<MaterialPtr> Renderer::getInstanceMaterials() {
    for (size_t i = 0; i < _materials.size(); i++) {
        if (!_materialsInstanced[i]) {
            _createInstanceMaterial(_materials[i], i);
        }
    }
    return _materials;
}

std::vector<MaterialPtr> Renderer::getMaterials() {
    return _materials;
}

void Renderer::setMaterials(const std::vector<MaterialPtr> &materials) {
    size_t count = materials.size();
    if (_materials.size() != count) {
        _materials.reserve(count);
        for (size_t i = _materials.size(); i < count; i++) {
            _materials.push_back(nullptr);
        }
    }
    if (_materialsInstanced.size() != 0) {
        _materialsInstanced.clear();
    }

    for (size_t i = 0; i < count; i++) {
        const auto &internalMaterial = _materials[i];
        const auto &material = materials[i];
        if (internalMaterial != material) {
            _materials[i] = material;
        }
    }
}

void Renderer::pushPrimitive(const RenderElement &element,
                             std::vector<RenderElement> &opaqueQueue,
                             std::vector<RenderElement> &alphaTestQueue,
                             std::vector<RenderElement> &transparentQueue) {
    const auto renderQueueType = element.material->renderQueueType;

    if (renderQueueType > (RenderQueueType::Transparent + RenderQueueType::AlphaTest) >> 1) {
        transparentQueue.push_back(element);
    } else if (renderQueueType > (RenderQueueType::AlphaTest + RenderQueueType::Opaque) >> 1) {
        alphaTestQueue.push_back(element);
    } else {
        opaqueQueue.push_back(element);
    }
}

void Renderer::setDistanceForSort(float dist) {
    _distanceForSort = dist;
}

float Renderer::distanceForSort() {
    return _distanceForSort;
}

void Renderer::updateShaderData(const Matrix4x4F &viewMat,
                                const Matrix4x4F &projMat) {
    auto worldMatrix = entity()->transform->worldMatrix();
    _mvMatrix = viewMat * worldMatrix;
    _mvpMatrix = projMat * viewMat * worldMatrix;
    _mvInvMatrix = _mvMatrix.inverse();
    _normalMatrix = worldMatrix.inverse();
    _normalMatrix = _normalMatrix.transposed();

    shaderData.setData(Renderer::_localMatrixProperty, entity()->transform->localMatrix());
    shaderData.setData(Renderer::_worldMatrixProperty, worldMatrix);
    shaderData.setData(Renderer::_mvMatrixProperty, _mvMatrix);
    shaderData.setData(Renderer::_mvpMatrixProperty, _mvpMatrix);
    shaderData.setData(Renderer::_mvInvMatrixProperty, _mvInvMatrix);
    shaderData.setData(Renderer::_normalMatrixProperty, _normalMatrix);
}

MaterialPtr Renderer::_createInstanceMaterial(const MaterialPtr &material, size_t index) {
    return nullptr;
}

}// namespace vox
