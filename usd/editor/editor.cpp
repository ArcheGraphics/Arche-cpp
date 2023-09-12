//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <pxr/imaging/garch/glApi.h>
#include <pxr/base/arch/fileSystem.h>
#include <pxr/usd/sdf/fileFormat.h>
#include <pxr/usd/sdf/layer.h>
#include <pxr/usd/usd/editTarget.h>
#include <pxr/usd/usd/primRange.h>
#include <pxr/base/trace/trace.h>
#include "editor.h"
#include "widgets/debug.h"
#include "widgets/sdf_layer_editor.h"
#include "widgets/sdf_layer_scene_graph_editor.h"
#include "widgets/file_browser.h"
#include "widgets/usd_prim_editor.h"
#include "widgets/modal_dialogs.h"
#include "widgets/stage_outliner.h"
#include "widgets/timeline.h"
#include "widgets/content_browser.h"
#include "widgets/sdf_prim_editor.h"
#include "base/constants.h"
#include "commands/commands.h"
#include "fonts/resources_loader.h"
#include "widgets/sdf_attribute_editor.h"
#include "widgets/text_editor.h"
#include "commands/shortcuts.h"
#include "widgets/stage_layer_editor.h"
#include "widgets/launcher_bar.h"
#include "manipulators/playblast.h"
#include "blueprints.h"
#include "base/usd_helpers.h"
#include "fonts/IconsFontAwesome5.h"

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_stdlib.h>

#include <utility>

namespace vox {
// There is a bug in the Undo/Redo when reloading certain layers, here is the post
// that explains how to debug the issue:
// Reloading model.stage doesn't work but reloading stage separately does
// https://groups.google.com/u/1/g/usd-interest/c/lRTmWgq78dc/m/HOZ6x9EdCQAJ

// Using define instead of constexpr because the TRACE_SCOPE doesn't work without string literals.
// TODO: find a way to use constexpr and add trace
#define DebugWindowTitle "Debug window"
#define ContentBrowserWindowTitle "Content browser"
#define UsdStageHierarchyWindowTitle "Stage outliner"
#define UsdPrimPropertiesWindowTitle "Stage property editor"
#define SdfLayerHierarchyWindowTitle "Layer hierarchy"
#define SdfLayerStackWindowTitle "Stage layer editor"
#define SdfPrimPropertiesWindowTitle "Layer property editor"
#define SdfLayerAsciiEditorWindowTitle "Layer text editor"
#define SdfAttributeWindowTitle "Attribute editor"
#define TimelineWindowTitle "Timeline"
#define ViewportWindowTitle "Viewport"
#define StatusBarWindowTitle "Status bar"
#define LauncherBarWindowTitle "Launcher bar"

// Used only in the editor, so no point adding them to ImGuiHelpers yet
inline bool BelongToSameDockTab(ImGuiWindow *w1, ImGuiWindow *w2) {
    if (!w1 || !w2)
        return false;
    if (!w1->RootWindow || !w2->RootWindow)
        return false;
    if (!w1->RootWindow->DockNode || !w2->RootWindow->DockNode)
        return false;
    if (!w1->RootWindow->DockNode->TabBar || !w2->RootWindow->DockNode->TabBar)
        return false;
    return w1->RootWindow->DockNode->TabBar == w2->RootWindow->DockNode->TabBar;
}

inline void BringWindowToTabFront(const char *windowName) {
    ImGuiContext &g = *GImGui;
    if (ImGuiWindow *window = ImGui::FindWindowByName(windowName)) {
        if (g.NavWindow != window && !BelongToSameDockTab(window, g.HoveredWindow)) {
            ImGuiDockNode *dockNode = window ? window->DockNode : nullptr;
            if (dockNode && dockNode->TabBar) {
                dockNode->TabBar->SelectedTabId = dockNode->TabBar->NextSelectedTabId = window->TabId;
            }
        }
    }
}

struct AboutModalDialog : public ModalDialog {
    explicit AboutModalDialog(Editor &editor) : editor(editor) {}
    void draw() override {
        ImGui::Text("This is a pre-alpha version for testing purpose.");
        ImGui::Text("Please send your feedbacks as github issues:");
        ImGui::Text("https://github.com/cpichard/usdtweak/issues");
        ImGui::Text("or by mail: cpichard.github@gmail.com");
        ImGui::Text("");
        ImGui::Text("usdtweak - Copyright (c) 2016-2023 Cyril Pichard - Apache License 2.0");
        ImGui::Text("");
        ImGui::Text("   Copyright (c) 2016-2023 Pixar - Modified Apache 2.0 License");
        ImGui::Text("");
        ImGui::Text("IMGUI - https://github.com/ocornut/imgui");
        ImGui::Text("   Copyright (c) 2014-2023 Omar Cornut - The MIT License (MIT)");
        ImGui::Text("");
        ImGui::Text("GLFW - https://www.glfw.org/");
        ImGui::Text("   Copyright © 2002-2006 Marcus Geelnard - The zlib/libpng License ");
        ImGui::Text("   Copyright © 2006-2019 Camilla Löwy - The zlib/libpng License ");
        ImGui::Text("");
        if (ImGui::Button("  Close  ")) {
            close_modal();
        }
    }
    [[nodiscard]] const char *dialog_id() const override { return "About Usdtweak"; }
    Editor &editor;
};

struct CloseEditorModalDialog : public ModalDialog {
    CloseEditorModalDialog(Editor &editor, std::string confirmReasons) : editor(editor), confirmReasons(std::move(confirmReasons)) {}

