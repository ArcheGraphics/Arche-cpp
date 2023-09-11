//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "../editor/editor_settings.h"

namespace vox {
// Load fonts, ini settings, texture and initialise an imgui context.
class ResourcesLoader {
public:
    ResourcesLoader();
    ~ResourcesLoader();

    // Return the settings that are going to be stored on disk
    static EditorSettings &GetEditorSettings();

    static int GetApplicationWidth();
    static int GetApplicationHeight();

private:
    static EditorSettings _editorSettings;
    static bool _resourcesLoaded;
};

}