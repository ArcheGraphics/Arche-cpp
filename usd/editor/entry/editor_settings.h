//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <list>
#include <string>
#include <utility>
#include <vector>

struct ImGuiTextBuffer;

namespace vox {
/// EditorSettings contains all the entry variables we want to persist between sessions
struct EditorSettings {
    EditorSettings(int width, int height);

    /// Editor windows states
    bool _showDebugWindow = false;
    bool _showPropertyEditor = true;
    bool _showOutliner = true;
    bool _showTimeline = false;
    bool _showLayerHierarchyEditor = false;
    bool _showLayerStackEditor = false;
    bool _showContentBrowser = false;
    bool _showPrimSpecEditor = false;
    bool _showViewport = false;
    bool _showStatusBar = true;
    bool _showLauncherBar = false;
    bool _textEditor = false;
    bool _showSdfAttributeEditor = false;
    int _mainWindowWidth;
    int _mainWindowHeight;

    /// Last file browser directory
    std::string _lastFileBrowserDirectory;

    /// Additional plugin paths -  It really belongs to an ApplicationSettings but we want to edit it
    /// in the entry - This might move in the future
    std::vector<std::string> _pluginPaths;

    /// Blueprints root location on disk
    std::vector<std::string> _blueprintLocations;

    // Add new file to the list of recent files
    void update_recent_files(const std::string &newFile);

    const std::list<std::string> &get_recent_files() const { return _recentFiles; }

    // Launcher commands.
    // We maintain a mapping between the commandName and the commandLine while keeping
    // the order as well. As we expect a very few number of commands, we store them
    // in 2 vectors instead of a map, it is more efficient as the code iterates on the command name list
    bool add_launcher(const std::string &commandName, const std::string &commandLine);

    bool remove_launcher(const std::string &commandName);

    const std::vector<std::string> &get_launcher_name_list() const { return _launcherNames; };

    std::string get_launcher_command_line(const std::string &commandName) const;

    // Serialization functions
    void parse_line(const char *line);

    void dump(ImGuiTextBuffer *);

private:
    /// Recent files
    std::list<std::string> _recentFiles;

    /// Launcher commands
    std::vector<std::string> _launcherNames;
    std::vector<std::string> _launcherCommandLines;
};

}// namespace vox