    void draw() override {
        ImGui::Text("%s", confirmReasons.c_str());
        ImGui::Text("Close anyway ?");
        if (ImGui::Button("  No  ")) {
            close_modal();
        }
        ImGui::SameLine();
        if (ImGui::Button("  Yes  ")) {
            close_modal();
            editor.shutdown();
        }
    }
    [[nodiscard]] const char *dialog_id() const override { return "Closing Usdtweak"; }
    Editor &editor;
    std::string confirmReasons;
};

void Editor::request_shutdown() const {
    if (!_isShutdown) {
        execute_after_draw<EditorShutdown>();
    }
}

bool Editor::has_unsaved_work() {
    for (const auto &layer : SdfLayer::GetLoadedLayers()) {
        if (layer && layer->IsDirty() && !layer->IsAnonymous()) {
            return true;
        }
    }
    return false;
}

void Editor::confirm_shutdown(const std::string &why) {
    force_close_current_modal();
    draw_modal_dialog<CloseEditorModalDialog>(*this, why);
}

/// Modal dialog used to create a new layer
struct CreateUsdFileModalDialog : public ModalDialog {
    explicit CreateUsdFileModalDialog(Editor &editor) : editor(editor), createStage(true) { reset_file_browser_file_path(); };

    void draw() override {
        draw_file_browser();
        ensure_file_browser_default_extension("usd");
        auto filePath = get_file_browser_file_path();
        ImGui::Checkbox("Open as stage", &createStage);
        if (file_path_exists()) {
            // ... could add other messages like permission denied, or incorrect extension
            ImGui::TextColored(ImVec4(1.0f, 0.1f, 0.1f, 1.0f), "Warning: overwriting");
        } else {
            if (!filePath.empty()) {
                ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "New stage: ");
            } else {
                ImGui::TextColored(ImVec4(1.0f, 0.1f, 0.1f, 1.0f), "Empty filename");
            }
        }

        ImGui::Text("%s", filePath.c_str());
        draw_ok_cancel_modal([&]() {
            if (!filePath.empty()) {
                if (createStage) {
                    editor.create_stage(filePath);
                } else {
                    editor.create_new_layer(filePath);
                }
            }
        });
    }

    [[nodiscard]] const char *dialog_id() const override { return "Create usd file"; }
    Editor &editor;
    bool createStage = true;
};

/// Modal dialog to open a layer
struct OpenUsdFileModalDialog : public ModalDialog {
    explicit OpenUsdFileModalDialog(Editor &editor) : editor(editor) { set_valid_extensions(get_usd_valid_extensions()); };
    ~OpenUsdFileModalDialog() override = default;
    void draw() override {
        draw_file_browser();

        if (file_path_exists()) {
            ImGui::Checkbox("Open as stage", &openAsStage);
            if (openAsStage) {
                ImGui::SameLine();
                ImGui::Checkbox("Load payloads", &openLoaded);
            }
        } else {
            ImGui::Text("Not found: ");
        }
        auto filePath = get_file_browser_file_path();
        ImGui::Text("%s", filePath.c_str());
        draw_ok_cancel_modal([&]() {
            if (!filePath.empty() && file_path_exists()) {
                if (openAsStage) {
                    editor.open_stage(filePath, openLoaded);
                } else {
                    editor.find_or_open_layer(filePath);
                }
            }
        });
    }

