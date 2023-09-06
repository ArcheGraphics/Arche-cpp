//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "scene.h"
#include "vector2.h"
#include "vector3.h"
#include "vector4.h"
#include "color.h"
#include "matrix4x4.h"

#include <queue>
#include <glog/logging.h>
#include "entity.h"
#include "camera.h"

namespace vox {
Scene::Scene(MTL::Device &device) : _device(device),
                                    _ambientLight(this) {
    _vertexUploader = {
        toAnyUploader<bool, MTL::RenderCommandEncoder>([](const bool &x, size_t location, MTL::RenderCommandEncoder &encoder) {
            encoder.setVertexBytes(&x, sizeof(bool), location);
        }),
        toAnyUploader<int32_t, MTL::RenderCommandEncoder>([](const int &x, size_t location, MTL::RenderCommandEncoder &encoder) {
            encoder.setVertexBytes(&x, sizeof(int), location);
        }),
        toAnyUploader<uint32_t, MTL::RenderCommandEncoder>([](const int &x, size_t location, MTL::RenderCommandEncoder &encoder) {
            encoder.setVertexBytes(&x, sizeof(int), location);
        }),
        toAnyUploader<float, MTL::RenderCommandEncoder>([](const float &x, size_t location, MTL::RenderCommandEncoder &encoder) {
            encoder.setVertexBytes(&x, sizeof(float), location);
        }),
        toAnyUploader<Vector2F, MTL::RenderCommandEncoder>([](const Vector2F &x, size_t location, MTL::RenderCommandEncoder &encoder) {
            encoder.setVertexBytes(&x, sizeof(Vector2F), location);
        }),
        toAnyUploader<Vector3F, MTL::RenderCommandEncoder>([](const Vector3F &x, size_t location, MTL::RenderCommandEncoder &encoder) {
            encoder.setVertexBytes(&x, sizeof(Vector4F), location);// float3 simd is extented from float4
        }),
        toAnyUploader<Point3F, MTL::RenderCommandEncoder>([](const Point3F &x, size_t location, MTL::RenderCommandEncoder &encoder) {
            encoder.setVertexBytes(&x, sizeof(Vector4F), location);// float3 simd is extented from float4
        }),
        toAnyUploader<Vector4F, MTL::RenderCommandEncoder>([](const Vector4F &x, size_t location, MTL::RenderCommandEncoder &encoder) {
            encoder.setVertexBytes(&x, sizeof(Vector4F), location);
        }),
        toAnyUploader<Color, MTL::RenderCommandEncoder>([](const Color &x, size_t location, MTL::RenderCommandEncoder &encoder) {
            encoder.setVertexBytes(&x, sizeof(Color), location);
        }),
        toAnyUploader<Matrix4x4F, MTL::RenderCommandEncoder>([](const Matrix4x4F &x, size_t location, MTL::RenderCommandEncoder &encoder) {
            encoder.setVertexBytes(&x, sizeof(Matrix4x4F), location);
        }),
        toAnyUploader<std::shared_ptr<MTL::Buffer>, MTL::RenderCommandEncoder>([](const std::shared_ptr<MTL::Buffer> &x, size_t location,
                                                                                  MTL::RenderCommandEncoder &encoder) {
            encoder.setVertexBuffer(x.get(), 0, location);
        }),
        toAnyUploader<SampledTexturePtr, MTL::RenderCommandEncoder>([](const SampledTexturePtr &x, size_t location,
                                                                       MTL::RenderCommandEncoder &encoder) {
            encoder.setVertexTexture(x->textureView().get(), location);
            encoder.setVertexSamplerState(&x->sampler(), location);
        }),
    };

    _fragmentUploader = {
        toAnyUploader<bool, MTL::RenderCommandEncoder>([](const bool &x, size_t location, MTL::RenderCommandEncoder &encoder) {
            encoder.setFragmentBytes(&x, sizeof(bool), location);
        }),
        toAnyUploader<int32_t, MTL::RenderCommandEncoder>([](const int &x, size_t location, MTL::RenderCommandEncoder &encoder) {
            encoder.setFragmentBytes(&x, sizeof(int), location);
        }),
        toAnyUploader<uint32_t, MTL::RenderCommandEncoder>([](const int &x, size_t location, MTL::RenderCommandEncoder &encoder) {
            encoder.setFragmentBytes(&x, sizeof(int), location);
        }),
        toAnyUploader<float, MTL::RenderCommandEncoder>([](const float &x, size_t location, MTL::RenderCommandEncoder &encoder) {
            encoder.setFragmentBytes(&x, sizeof(float), location);
        }),
        toAnyUploader<Vector2F, MTL::RenderCommandEncoder>([](const Vector2F &x, size_t location, MTL::RenderCommandEncoder &encoder) {
            encoder.setFragmentBytes(&x, sizeof(Vector2F), location);
        }),
        toAnyUploader<Vector3F, MTL::RenderCommandEncoder>([](const Vector3F &x, size_t location, MTL::RenderCommandEncoder &encoder) {
            encoder.setFragmentBytes(&x, sizeof(Vector4F), location);// float3 simd is extented from float4
        }),
        toAnyUploader<Point3F, MTL::RenderCommandEncoder>([](const Point3F &x, size_t location, MTL::RenderCommandEncoder &encoder) {
            encoder.setFragmentBytes(&x, sizeof(Vector4F), location);// float3 simd is extented from float4
        }),
        toAnyUploader<Vector4F, MTL::RenderCommandEncoder>([](const Vector4F &x, size_t location, MTL::RenderCommandEncoder &encoder) {
            encoder.setFragmentBytes(&x, sizeof(Vector4F), location);
        }),
        toAnyUploader<Color, MTL::RenderCommandEncoder>([](const Color &x, size_t location, MTL::RenderCommandEncoder &encoder) {
            encoder.setFragmentBytes(&x, sizeof(Color), location);
        }),
        toAnyUploader<Matrix4x4F, MTL::RenderCommandEncoder>([](const Matrix4x4F &x, size_t location, MTL::RenderCommandEncoder &encoder) {
            encoder.setFragmentBytes(&x, sizeof(Matrix4x4F), location);
        }),
        toAnyUploader<std::shared_ptr<MTL::Buffer>, MTL::RenderCommandEncoder>([](const std::shared_ptr<MTL::Buffer> &x, size_t location,
                                                                                  MTL::RenderCommandEncoder &encoder) {
            encoder.setFragmentBuffer(x.get(), 0, location);
        }),
        toAnyUploader<SampledTexturePtr, MTL::RenderCommandEncoder>([](const SampledTexturePtr &x, size_t location,
                                                                       MTL::RenderCommandEncoder &encoder) {
            encoder.setFragmentTexture(x->textureView().get(), location);
            encoder.setFragmentSamplerState(&x->sampler(), location);
        }),
    };

    _computeUploader = {
        toAnyUploader<int32_t, MTL::ComputeCommandEncoder>([](const int32_t &x, size_t location, MTL::ComputeCommandEncoder &encoder) {
            encoder.setBytes(&x, sizeof(int32_t), location);
        }),
        toAnyUploader<uint32_t, MTL::ComputeCommandEncoder>([](const uint32_t &x, size_t location, MTL::ComputeCommandEncoder &encoder) {
            encoder.setBytes(&x, sizeof(uint32_t), location);
        }),
        toAnyUploader<float, MTL::ComputeCommandEncoder>([](const float &x, size_t location, MTL::ComputeCommandEncoder &encoder) {
            encoder.setBytes(&x, sizeof(float), location);
        }),
        toAnyUploader<Vector2F, MTL::ComputeCommandEncoder>([](const Vector2F &x, size_t location, MTL::ComputeCommandEncoder &encoder) {
            encoder.setBytes(&x, sizeof(Vector2F), location);
        }),
        toAnyUploader<Vector3F, MTL::ComputeCommandEncoder>([](const Vector3F &x, size_t location, MTL::ComputeCommandEncoder &encoder) {
            encoder.setBytes(&x, sizeof(Vector4F), location);// float3 simd is extented from float4
        }),
        toAnyUploader<Point3F, MTL::ComputeCommandEncoder>([](const Point3F &x, size_t location, MTL::ComputeCommandEncoder &encoder) {
            encoder.setBytes(&x, sizeof(Vector4F), location);// float3 simd is extented from float4
        }),
        toAnyUploader<Vector4F, MTL::ComputeCommandEncoder>([](const Vector4F &x, size_t location, MTL::ComputeCommandEncoder &encoder) {
            encoder.setBytes(&x, sizeof(Vector4F), location);
        }),
        toAnyUploader<Color, MTL::ComputeCommandEncoder>([](const Color &x, size_t location, MTL::ComputeCommandEncoder &encoder) {
            encoder.setBytes(&x, sizeof(Color), location);
        }),
        toAnyUploader<Matrix4x4F, MTL::ComputeCommandEncoder>([](const Matrix4x4F &x, size_t location, MTL::ComputeCommandEncoder &encoder) {
            encoder.setBytes(&x, sizeof(Matrix4x4F), location);
        }),
        toAnyUploader<std::shared_ptr<MTL::Buffer>, MTL::ComputeCommandEncoder>([](const std::shared_ptr<MTL::Buffer> &x, size_t location,
                                                                                   MTL::ComputeCommandEncoder &encoder) {
            encoder.setBuffer(x.get(), 0, location);
        }),
        toAnyUploader<SampledTexturePtr, MTL::ComputeCommandEncoder>([](const SampledTexturePtr &x, size_t location,
                                                                        MTL::ComputeCommandEncoder &encoder) {
            encoder.setTexture(x->textureView().get(), location);
            encoder.setSamplerState(&x->sampler(), location);
        }),
    };
    _ambientLight.registerUploader(this);
}

MTL::Device &Scene::device() {
    return _device;
}

const std::unordered_map<std::type_index, std::function<void(std::any const &, size_t, MTL::RenderCommandEncoder &)>> &
Scene::vertexUploader() {
    return _vertexUploader;
}

const std::unordered_map<std::type_index, std::function<void(std::any const &, size_t, MTL::RenderCommandEncoder &)>> &
Scene::fragmentUploader() {
    return _fragmentUploader;
}

const std::unordered_map<std::type_index, std::function<void(std::any const &, size_t, MTL::ComputeCommandEncoder &)>> &
Scene::computeUploader() {
    return _computeUploader;
}

AmbientLight &Scene::ambientLight() {
    return _ambientLight;
}

size_t Scene::rootEntitiesCount() {
    return _rootEntities.size();
}

const std::vector<EntityPtr> &Scene::rootEntities() {
    return _rootEntities;
}

bool Scene::destroyed() {
    return _destroyed;
}

EntityPtr Scene::createRootEntity(std::string name) {
    const auto entity = std::make_shared<Entity>(name);
    addRootEntity(entity);
    return entity;
}

void Scene::addRootEntity(EntityPtr entity) {
    const auto isRoot = entity->_isRoot;

    // let entity become root
    if (!isRoot) {
        entity->_isRoot = true;
        entity->_removeFromParent();
    }

    // add or remove from scene's rootEntities
    const auto oldScene = entity->_scene;
    if (oldScene != this) {
        if (oldScene && isRoot) {
            oldScene->_removeEntity(entity);
        }
        _rootEntities.push_back(entity);
        Entity::_traverseSetOwnerScene(entity.get(), this);
    } else if (!isRoot) {
        _rootEntities.push_back(entity);
    }

    // process entity active/inActive
    if (!entity->_isActiveInHierarchy && entity->_isActive) {
        entity->_processActive();
    }
}

void Scene::removeRootEntity(EntityPtr entity) {
    if (entity->_isRoot && entity->_scene == this) {
        _removeEntity(entity);
        entity->_processInActive();
        Entity::_traverseSetOwnerScene(entity.get(), nullptr);
    }
}

EntityPtr Scene::getRootEntity(size_t index) {
    return _rootEntities[index];
}

EntityPtr Scene::findEntityByName(const std::string &name) {
    const auto &children = _rootEntities;
    for (size_t i = 0; i < children.size(); i++) {
        const auto &child = children[i];
        if (child->name == name) {
            return child;
        }
    }

    for (size_t i = 0; i < children.size(); i++) {
        const auto &child = children[i];
        const auto entity = child->findByName(name);
        if (entity) {
            return entity;
        }
    }
    return nullptr;
}

void Scene::destroy() {
    if (_destroyed) {
        return;
    }
    for (size_t i = 0, n = rootEntitiesCount(); i < n; i++) {
        _rootEntities[i]->destroy();
    }
    _rootEntities.clear();
    _destroyed = true;
}

void Scene::attachRenderCamera(Camera *camera) {
    auto iter = std::find(_activeCameras.begin(), _activeCameras.end(), camera);
    if (iter == _activeCameras.end()) {
        _activeCameras.push_back(camera);
    } else {
        LOG(INFO) << "Camera already attached." << std::endl;
    }
}

void Scene::detachRenderCamera(Camera *camera) {
    auto iter = std::find(_activeCameras.begin(), _activeCameras.end(), camera);
    if (iter != _activeCameras.end()) {
        _activeCameras.erase(iter);
    }
}

void Scene::_processActive(bool active) {
    const auto &rootEntities = _rootEntities;
    for (size_t i = 0; i < rootEntities.size(); i++) {
        const auto &entity = rootEntities[i];
        if (entity->_isActive) {
            active ? entity->_processActive() : entity->_processInActive();
        }
    }
}

void Scene::_removeEntity(EntityPtr entity) {
    auto &oldRootEntities = _rootEntities;
    oldRootEntities.erase(std::remove(oldRootEntities.begin(),
                                      oldRootEntities.end(), entity),
                          oldRootEntities.end());
}

//MARK: - Update Loop
void Scene::updateShaderData() {
    // union scene and camera macro.
    for (auto &camera : _activeCameras) {
        camera->update();
    }
}

void Scene::update(float deltaTime) {
    _componentsManager.callScriptOnStart();

    _physicsManager.callColliderOnUpdate();
    _physicsManager.update(deltaTime);
    _physicsManager.callColliderOnLateUpdate();
    _physicsManager.callCharacterControllerOnLateUpdate();

    _componentsManager.callScriptOnUpdate(deltaTime);
    _componentsManager.callAnimatorUpdate(deltaTime);
    _componentsManager.callSceneAnimatorUpdate(deltaTime);
    _componentsManager.callScriptOnLateUpdate(deltaTime);

    _componentsManager.callRendererOnUpdate(deltaTime);

    updateShaderData();
}

void Scene::updateInputEvent(const InputEvent &inputEvent) {
    _componentsManager.callScriptInputEvent(inputEvent);
}

void Scene::updateSize(uint32_t win_width, uint32_t win_height,
                       uint32_t fb_width, uint32_t fb_height) {
    _componentsManager.callScriptResize(win_width, win_height,
                                        fb_width, fb_height);
}

}// namespace vox
