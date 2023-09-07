//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "base/singleton.h"
#include "ecs/scene.h"

namespace vox {
/**
 * The scene manager of the current scene
 */
class SceneManager : public Singleton<SceneManager> {
public:
    static SceneManager &get_singleton();

    static SceneManager *get_singleton_ptr();

    /**
     * Default constructor
     * @param p_scene_root_folder (Optional)
     */
    explicit SceneManager(MTL::Device &device, std::string p_scene_root_folder = "");

    /**
     * Default destructor
     */
    ~SceneManager();

    /**
     * Update
     */
    void update();

    /**
     * Load an empty scene in memory
     */
    void load_empty_scene();

    /**
     * Load an empty lighted scene in memory
     */
    void load_empty_lighted_scene();

    /**
     * Destroy current scene from memory
     */
    void unload_current_scene();

    /**
     * Return true if a scene is currently loaded
     */
    [[nodiscard]] bool has_current_scene() const;

    /*
     * Return current loaded scene
     */
    Scene *get_current_scene();

    /**
     * Return the current scene source path
     */
    [[nodiscard]] std::string get_current_scene_source_path() const;

    /**
     * Return true if the currently loaded scene has been loaded from a file
     */
    [[nodiscard]] bool is_current_scene_loaded_from_disk() const;

    /**
     * Store the given path as the current scene source path
     * @param p_path p_path
     */
    void store_current_scene_source_path(const std::string &p_path);

    /**
     * Reset the current scene source path to an empty string
     */
    void forget_current_scene_source_path();

private:
    MTL::Device &device_;
    const std::string scene_root_folder_;
    std::unique_ptr<Scene> current_scene_{nullptr};

    bool current_scene_loaded_from_path_{false};
    std::string current_scene_source_path_;

    std::function<void()> delayed_load_call_;
};

template<>
inline SceneManager *Singleton<SceneManager>::ms_singleton{nullptr};

}// namespace vox