    [[nodiscard]] const char *dialog_id() const override { return "Open layer"; }
    Editor &editor;
    bool openAsStage = true;
    bool openLoaded = true;
};

struct SaveLayerAsDialog : public ModalDialog {

    SaveLayerAsDialog(Editor &editor, SdfLayerRefPtr layer) : editor(editor), _layer(std::move(layer)){};
    ~SaveLayerAsDialog() override = default;
    void draw() override {
        draw_file_browser();
        ensure_file_browser_default_extension("usd");
        if (file_path_exists()) {
            ImGui::TextColored(ImVec4(1.0f, 0.1f, 0.1f, 1.0f), "Overwrite: ");
        } else {
            ImGui::Text("Save to: ");
        }
        auto filePath = get_file_browser_file_path();
        ImGui::Text("%s", filePath.c_str());
        draw_ok_cancel_modal([&]() {// On Ok ->
            if (!filePath.empty()) {
                editor.save_layer_as(_layer, filePath);
            }
        });
    }

    [[nodiscard]] const char *dialog_id() const override { return "Save layer as"; }
    Editor &editor;
    SdfLayerRefPtr _layer;
};

struct ExportStageDialog : public ModalDialog {
    typedef enum { ExportUSDZ = 0,
                   ExportArKit,
                   ExportFlatten } ExportType;
    ExportStageDialog(Editor &editor, ExportType exportType) : editor(editor), _exportType(exportType) {
        switch (_exportType) {
            case ExportUSDZ:
                _exportTypeStr = "Export Compressed USD (usdz)";
                _defaultExtension = "usdz";
                break;
            case ExportArKit:
                _exportTypeStr = "Export ArKit (usdz)";
                _defaultExtension = "usdz";
                break;
            case ExportFlatten:
                _exportTypeStr = "Export Flattened USD (usd)";
                _defaultExtension = "usd";
                break;
        }
    };
    ~ExportStageDialog() override = default;

    void draw() override {
        draw_file_browser();
        switch (_exportType) {
            case ExportUSDZ:// falls through
            case ExportArKit:
                ensure_file_browser_extension(_defaultExtension);
                break;
            case ExportFlatten:
                ensure_file_browser_default_extension(_defaultExtension);
                break;
        }
        if (file_path_exists()) {
            ImGui::TextColored(ImVec4(1.0f, 0.1f, 0.1f, 1.0f), "Overwrite: ");
        } else {
            ImGui::Text("Export to: ");
        }
        auto filePath = get_file_browser_file_path();
        ImGui::Text("%s", filePath.c_str());
        draw_ok_cancel_modal([&]() {// On Ok ->
            if (!filePath.empty()) {
                switch (_exportType) {
                    case ExportUSDZ:
                        execute_after_draw<EditorExportUsdz>(filePath, false);
                        break;
                    case ExportArKit:
                        execute_after_draw<EditorExportUsdz>(filePath, true);
                        break;
                    case ExportFlatten:
                        execute_after_draw<EditorExportFlattenedStage>(filePath);
                        break;
                }
            }
        });
    }

    [[nodiscard]] const char *dialog_id() const override { return _exportTypeStr.c_str(); }
    Editor &editor;
    ExportType _exportType;
    std::string _exportTypeStr;
    std::string _defaultExtension;
};

static void begin_backgound_dock() {
    // Setup dockspace using experimental imgui branch
    static bool alwaysOpened = true;
    static ImGuiDockNodeFlags dockFlags = ImGuiDockNodeFlags_None;
    static ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDocking;
    windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    ImGuiViewport *viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace", &alwaysOpened, windowFlags);
    ImGui::PopStyleVar(3);

    ImGuiID dockspaceid = ImGui::GetID("dockspace");
    ImGui::DockSpace(dockspaceid, ImVec2(0.0f, 0.0f), dockFlags);
}

