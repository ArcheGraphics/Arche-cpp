//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "light_manager.h"
#include "shader/shader.h"
#include "ecs/scene.h"
#include "components/camera.h"
#include "framework/common/metal_helpers.h"

namespace vox {
LightManager *LightManager::getSingletonPtr() {
    return msSingleton;
}

LightManager &LightManager::getSingleton() {
    assert(msSingleton);
    return (*msSingleton);
}

LightManager::LightManager(MTL::Library &library, Scene *scene)
    : _library(library),
      _scene(scene),
      _pointLightProperty(Shader::createProperty("u_pointLight", ShaderDataGroup::Scene)),
      _spotLightProperty(Shader::createProperty("u_spotLight", ShaderDataGroup::Scene)),
      _directLightProperty(Shader::createProperty("u_directLight", ShaderDataGroup::Scene)),
      _forwardPlusProp(Shader::createProperty("u_cluster_uniform", ShaderDataGroup::Scene)),
      _clustersProp(Shader::createProperty("u_clusters", ShaderDataGroup::Compute)),
      _clusterLightsProp(Shader::createProperty("u_clusterLights", ShaderDataGroup::Scene)) {
    Shader::create("cluster_debug", "vertex_unlit", "fragment_cluster_debug");

    auto &device = _scene->device();
    _clustersBuffer = CLONE_METAL_CUSTOM_DELETER(MTL::Buffer, device.newBuffer(sizeof(Clusters),
                                                                               MTL::ResourceOptionCPUCacheModeDefault));
    _shaderData.setBufferFunctor(_clustersProp, [this]() -> auto {
        return _clustersBuffer;
    });

    _clusterLightsBuffer = CLONE_METAL_CUSTOM_DELETER(MTL::Buffer, device.newBuffer(sizeof(ClusterLightGroup),
                                                                                    MTL::ResourceOptionCPUCacheModeDefault));
    _scene->shaderData.setBufferFunctor(_clusterLightsProp, [this]() -> auto {
        return _clusterLightsBuffer;
    });

    _clusterBoundsCompute =
        std::make_unique<ComputePass>(_library, scene, "cluster_bounds");
    _clusterBoundsCompute->attachShaderData(&_shaderData);
    _clusterBoundsCompute->attachShaderData(&_scene->shaderData);
    _clusterBoundsCompute->setThreadsPerGrid(TILE_COUNT[0], TILE_COUNT[1], TILE_COUNT[2]);

    _clusterLightsCompute =
        std::make_unique<ComputePass>(_library, scene, "cluster_light");
    _clusterLightsCompute->attachShaderData(&_shaderData);
    _clusterLightsCompute->attachShaderData(&_scene->shaderData);
    _clusterLightsCompute->setThreadsPerGrid(TILE_COUNT[0], TILE_COUNT[1], TILE_COUNT[2]);
}

void LightManager::setCamera(Camera *camera) {
    _camera = camera;
    _clusterBoundsCompute->attachShaderData(&_camera->shaderData);
    _clusterLightsCompute->attachShaderData(&_camera->shaderData);
}

//MARK: - Point Light
void LightManager::attachPointLight(PointLight *light) {
    auto iter = std::find(_pointLights.begin(), _pointLights.end(), light);
    if (iter == _pointLights.end()) {
        _pointLights.push_back(light);
    } else {
        LOG(ERROR) << "Light already attached." << std::endl;
        ;
    }
}

void LightManager::detachPointLight(PointLight *light) {
    auto iter = std::find(_pointLights.begin(), _pointLights.end(), light);
    if (iter != _pointLights.end()) {
        _pointLights.erase(iter);
    }
}

const std::vector<PointLight *> &LightManager::pointLights() const {
    return _pointLights;
}

//MARK: - Spot Light
void LightManager::attachSpotLight(SpotLight *light) {
    auto iter = std::find(_spotLights.begin(), _spotLights.end(), light);
    if (iter == _spotLights.end()) {
        _spotLights.push_back(light);
    } else {
        LOG(ERROR) << "Light already attached." << std::endl;
        ;
    }
}

void LightManager::detachSpotLight(SpotLight *light) {
    auto iter = std::find(_spotLights.begin(), _spotLights.end(), light);
    if (iter != _spotLights.end()) {
        _spotLights.erase(iter);
    }
}

const std::vector<SpotLight *> &LightManager::spotLights() const {
    return _spotLights;
}

//MARK: - Direct Light
void LightManager::attachDirectLight(DirectLight *light) {
    auto iter = std::find(_directLights.begin(), _directLights.end(), light);
    if (iter == _directLights.end()) {
        _directLights.push_back(light);
    } else {
        LOG(ERROR) << "Light already attached." << std::endl;
        ;
    }
}

void LightManager::detachDirectLight(DirectLight *light) {
    auto iter = std::find(_directLights.begin(), _directLights.end(), light);
    if (iter != _directLights.end()) {
        _directLights.erase(iter);
    }
}

const std::vector<DirectLight *> &LightManager::directLights() const {
    return _directLights;
}

//MARK: - Internal Uploader
void LightManager::_updateShaderData(MTL::Device &device, ShaderData &shaderData) {
    size_t pointLightCount = _pointLights.size();
    _pointLightDatas.resize(pointLightCount);
    size_t spotLightCount = _spotLights.size();
    _spotLightDatas.resize(spotLightCount);
    size_t directLightCount = _directLights.size();
    _directLightDatas.resize(directLightCount);

    for (size_t i = 0; i < pointLightCount; i++) {
        _pointLights[i]->_updateShaderData(_pointLightDatas[i]);
    }

    for (size_t i = 0; i < spotLightCount; i++) {
        _spotLights[i]->_updateShaderData(_spotLightDatas[i]);
    }

    for (size_t i = 0; i < directLightCount; i++) {
        _directLights[i]->_updateShaderData(_directLightDatas[i]);
    }

    if (directLightCount) {
        if (_directLightBuffer == nullptr || _directLightBuffer->length() != sizeof(DirectLightData) * _directLightDatas.size()) {
            _directLightBuffer =
                CLONE_METAL_CUSTOM_DELETER(MTL::Buffer, device.newBuffer(_directLightDatas.data(),
                                                                         _directLightDatas.size() * sizeof(DirectLightData),
                                                                         MTL::ResourceOptionCPUCacheModeDefault));
        } else {
            memcpy(_directLightBuffer->contents(), _directLightDatas.data(), _directLightDatas.size() * sizeof(DirectLightData));
        }

        shaderData.enableMacro(DIRECT_LIGHT_COUNT, std::make_pair(directLightCount, MTL::DataTypeInt));
        shaderData.setData(LightManager::_directLightProperty, _directLightBuffer);
    } else {
        shaderData.disableMacro(DIRECT_LIGHT_COUNT);
    }

    if (pointLightCount) {
        if (_pointLightBuffer == nullptr || _pointLightBuffer->length() != sizeof(PointLightData) * _pointLightDatas.size()) {
            _pointLightBuffer =
                CLONE_METAL_CUSTOM_DELETER(MTL::Buffer, device.newBuffer(_pointLightDatas.data(),
                                                                         _pointLightDatas.size() * sizeof(PointLightData),
                                                                         MTL::ResourceOptionCPUCacheModeDefault));
        } else {
            memcpy(_pointLightBuffer->contents(), _pointLightDatas.data(), _pointLightDatas.size() * sizeof(PointLightData));
        }

        shaderData.enableMacro(POINT_LIGHT_COUNT, std::make_pair(pointLightCount, MTL::DataTypeInt));
        shaderData.setData(LightManager::_pointLightProperty, _pointLightBuffer);
    } else {
        shaderData.disableMacro(POINT_LIGHT_COUNT);
    }

    if (spotLightCount) {
        if (_spotLightBuffer == nullptr || _spotLightBuffer->length() != sizeof(SpotLightData) * _spotLightDatas.size()) {
            _spotLightBuffer =
                CLONE_METAL_CUSTOM_DELETER(MTL::Buffer, device.newBuffer(_spotLightDatas.data(),
                                                                         _spotLightDatas.size() * sizeof(SpotLightData),
                                                                         MTL::ResourceOptionCPUCacheModeDefault));
        } else {
            memcpy(_spotLightBuffer->contents(), _spotLightDatas.data(), _spotLightDatas.size() * sizeof(SpotLightData));
        }

        shaderData.enableMacro(SPOT_LIGHT_COUNT, std::make_pair(spotLightCount, MTL::DataTypeInt));
        shaderData.setData(LightManager::_spotLightProperty, _spotLightBuffer);
    } else {
        shaderData.disableMacro(SPOT_LIGHT_COUNT);
    }
}

//MARK: - Forward Plus
void LightManager::draw(MTL::CommandBuffer &commandBuffer) {
    _updateShaderData(_scene->device(), _scene->shaderData);

    size_t pointLightCount = _pointLights.size();
    size_t spotLightCount = _spotLights.size();
    if (pointLightCount + spotLightCount > FORWARD_PLUS_ENABLE_MIN_COUNT) {
        _scene->shaderData.enableMacro(NEED_FORWARD_PLUS);

        bool updateBounds = false;

        if (_forwardPlusUniforms.x != _camera->width() ||
            _forwardPlusUniforms.y != _camera->height()) {
            updateBounds = true;
            _forwardPlusUniforms.x = _camera->framebufferWidth();
            _forwardPlusUniforms.y = _camera->framebufferHeight();
        }
        _forwardPlusUniforms.z = _camera->nearClipPlane();
        _forwardPlusUniforms.w = _camera->farClipPlane();
        _scene->shaderData.setData(_forwardPlusProp, _forwardPlusUniforms);

        // Reset the light offset counter to 0 before populating the light clusters.
        uint32_t empty = 0;
        memcpy(_clusterLightsBuffer->contents(), &empty, sizeof(uint32_t));

        auto encoder = CLONE_METAL_CUSTOM_DELETER(MTL::ComputeCommandEncoder,
                                                  commandBuffer.computeCommandEncoder());
        if (updateBounds) {
            _clusterBoundsCompute->compute(*encoder);
        }
        _clusterLightsCompute->compute(*encoder);
        encoder->endEncoding();
    } else {
        _scene->shaderData.disableMacro(NEED_FORWARD_PLUS);
    }
}

}// namespace vox
