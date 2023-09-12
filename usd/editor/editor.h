//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "entry/editor_settings.h"
#include "selection.h"
#include "manipulators/viewport.h"
#include <pxr/usd/sdf/layer.h>
#include <pxr/usd/sdf/primSpec.h>
#include <pxr/usd/usdUtils/stageCache.h>

#include <set>
#include <future>

struct GLFWwindow;

PXR_NAMESPACE_USING_DIRECTIVE

namespace vox {
/// Editor contains the data shared between widgets, like selections, stages, etc etc
class Editor {

public:
    Editor();
    ~Editor();

    /// Removing the copy constructors as we want to make sure there are no unwanted copies of the
    /// editor. There should be only one editor for now but we want to control the construction
    /// and destruction of the editor to delete properly the contexts, so it's not a singleton
    Editor(const Editor &) = delete;
    Editor &operator=(const Editor &) = delete;

    /// Calling Shutdown will stop the main loop
    void shutdown() { _isShutdown = true; }
    bool is_shutdown() const { return _isShutdown; }
    void request_shutdown();
    void confirm_shutdown(std::string why);

    /// Check if there are some unsaved work, looking at all the layers dirtyness
    bool has_unsaved_work();

    /// Sets the current edited layer
    void set_current_layer(SdfLayerRefPtr layer, bool showContentBrowser = false);
    SdfLayerRefPtr get_current_layer();
    void set_previous_layer();// go backward in the layer history
    void set_next_layer();    // go forward in the layer history

    /// List of stages
    /// Using a stage cache to store the stages, seems to work well
    UsdStageRefPtr get_current_stage() { return _currentStage; }
    void set_current_stage(UsdStageCache::Id current);
    void set_current_stage(UsdStageRefPtr stage);
    void set_current_edit_target(SdfLayerHandle layer);

    UsdStageCache &get_stage_cache() { return _stageCache.Get(); }

    /// Returns the selected primspec
    /// There should be one selected primspec per layer ideally, so it's very likely this function will move
    Selection &get_selection() { return _selection; }
    void set_layer_path_selection(const SdfPath &primPath);
    void add_layer_path_selection(const SdfPath &primPath);
    void set_stage_path_selection(const SdfPath &primPath);
    void add_stage_path_selection(const SdfPath &primPath);

    /// Create a new layer in file path
    void create_new_layer(const std::string &path);
    void find_or_open_lLayer(const std::string &path);
    void create_stage(const std::string &path);
    void open_stage(const std::string &path, bool openLoaded = true);
    void save_layer_as(SdfLayerRefPtr layer, const std::string &path);

    /// Render the hydra viewport
    void hydra_render();

    ///
    /// Drawing functions for the main editor
    ///

    /// Draw the menu bar
    void draw_main_menu_bar();

    /// Draw the UI
    void draw();

    // GLFW callbacks
    void install_callbacks(GLFWwindow *window);
    void remove_callbacks(GLFWwindow *window);

    /// There is only one viewport for now, but could have multiple in the future
    Viewport &get_viewport();

    /// Playback controls
    void start_playback() { get_viewport().start_playback(); };
    void stop_playback() { get_viewport().stop_playback(); };

    void show_dialog_save_layer_as(SdfLayerHandle layerToSaveAs);

    // Launcher functions
    const std::vector<std::string> &get_launcher_name_list() const { return _settings.get_launcher_name_list(); }
    bool add_launcher(const std::string &launcherName, const std::string &commandLine) {
        return _settings.add_launcher(launcherName, commandLine);
    }
    bool remove_launcher(std::string launcherName) { return _settings.remove_launcher(launcherName); };
    void run_launcher(const std::string &launcherName);

    // Additional plugin paths kept in the settings
    inline const std::vector<std::string> &get_plugin_paths() const { return _settings._pluginPaths; }
    inline void add_plugin_path(const std::string &path) { _settings._pluginPaths.push_back(path); }
    inline void remove_plugin_path(const std::string &path) {
        const auto &found = std::find(_settings._pluginPaths.begin(), _settings._pluginPaths.end(), path);
        if (found != _settings._pluginPaths.end()) {
            _settings._pluginPaths.erase(found);
        }
    }


private:
    /// Interface with the settings
    void load_settings();
    void save_settings() const;

    /// glfw callback to handle drag and drop from external applications
    static void drop_callback(GLFWwindow *window, int count, const char **paths);

    /// glfw callback to close the application
    static void window_close_callback(GLFWwindow *window);

    /// glfw resize callback
    static void window_size_callback(GLFWwindow *window, int width, int height);

    /// Using a stage cache to store the stages, seems to work well
    UsdUtilsStageCache _stageCache;

    /// List of layers.
    SdfLayerRefPtrVector _layerHistory;
    size_t _layerHistoryPointer;

    /// Setting _isShutdown to true will stop the main loop
    bool _isShutdown = false;

    ///
    /// Editor settings contains the persisted data
    ///
    EditorSettings _settings;

    UsdStageRefPtr _currentStage;
    Viewport _viewport;

    /// Selection for stages and layers
    Selection _selection;

    /// Selected attribute, for showing in the spreadsheet or metadata
    SdfPath _selectedAttribute;

    /// Storing the tasks created by launchers.
    std::vector<std::future<int>> _launcherTasks;
};

}// namespace vox