static void end_background_dock() {
    ImGui::End();
}

/// Call back for dropping a file in the ui
/// TODO Drop callback should popup a modal dialog with the different options available
void Editor::drop_callback(GLFWwindow *window, int count, const char **paths) {
    void *userPointer = glfwGetWindowUserPointer(window);
    if (userPointer) {
        auto *editor = static_cast<Editor *>(userPointer);
        // TODO: Create a task, add a callback
        if (editor && count) {
            for (int i = 0; i < count; ++i) {
                // make a drop event ?
                if (ArchGetFileLength(paths[i]) == 0) {
                    // if the file is empty, this is considered a new file
                    editor->create_stage(std::string(paths[i]));
                } else {
                    editor->find_or_open_layer(std::string(paths[i]));
                }
            }
        }
    }
}

void Editor::window_close_callback(GLFWwindow *window) {
    void *userPointer = glfwGetWindowUserPointer(window);
    if (userPointer) {
        auto *editor = static_cast<Editor *>(userPointer);
        editor->request_shutdown();
    }
}

void Editor::window_size_callback(GLFWwindow *window, int width, int height) {
    void *userPointer = glfwGetWindowUserPointer(window);
    if (userPointer) {
        auto *editor = static_cast<Editor *>(userPointer);
        editor->_settings._mainWindowWidth = width;
        editor->_settings._mainWindowHeight = height;
    }
}

Editor::Editor()
    : _viewport(UsdStageRefPtr(), _selection),
      _layerHistoryPointer(0),
      _settings(500, 500) {
    // todo
    execute_after_draw<EditorSetDataPointer>(this);// This is specialized to execute here, not after the draw
    load_settings();
    set_file_browser_directory(_settings._lastFileBrowserDirectory);
    Blueprints::get_instance().set_blueprints_locations(_settings._blueprintLocations);
}

Editor::~Editor() {
    _settings._lastFileBrowserDirectory = get_file_browser_directory();
    save_settings();
}

void Editor::install_callbacks(GLFWwindow *window) {
    // Install glfw callbacks
    glfwSetWindowUserPointer(window, this);
    glfwSetDropCallback(window, Editor::drop_callback);
    glfwSetWindowCloseCallback(window, Editor::window_close_callback);
    glfwSetWindowSizeCallback(window, Editor::window_size_callback);
}

void Editor::remove_callbacks(GLFWwindow *window) { glfwSetWindowUserPointer(window, nullptr); }

void Editor::set_current_stage(UsdStageCache::Id current) {
    set_current_stage(get_stage_cache().Find(current));
}

void Editor::set_current_stage(const UsdStageRefPtr &stage) {
    if (_currentStage != stage) {
        _currentStage = stage;
        // NOTE: We set the default layer to the current stage root
        // this might have side effects
        if (_currentStage) {
            set_current_layer(_currentStage->GetRootLayer());
        }
        // TODO multiple viewport management
        _viewport.set_current_stage(stage);
    }
}

void Editor::set_current_layer(const SdfLayerRefPtr &layer, bool showContentBrowser) {
    if (!layer)
        return;
    if (!_layerHistory.empty()) {
        if (get_current_layer() != layer) {
            if (_layerHistoryPointer < _layerHistory.size() - 1) {
                _layerHistory.resize(_layerHistoryPointer + 1);
            }
            _layerHistory.push_back(layer);
            _layerHistoryPointer = _layerHistory.size() - 1;
        }
    } else {
        _layerHistory.push_back(layer);
        _layerHistoryPointer = _layerHistory.size() - 1;
    }
    if (showContentBrowser) {
        _settings._showContentBrowser = true;
    }
}

void Editor::set_current_edit_target(const SdfLayerHandle &layer) {
    if (get_current_stage()) {
        get_current_stage()->SetEditTarget(UsdEditTarget(layer));
    }
}

SdfLayerRefPtr Editor::get_current_layer() {
    return _layerHistory.empty() ? SdfLayerRefPtr() : _layerHistory[_layerHistoryPointer];
}

void Editor::set_previous_layer() {
    if (_layerHistoryPointer > 0) {
        _layerHistoryPointer--;
    }
}

