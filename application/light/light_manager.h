//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "base/singleton.h"
#include "light/direct_light.h"
#include "light/point_light.h"
#include "light/spot_light.h"
#include "framework/shader/shader_data.h"

namespace vox {
class Scene;
class Camera;

/**
 * Light Manager.
 */
class LightManager : public Singleton<LightManager> {
public:
    static LightManager &get_singleton();

    static LightManager *get_singleton_ptr();

    explicit LightManager(Scene *scene);

    /**
     * Register a light object to the current scene.
     * @param light render light
     */
    void attach_point_light(PointLight *light);

    /**
     * Remove a light object from the current scene.
     * @param light render light
     */
    void detach_point_light(PointLight *light);

    [[nodiscard]] const std::vector<PointLight *> &get_point_lights() const;

public:
    /**
     * Register a light object to the current scene.
     * @param light render light
     */
    void attach_spot_light(SpotLight *light);

    /**
     * Remove a light object from the current scene.
     * @param light render light
     */
    void detach_spot_light(SpotLight *light);

    [[nodiscard]] const std::vector<SpotLight *> &get_spot_lights() const;

public:
    /**
     * Register a light object to the current scene.
     * @param light direct light
     */
    void attach_direct_light(DirectLight *light);

    /**
     * Remove a light object from the current scene.
     * @param light direct light
     */
    void detach_direct_light(DirectLight *light);

    [[nodiscard]] const std::vector<DirectLight *> &get_direct_lights() const;

private:
    Scene *scene_{nullptr};
    Camera *camera_{nullptr};

    std::vector<PointLight *> point_lights_;
    std::vector<PointLight::PointLightData> point_light_datas_;
    const std::string point_light_property_;

    std::vector<SpotLight *> spot_lights_;
    std::vector<SpotLight::SpotLightData> spot_light_datas_;
    const std::string spot_light_property_;

    std::vector<DirectLight *> direct_lights_;
    std::vector<DirectLight::DirectLightData> direct_light_datas_;
    const std::string direct_light_property_;

    void update_shader_data(ShaderData &shader_data);
};

template<>
inline LightManager *Singleton<LightManager>::ms_singleton{nullptr};

}// namespace vox
