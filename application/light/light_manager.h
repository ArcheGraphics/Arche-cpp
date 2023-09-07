//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "point_light.h"
#include "spot_light.h"
#include "direct_light.h"
#include "shader/shader_data.h"
#include "rendering/compute_pass.h"
#include <Metal/Metal.hpp>
#include "base/singleton.h"

namespace vox {
/**
 * Light Manager.
 */
class LightManager : public Singleton<LightManager> {
public:
    static constexpr uint32_t FORWARD_PLUS_ENABLE_MIN_COUNT = 20;

    static constexpr std::array<uint32_t, 3> TILE_COUNT = {32, 18, 48};
    static constexpr uint32_t TOTAL_TILES = TILE_COUNT[0] * TILE_COUNT[1] * TILE_COUNT[2];
    // Each cluster tracks up to MAX_LIGHTS_PER_CLUSTER light indices (ints) and one light count.
    // This limitation should be able to go away when we have atomic methods in WGSL.
    static constexpr uint32_t MAX_LIGHTS_PER_CLUSTER = 50;
    static constexpr uint32_t CLUSTER_LIGHTS_SIZE = (8 * TOTAL_TILES) + (4 * MAX_LIGHTS_PER_CLUSTER * TOTAL_TILES) + 4;

    static LightManager &getSingleton(void);

    static LightManager *getSingletonPtr(void);

    LightManager(MTL::Library &library, Scene *scene);

    void setCamera(Camera *camera);

    /**
     * Register a light object to the current scene.
     * @param light render light
     */
    void attachPointLight(PointLight *light);

    /**
     * Remove a light object from the current scene.
     * @param light render light
     */
    void detachPointLight(PointLight *light);

    const std::vector<PointLight *> &pointLights() const;

public:
    /**
     * Register a light object to the current scene.
     * @param light render light
     */
    void attachSpotLight(SpotLight *light);

    /**
     * Remove a light object from the current scene.
     * @param light render light
     */
    void detachSpotLight(SpotLight *light);

    const std::vector<SpotLight *> &spotLights() const;

public:
    /**
     * Register a light object to the current scene.
     * @param light direct light
     */
    void attachDirectLight(DirectLight *light);

    /**
     * Remove a light object from the current scene.
     * @param light direct light
     */
    void detachDirectLight(DirectLight *light);

    const std::vector<DirectLight *> &directLights() const;

public:
    void draw(MTL::CommandBuffer &commandBuffer);

private:
    MTL::Library &_library;
    Scene *_scene{nullptr};
    Camera *_camera{nullptr};

    std::vector<PointLight *> _pointLights;
    std::vector<PointLightData> _pointLightDatas;
    std::shared_ptr<MTL::Buffer> _pointLightBuffer;
    ShaderProperty _pointLightProperty;

    std::vector<SpotLight *> _spotLights;
    std::vector<SpotLightData> _spotLightDatas;
    std::shared_ptr<MTL::Buffer> _spotLightBuffer;
    ShaderProperty _spotLightProperty;

    std::vector<DirectLight *> _directLights;
    std::vector<DirectLightData> _directLightDatas;
    std::shared_ptr<MTL::Buffer> _directLightBuffer;
    ShaderProperty _directLightProperty;

    void _updateShaderData(MTL::Device &device, ShaderData &shaderData);

private:
    Vector4F _forwardPlusUniforms;// outputSize, zNear, zFar
    ShaderProperty _forwardPlusProp;

    struct ClusterBounds {
        Vector3F minAABB;
        float _pad1;
        Vector3F maxAABB;
        float _pad2;
    };
    struct Clusters {
        std::array<ClusterBounds, TOTAL_TILES> bounds;
    };
    ShaderProperty _clustersProp;
    std::shared_ptr<MTL::Buffer> _clustersBuffer;

    struct ClusterLights {
        uint32_t offset;
        uint32_t point_count;
        uint32_t spot_count;
    };
    struct ClusterLightGroup {
        uint32_t offset;
        std::array<ClusterLights, TOTAL_TILES> lights;
        std::array<uint32_t, MAX_LIGHTS_PER_CLUSTER * TOTAL_TILES> indices;
    };
    ShaderProperty _clusterLightsProp;
    std::shared_ptr<MTL::Buffer> _clusterLightsBuffer;

    ShaderData _shaderData;
    std::unique_ptr<ComputePass> _clusterBoundsCompute{nullptr};
    std::unique_ptr<ComputePass> _clusterLightsCompute{nullptr};
};

template<>
inline LightManager *Singleton<LightManager>::msSingleton{nullptr};

}// namespace vox