void Editor::set_next_layer() {
    if (_layerHistoryPointer < _layerHistory.size() - 1) {
        _layerHistoryPointer++;
    }
}

void Editor::create_new_layer(const std::string &path) {
    auto newLayer = SdfLayer::CreateNew(path);
    set_current_layer(newLayer, true);
}

void Editor::find_or_open_layer(const std::string &path) {
    auto newLayer = SdfLayer::FindOrOpen(path);
    set_current_layer(newLayer, true);
}

//
void Editor::open_stage(const std::string &path, bool openLoaded) {
    auto newStage = UsdStage::Open(path, openLoaded ? UsdStage::LoadAll : UsdStage::LoadNone);// TODO: as an option
    if (newStage) {
        get_stage_cache().Insert(newStage);
        set_current_stage(newStage);
        _settings._showContentBrowser = true;
        _settings._showViewport = true;
        _settings.update_recent_files(path);
    }
}

void Editor::save_layer_as(const SdfLayerRefPtr &layer, const std::string &path) {
    if (!layer) return;
    auto newLayer = SdfLayer::CreateNew(path);
    if (!newLayer) {
        newLayer = SdfLayer::FindOrOpen(path);
    }
    if (newLayer) {
        newLayer->TransferContent(layer);
        newLayer->Save();
        set_current_layer(newLayer, true);
    }
}

void Editor::create_stage(const std::string &path) {
    auto usdaFormat = SdfFileFormat::FindByExtension("usda");
    auto layer = SdfLayer::New(usdaFormat, path);
    if (layer) {
        auto newStage = UsdStage::Open(layer);
        if (newStage) {
            get_stage_cache().Insert(newStage);
            set_current_stage(newStage);
            _settings._showContentBrowser = true;
            _settings._showViewport = true;
        }
    }
}

Viewport &Editor::get_viewport() {
    return _viewport;
}

void Editor::hydra_render() {
    _viewport.update();
    _viewport.render();
}

void Editor::show_dialog_save_layer_as(const SdfLayerHandle &layerToSaveAs) { draw_modal_dialog<SaveLayerAsDialog>(*this, layerToSaveAs); }

void Editor::add_layer_path_selection(const SdfPath &primPath) {
    _selection.add_selected(get_current_layer(), primPath);
    BringWindowToTabFront(SdfPrimPropertiesWindowTitle);
}

void Editor::set_layer_path_selection(const SdfPath &primPath) {
    _selection.set_selected(get_current_layer(), primPath);
    BringWindowToTabFront(SdfPrimPropertiesWindowTitle);
}

void Editor::add_stage_path_selection(const SdfPath &primPath) {
    _selection.add_selected(get_current_stage(), primPath);
    BringWindowToTabFront(UsdPrimPropertiesWindowTitle);
}

void Editor::set_stage_path_selection(const SdfPath &primPath) {
    _selection.set_selected(get_current_stage(), primPath);
    BringWindowToTabFront(UsdPrimPropertiesWindowTitle);
}

void Editor::draw_main_menu_bar() {
    //ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 8));
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem(ICON_FA_FILE " New")) {
                draw_modal_dialog<CreateUsdFileModalDialog>(*this);
            }
            if (ImGui::MenuItem(ICON_FA_FOLDER_OPEN " Open")) {
                draw_modal_dialog<OpenUsdFileModalDialog>(*this);
            }
            if (ImGui::BeginMenu(ICON_FA_FOLDER_OPEN " Open Recent (as stage)")) {
                for (const auto &recentFile : _settings.get_recent_files()) {
                    if (ImGui::MenuItem(recentFile.c_str())) {
                        execute_after_draw<EditorOpenStage>(recentFile);
                    }
                }
                ImGui::EndMenu();
            }
            ImGui::Separator();
            const bool hasLayer = get_current_layer() != SdfLayerRefPtr();
            if (ImGui::MenuItem(ICON_FA_SAVE " Save layer", "CTRL+S", false, hasLayer)) {
                get_current_layer()->Save(true);
            }
            if (ImGui::MenuItem(ICON_FA_SAVE " Save current layer as", "CTRL+F", false, hasLayer)) {
                execute_after_draw<EditorSaveLayerAs>(get_current_layer());
            }
            const bool hasCurrentStage = get_current_stage();
            if (ImGui::BeginMenu(ICON_FA_SHARE " Export Stage", hasCurrentStage)) {
                if (ImGui::MenuItem("Compressed package (usdz)")) {
                    if (get_current_stage()) {
                        draw_modal_dialog<ExportStageDialog>(*this, ExportStageDialog::ExportUSDZ);
                    }
                }
                if (ImGui::MenuItem("Arkit package (usdz)")) {
                    if (get_current_stage()) {
                        draw_modal_dialog<ExportStageDialog>(*this, ExportStageDialog::ExportArKit);
                    }
                }
                if (ImGui::MenuItem("Flattened stage (usd)")) {
                    if (get_current_stage()) {
                        draw_modal_dialog<ExportStageDialog>(*this, ExportStageDialog::ExportFlatten);
                    }
                }
                ImGui::EndMenu();
            }

            ImGui::Separator();
            if (ImGui::MenuItem("Quit")) {
                request_shutdown();
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit")) {
            if (ImGui::MenuItem("Undo", "CTRL+Z")) {
                execute_after_draw<UndoCommand>();
            }
            if (ImGui::MenuItem("Redo", "CTRL+R")) {
                execute_after_draw<RedoCommand>();
            }
            if (ImGui::MenuItem("Clear Undo/Redo")) {
                execute_after_draw<ClearUndoRedoCommand>();
            }
            if (ImGui::MenuItem("Clear History")) {
                _layerHistory.clear();
                _layerHistoryPointer = 0;
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Cut", "CTRL+X", false, false)) {
            }
            if (ImGui::MenuItem("Copy", "CTRL+C", false, false)) {
            }
            if (ImGui::MenuItem("Paste", "CTRL+V", false, false)) {
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Tools")) {
            // TODO: we should really check if storm is available
            if (ImGui::MenuItem(ICON_FA_IMAGES " Storm playblast")) {
                if (get_current_stage()) {
                    draw_modal_dialog<PlayblastModalDialog>(get_current_stage());
                }
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Windows")) {
            ImGui::MenuItem(DebugWindowTitle, nullptr, &_settings._showDebugWindow);
            ImGui::MenuItem(ContentBrowserWindowTitle, nullptr, &_settings._showContentBrowser);
            ImGui::MenuItem(UsdStageHierarchyWindowTitle, nullptr, &_settings._showOutliner);
            ImGui::MenuItem(UsdPrimPropertiesWindowTitle, nullptr, &_settings._showPropertyEditor);
            ImGui::MenuItem(SdfLayerHierarchyWindowTitle, nullptr, &_settings._showLayerHierarchyEditor);
            ImGui::MenuItem(SdfLayerStackWindowTitle, nullptr, &_settings._showLayerStackEditor);
            ImGui::MenuItem(SdfPrimPropertiesWindowTitle, nullptr, &_settings._showPrimSpecEditor);
            ImGui::MenuItem(SdfLayerAsciiEditorWindowTitle, nullptr, &_settings._textEditor);
            ImGui::MenuItem(SdfAttributeWindowTitle, nullptr, &_settings._showSdfAttributeEditor);
            ImGui::MenuItem(TimelineWindowTitle, nullptr, &_settings._showTimeline);
            ImGui::MenuItem(ViewportWindowTitle, nullptr, &_settings._showViewport);
            ImGui::MenuItem(StatusBarWindowTitle, nullptr, &_settings._showStatusBar);
            ImGui::MenuItem(LauncherBarWindowTitle, nullptr, &_settings._showLauncherBar);
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Help")) {
            if (ImGui::MenuItem("About")) {
                draw_modal_dialog<AboutModalDialog>(*this);
            }
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

void Editor::draw() {
    // Main Menu bar
    draw_main_menu_bar();

    // Dock
    begin_backgound_dock();
    const auto &rootLayer = get_current_layer();
    const ImGuiWindowFlags layerWindowFlag = (rootLayer && rootLayer->IsDirty()) ? ImGuiWindowFlags_UnsavedDocument : ImGuiWindowFlags_None;

    if (_settings._showViewport) {
        TRACE_SCOPE(ViewportWindowTitle)
        ImGui::Begin(ViewportWindowTitle, &_settings._showViewport);
        get_viewport().draw();
        ImGui::End();
    }

    if (_settings._showDebugWindow) {
        TRACE_SCOPE(DebugWindowTitle)
        ImGui::Begin(DebugWindowTitle, &_settings._showDebugWindow);
        draw_debug_ui();
        ImGui::End();
    }
    if (_settings._showStatusBar) {
        ImGuiWindowFlags statusFlags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar;
        if (ImGui::BeginViewportSideBar("##StatusBar", nullptr, ImGuiDir_Down, ImGui::GetFrameHeight(), statusFlags)) {
            if (ImGui::BeginMenuBar()) {// Drawing only the framerate
                ImGui::Text("\xee\x81\x99"
                            " %.3f ms/frame  (%.1f FPS)",
                            1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
                ImGui::EndMenuBar();
            }
        }
        ImGui::End();
    }

    if (_settings._showLauncherBar) {
        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_None;
        ImGui::Begin(LauncherBarWindowTitle, &_settings._showLauncherBar, windowFlags);
        draw_launcher_bar(this);
        ImGui::End();
    }

    if (_settings._showPropertyEditor) {
        TRACE_SCOPE(UsdPrimPropertiesWindowTitle)
        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_None;
        // WIP windowFlags |= ImGuiWindowFlags_MenuBar;
        ImGui::Begin(UsdPrimPropertiesWindowTitle, &_settings._showPropertyEditor, windowFlags);
        if (get_current_stage()) {
            auto prim = get_current_stage()->GetPrimAtPath(_selection.get_anchor_prim_path(get_current_stage()));
            draw_usd_prim_properties(prim, get_viewport().get_current_time_code());
        }
        ImGui::End();
    }

    if (_settings._showOutliner) {
        const ImGuiWindowFlags windowFlagsWithMenu = ImGuiWindowFlags_None | ImGuiWindowFlags_MenuBar;
        TRACE_SCOPE(UsdStageHierarchyWindowTitle)
        ImGui::Begin(UsdStageHierarchyWindowTitle, &_settings._showOutliner, windowFlagsWithMenu);
        draw_stage_outliner(get_current_stage(), _selection);
        ImGui::End();
    }

    if (_settings._showTimeline) {
        TRACE_SCOPE(TimelineWindowTitle)
        ImGui::Begin(TimelineWindowTitle, &_settings._showTimeline);
        UsdTimeCode tc = get_viewport().get_current_time_code();
        draw_timeline(get_current_stage(), tc);
        get_viewport().set_current_time_code(tc);
        ImGui::End();
    }

    if (_settings._showLayerHierarchyEditor) {
        TRACE_SCOPE(SdfLayerHierarchyWindowTitle)
        const std::string title(SdfLayerHierarchyWindowTitle + (rootLayer ? " - " + rootLayer->GetDisplayName() : "") +
                                "###Layer hierarchy");
        ImGui::Begin(title.c_str(), &_settings._showLayerHierarchyEditor, layerWindowFlag);
        draw_layer_prim_hierarchy(rootLayer, get_selection());
        ImGui::End();
    }

    if (_settings._showLayerStackEditor) {
        TRACE_SCOPE(SdfLayerStackWindowTitle)
        const std::string title(SdfLayerStackWindowTitle "###Layer stack");
        ImGui::Begin(title.c_str(), &_settings._showLayerStackEditor, layerWindowFlag);
        //DrawLayerSublayerStack(rootLayer);
        draw_stage_layer_editor(get_current_stage());
        ImGui::End();
    }

    if (_settings._showContentBrowser) {
        TRACE_SCOPE(ContentBrowserWindowTitle)
        const ImGuiWindowFlags windowFlags = ImGuiWindowFlags_None | ImGuiWindowFlags_MenuBar;
        ImGui::Begin(ContentBrowserWindowTitle, &_settings._showContentBrowser, windowFlags);
        draw_content_browser(*this);
        ImGui::End();
    }

    if (_settings._showPrimSpecEditor) {
        const ImGuiWindowFlags windowFlagsWithMenu = ImGuiWindowFlags_None | ImGuiWindowFlags_MenuBar;
        TRACE_SCOPE(SdfPrimPropertiesWindowTitle)
        ImGui::Begin(SdfPrimPropertiesWindowTitle, &_settings._showPrimSpecEditor, windowFlagsWithMenu);
        const SdfPath &primPath = _selection.get_anchor_prim_path(get_current_layer());
        // Ideally this condition should be moved in a function like DrawLayerProperties()
        if (primPath != SdfPath() && primPath != SdfPath::AbsoluteRootPath()) {
            auto selectedPrimSpec = get_current_layer()->GetPrimAtPath(primPath);
            draw_sdf_prim_editor_menu_bar(selectedPrimSpec);
            draw_sdf_prim_editor(selectedPrimSpec, get_selection());
        } else {
            auto headerSize = ImGui::GetWindowSize();
            headerSize.y = TableRowDefaultHeight * 3;// 3 fields in the header
            headerSize.x = -FLT_MIN;
            draw_sdf_layer_editor_menu_bar(get_current_layer());// TODO: write a menu for layer
            ImGui::BeginChild("##LayerHeader", headerSize);
            draw_sdf_layer_identity(get_current_layer(), SdfPath::AbsoluteRootPath());
            ImGui::EndChild();
            ImGui::Separator();
            ImGui::BeginChild("##LayerBody");
            draw_layer_sublayer_stack(get_current_layer());
            draw_sdf_layer_metadata(get_current_layer());

            ImGui::EndChild();
        }

        ImGui::End();
    }

#if 0// experimental connection editor is disabled
    ImGui::Begin("Connection editor");
    if (GetCurrentStage()) {
        auto prim = GetCurrentStage()->GetPrimAtPath(_selection.GetAnchorPrimPath(GetCurrentStage()));
        DrawConnectionEditor(prim);
    }
    ImGui::End();
#endif

    if (_settings._textEditor) {
        TRACE_SCOPE(SdfLayerAsciiEditorWindowTitle)
        ImGui::Begin(SdfLayerAsciiEditorWindowTitle, &_settings._textEditor);
        draw_text_editor(get_current_layer());
        ImGui::End();
    }

    if (_settings._showSdfAttributeEditor) {
        TRACE_SCOPE(SdfAttributeWindowTitle)
        ImGui::Begin(SdfAttributeWindowTitle, &_settings._showSdfAttributeEditor);
        draw_sdf_attribute_editor(get_current_layer(), get_selection());
        ImGui::End();
    }

    draw_current_modal();

    ///////////////////////
    // Top level shortcuts functions
    add_shortcut<UndoCommand, ImGuiKey_LeftCtrl, ImGuiKey_Z>();
    add_shortcut<RedoCommand, ImGuiKey_LeftCtrl, ImGuiKey_R>();
    end_background_dock();
}

void Editor::run_launcher(const std::string &launcherName) {
    std::string commandLine = _settings.get_launcher_command_line(launcherName);
    if (commandLine.empty())
        return;
    // Process the command line
    auto pos = commandLine.find("__STAGE_PATH__");
    if (pos != std::string::npos) {
        commandLine.replace(pos, 14, get_current_stage() ? get_current_stage()->GetRootLayer()->GetRealPath() : "");
    }

    pos = commandLine.find("__LAYER_PATH__");
    if (pos != std::string::npos) {
        commandLine.replace(pos, 14, get_current_layer() ? get_current_layer()->GetRealPath() : "");
    }

    pos = commandLine.find("__CURRENT_TIME__");
    if (pos != std::string::npos) {
        auto timeCode = get_viewport().get_current_time_code();
        if (!timeCode.IsDefault()) {
            commandLine.replace(pos, 16, std::to_string(timeCode.GetValue()));
        }
    }

    auto command = [commandLine]() -> int { return std::system(commandLine.c_str()); };
    // TODO: we are just storing the tasks in a vector, we shoud do some
    // cleaning when the tasks are done
    _launcherTasks.emplace_back(std::async(std::launch::async, command));
}

void Editor::load_settings() {
    _settings = ResourcesLoader::GetEditorSettings();
}

void Editor::save_settings() const {
    ResourcesLoader::GetEditorSettings() = _settings;
}

}// namespace vox