//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <iostream>

#include <pxr/imaging/garch/glApi.h>
#include <pxr/usd/usd/primRange.h>
#include <pxr/usd/usdGeom/boundable.h>
#include <pxr/usd/usdGeom/camera.h>
#include <pxr/usd/usdGeom/metrics.h>
#include <pxr/usd/usdGeom/bboxCache.h>
#include <pxr/usd/usdGeom/imageable.h>
#include <pxr/usd/usdUtils/stageCache.h>

#include "base/imgui_helpers.h"
#include "viewport.h"
#include "commands/commands.h"
#include "base/constants.h"
#include "commands/shortcuts.h"
#include "widgets/usd_prim_editor.h"// DrawUsdPrimEditTarget

namespace clk = std::chrono;

// TODO: picking meshes: https://groups.google.com/g/usd-interest/c/P2CynIu7MYY/m/UNPIKzmMBwAJ

namespace vox {
// The camera path will move once we create a CameraList class in charge of
// camera selection per stage
static SdfPath perspectiveCameraPath("/usdtweak/cameras/cameraPerspective");

/// Draw a camera selection
void draw_camera_list(Viewport &viewport) {
    ScopedStyleColor defaultStyle(DefaultColorStyle);
    // TODO: the viewport cameras and the stage camera should live in different lists
    constexpr char const *perspectiveCameraName = "Perspective";
    if (ImGui::BeginListBox("##CameraList")) {
        // OpenGL Cameras
        if (ImGui::Selectable(perspectiveCameraName, viewport.get_camera_path() == perspectiveCameraPath)) {
            viewport.set_camera_path(perspectiveCameraPath);
        }
        if (viewport.get_current_stage()) {
            UsdPrimRange range = viewport.get_current_stage()->Traverse();
            for (const auto &prim : range) {
                if (prim.IsA<UsdGeomCamera>()) {
                    ImGui::PushID(prim.GetPath().GetString().c_str());
                    const bool isSelected = (prim.GetPath() == viewport.get_camera_path());
                    if (ImGui::Selectable(prim.GetName().data(), isSelected)) {
                        viewport.set_camera_path(prim.GetPath());
                    }
                    if (ImGui::IsItemHovered() && GImGui->HoveredIdTimer > 2) {
                        ImGui::SetTooltip("%s", prim.GetPath().GetString().c_str());
                    }
                    ImGui::PopID();
                }
            }
        }
        ImGui::EndListBox();
    }
}

static void DrawViewportCameraEditor(Viewport &viewport) {
    GfCamera &camera = viewport.get_current_camera();
    float focal = camera.GetFocalLength();
    ImGui::InputFloat("Focal length", &focal);
    if (ImGui::IsItemDeactivatedAfterEdit()) {
        camera.SetFocalLength(focal);
    }
    GfRange1f clippingRange = camera.GetClippingRange();
    ImGui::InputFloat2("Clipping range", (float *)&clippingRange);
    if (ImGui::IsItemDeactivatedAfterEdit()) {
        camera.SetClippingRange(clippingRange);
    }

    if (ImGui::Button("Duplicate camera")) {
        UsdStageRefPtr stage = viewport.get_current_stage();
        // Find the next camera path
        std::string cameraPath = UsdGeomCameraDefaultPrefix;
        for (int cameraNumber = 1; stage->GetPrimAtPath(SdfPath(cameraPath)); cameraNumber++) {
            cameraPath = std::string(UsdGeomCameraDefaultPrefix) + std::to_string(cameraNumber);
        }
        if (stage) {
            // It's not worth creating a command, just use a function
            std::function<void()> duplicateCamera = [camera, cameraPath, stage]() {
                UsdGeomCamera newGeomCamera = UsdGeomCamera::Define(stage, SdfPath(cameraPath));
                newGeomCamera.SetFromCamera(camera, UsdTimeCode::Default());
            };
            execute_after_draw<UsdFunctionCall>(viewport.get_current_stage(), duplicateCamera);
        }
    }
}

static void DrawUsdGeomCameraEditor(const UsdGeomCamera &usdGeomCamera, UsdTimeCode keyframeTimeCode) {
    auto camera = usdGeomCamera.GetCamera(keyframeTimeCode);
    float focal = camera.GetFocalLength();
    ImGui::InputFloat("Focal length", &focal);
    if (ImGui::IsItemDeactivatedAfterEdit()) {
        auto attr = usdGeomCamera.GetFocalLengthAttr();// this is not ideal as
        VtValue value(focal);
        execute_after_draw<AttributeSet>(attr, value, keyframeTimeCode);
    }
    GfRange1f clippingRange = camera.GetClippingRange();
    ImGui::InputFloat2("Clipping range", (float *)&clippingRange);
    if (ImGui::IsItemDeactivatedAfterEdit()) {
        auto attr = usdGeomCamera.GetClippingRangeAttr();
        VtValue value(GfVec2f(clippingRange.GetMin(), clippingRange.GetMax()));
        execute_after_draw<AttributeSet>(attr, value, keyframeTimeCode);
    }

    if (ImGui::Button("Duplicate selected camera")) {
        // TODO: We probably want to duplicate this camera prim using the same parent
        // as the movement of the camera can be set on the parents
    }
}

void draw_camera_editor(Viewport &viewport) {
    ScopedStyleColor defaultStyle(DefaultColorStyle);
    // 2 cases:
    //   1) the camera selected is part of the scene
    //   2) the camera is managed by the viewport and does not belong to the scene
    auto usdGeomCamera = viewport.get_usd_geom_camera();
    if (usdGeomCamera) {
        DrawUsdGeomCameraEditor(usdGeomCamera, viewport.get_current_time_code());
    } else {
        DrawViewportCameraEditor(viewport);
    }
}

Viewport::Viewport(UsdStageRefPtr stage, Selection &selection)
    : _stage(stage), _cameraManipulator({InitialWindowWidth, InitialWindowHeight}),
      _currentEditingState(new MouseHoverManipulator()), _activeManipulator(&_positionManipulator), _selection(selection),
      _textureSize(1, 1), _selectedCameraPath(perspectiveCameraPath), _renderCamera(&_perspectiveCamera) {

    // Viewport draw target
    _cameraManipulator.reset_position(get_current_camera());

    _drawTarget = GlfDrawTarget::New(_textureSize, false);
    _drawTarget->Bind();
    _drawTarget->AddAttachment("color", GL_RGBA, GL_FLOAT, GL_RGBA);
    _drawTarget->AddAttachment("depth", GL_DEPTH_COMPONENT, GL_FLOAT, GL_DEPTH_COMPONENT32F);
    auto color = _drawTarget->GetAttachment("color");
    _textureId = color->GetGlTextureName();
    _drawTarget->Unbind();
}

Viewport::~Viewport() {
    if (_renderer) {
        _renderer = nullptr;// will be deleted in the map
    }
    // Delete renderers
    _drawTarget->Bind();
    for (auto &renderer : _renderers) {
        // Warning, InvalidateBuffers might be defered ... :S to check
        // removed in 20.11: renderer.second->InvalidateBuffers();
        if (renderer.second) {
            delete renderer.second;
            renderer.second = nullptr;
        }
    }
    _drawTarget->Unbind();
    _renderers.clear();
}

static void draw_opened_stages() {
    ScopedStyleColor defaultStyle(DefaultColorStyle);
    const UsdStageCache &stageCache = UsdUtilsStageCache::Get();
    const auto allStages = stageCache.GetAllStages();
    for (const auto &stagePtr : allStages) {
        if (ImGui::MenuItem(stagePtr->GetRootLayer()->GetIdentifier().c_str())) {
            execute_after_draw<EditorSetCurrentStage>(stagePtr->GetRootLayer());
        }
    }
}

/// Draw the viewport widget
void Viewport::draw() {
    const ImVec2 wsize = ImGui::GetWindowSize();

    // Set the size of the texture here as we need the current window size
    const auto cursorPos = ImGui::GetCursorPos();
    _textureSize = GfVec2i(std::max(1.f, wsize[0] - 2 * GImGui->Style.ItemSpacing.x),
                           std::max(1.f, wsize[1] - cursorPos.y - 2 * GImGui->Style.ItemSpacing.y));

    if (_textureId) {
        // Get the size of the child (i.e. the whole draw size of the windows).
        ImGui::Image((ImTextureID)((uintptr_t)_textureId), ImVec2(_textureSize[0], _textureSize[1]), ImVec2(0, 1), ImVec2(1, 0));
        // TODO: it is possible to have a popup menu on top of the viewport.
        // It should be created depending on the manipulator/editor state
        //if (ImGui::BeginPopupContextItem()) {
        //    ImGui::Button("ColorCorrection");
        //    ImGui::Button("Deactivate");
        //    ImGui::EndPopup();
        //}
        handle_manipulation_events();
        handle_keyboard_shortcut();
        ImGui::BeginDisabled(!bool(get_current_stage()));
        draw_tool_bar(cursorPos + ImVec2(120, 40));
        draw_manipulator_toolbox(cursorPos + ImVec2(15, 40));
        draw_stage_selector(cursorPos + ImVec2(15, 15));
        ImGui::EndDisabled();
    }
}

void Viewport::draw_stage_selector(const ImVec2 widgetOrigin) {
    const ImVec2 buttonSize(25, 25);// Button size
    const ImVec4 defaultColor(0.1, 0.1, 0.1, 0.7);
    const ImVec4 selectedColor(ColorButtonHighlight);
    const ImGuiStyle &style = ImGui::GetStyle();
    ImGui::SetCursorPos(widgetOrigin);
    if (!bool(get_current_stage())) {
        ImGui::Text("No stage loaded");
        return;
    }
    // Background frame
    const std::string stageName = get_current_stage() ? get_current_stage()->GetRootLayer()->GetDisplayName() : "";
    const auto stageNameTextSize = ImGui::CalcTextSize(stageName.c_str(), nullptr, false, false);
    const std::string editTargetName = get_current_stage() ? get_current_stage()->GetEditTarget().GetLayer()->GetDisplayName() : "";
    const auto editTargetNameTextSize = ImGui::CalcTextSize(editTargetName.c_str(), nullptr, false, false);

    ImVec2 frameSize(stageNameTextSize.x + editTargetNameTextSize.x + 2 * buttonSize.x + 4 * style.FramePadding.y, std::max(stageNameTextSize.y, editTargetNameTextSize.y));
    frameSize += ImVec2(2 * style.FramePadding.x, 2 * style.FramePadding.y);
    const ImVec2 frameTop = ImGui::GetCurrentWindow()->DC.CursorPos - style.FramePadding;
    const ImVec2 frameBot = frameTop + frameSize + style.FramePadding;
    ImGui::PushStyleColor(ImGuiCol_FrameBg, defaultColor);
    ImGui::RenderFrame(frameTop, frameBot, ImGui::GetColorU32(ImGuiCol_FrameBg), false, false);
    ImGui::PopStyleColor();

    // Stage selector
    ImGui::SetCursorPos(widgetOrigin);
    ImGui::SmallButton(ICON_UT_STAGE);
    if (ImGui::BeginPopupContextItem(nullptr, ImGuiPopupFlags_MouseButtonLeft)) {
        draw_opened_stages();
        ImGui::EndPopup();
    }
    ImGui::SameLine();
    ImGui::Text("%s", stageName.c_str());

    // Edit target selector
    ImGui::SameLine();
    ImGui::SmallButton(ICON_FA_PEN);
    if (get_current_stage() && ImGui::BeginPopupContextItem(nullptr, ImGuiPopupFlags_MouseButtonLeft)) {
        const UsdPrim &selected = get_selection().is_selection_empty(get_current_stage()) ?
                                      get_current_stage()->GetPseudoRoot() :
                                      get_current_stage()->GetPrimAtPath(get_selection().get_anchor_prim_path(get_current_stage()));
        DrawUsdPrimEditTarget(selected);
        ImGui::EndPopup();
    }
    ImGui::SameLine();
    ImGui::Text("%s", editTargetName.c_str());
}

void Viewport::draw_tool_bar(const ImVec2 widgetPosition) {
    const ImVec2 buttonSize(25, 25);// Button size
    const ImVec4 defaultColor(0.1, 0.1, 0.1, 0.7);
    const ImVec4 selectedColor(ColorButtonHighlight);

    ImGui::SetCursorPos(widgetPosition);
    ImGui::PushStyleColor(ImGuiCol_Button, defaultColor);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, defaultColor);
    ImGui::Button(ICON_FA_CAMERA);
    ImGuiPopupFlags flags = ImGuiPopupFlags_MouseButtonLeft;
    if (_renderer && ImGui::BeginPopupContextItem(nullptr, flags)) {
        draw_camera_list(*this);
        draw_camera_editor(*this);
        ImGui::EndPopup();
    }
    if (ImGui::IsItemHovered() && GImGui->HoveredIdTimer > 1) {
        ImGui::SetTooltip("Cameras");
    }
    ImGui::SameLine();
    ImGui::Button(ICON_FA_USER_COG);
    if (_renderer && ImGui::BeginPopupContextItem(nullptr, flags)) {
        draw_renderer_controls(*_renderer);
        draw_renderer_selection_combo(*_renderer);
        draw_color_correction(*_renderer, _imagingSettings);
        draw_aov_settings(*_renderer);
        draw_renderer_commands(*_renderer);
        if (ImGui::BeginMenu("Renderer Settings")) {
            draw_renderer_settings(*_renderer, _imagingSettings);
            ImGui::EndMenu();
        }
        ImGui::EndPopup();
    }
    if (ImGui::IsItemHovered() && GImGui->HoveredIdTimer > 1) {
        ImGui::SetTooltip("Renderer settings");
    }
    ImGui::SameLine();
    ImGui::Button(ICON_FA_TV);
    if (_renderer && ImGui::BeginPopupContextItem(nullptr, flags)) {
        draw_imaging_settings(*_renderer, _imagingSettings);
        ImGui::EndPopup();
    }
    if (ImGui::IsItemHovered() && GImGui->HoveredIdTimer > 1) {
        ImGui::SetTooltip("Viewport settings");
    }
    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Button, _imagingSettings.enableCameraLight ? selectedColor : defaultColor);
    if (ImGui::Button(ICON_FA_FIRE)) {
        _imagingSettings.enableCameraLight = !_imagingSettings.enableCameraLight;
    }
    if (ImGui::IsItemHovered() && GImGui->HoveredIdTimer > 1) {
        ImGui::SetTooltip("Enable camera light");
    }
    ImGui::PopStyleColor();
    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Button, _imagingSettings.enableSceneMaterials ? selectedColor : defaultColor);
    if (ImGui::Button(ICON_FA_HAND_SPARKLES)) {
        _imagingSettings.enableSceneMaterials = !_imagingSettings.enableSceneMaterials;
    }
    if (ImGui::IsItemHovered() && GImGui->HoveredIdTimer > 1) {
        ImGui::SetTooltip("Enable scene materials");
    }
    ImGui::PopStyleColor();
    if (_renderer && _renderer->GetRendererPlugins().size() >= 2) {
        ImGui::SameLine();
        ImGui::Button(_renderer->GetRendererDisplayName(_renderer->GetCurrentRendererId()).c_str());
        if (ImGui::IsItemHovered() && GImGui->HoveredIdTimer > 1) {
            ImGui::SetTooltip("Render delegate");
        }
        if (ImGui::BeginPopupContextItem(nullptr, flags)) {
            draw_renderer_selection_list(*_renderer);
            ImGui::EndPopup();
        }
    }
    ImGui::PopStyleColor(2);
}

// Poor man manipulator toolbox
void Viewport::draw_manipulator_toolbox(const ImVec2 widgetPosition) {
    const ImVec2 buttonSize(25, 25);// Button size
    const ImVec4 defaultColor(0.1, 0.1, 0.1, 0.9);
    const ImVec4 selectedColor(ColorButtonHighlight);
    ImGui::SetCursorPos(widgetPosition);
    ImGui::SetNextItemWidth(80);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, defaultColor);
    draw_pick_mode(_selectionManipulator);
    ImGui::PopStyleColor();

    ImGui::PushStyleColor(ImGuiCol_Button, is_chosen_manipulator<MouseHoverManipulator>() ? selectedColor : defaultColor);
    ImGui::SetCursorPosX(widgetPosition.x);

    if (ImGui::Button(ICON_FA_LOCATION_ARROW, buttonSize)) {
        choose_manipulator<MouseHoverManipulator>();
    }
    ImGui::PopStyleColor();

    ImGui::PushStyleColor(ImGuiCol_Button, is_chosen_manipulator<PositionManipulator>() ? selectedColor : defaultColor);
    ImGui::SetCursorPosX(widgetPosition.x);
    if (ImGui::Button(ICON_FA_ARROWS_ALT, buttonSize)) {
        choose_manipulator<PositionManipulator>();
    }
    ImGui::PopStyleColor();

    ImGui::PushStyleColor(ImGuiCol_Button, is_chosen_manipulator<RotationManipulator>() ? selectedColor : defaultColor);
    ImGui::SetCursorPosX(widgetPosition.x);
    if (ImGui::Button(ICON_FA_SYNC_ALT, buttonSize)) {
        choose_manipulator<RotationManipulator>();
    }
    ImGui::PopStyleColor();

    ImGui::PushStyleColor(ImGuiCol_Button, is_chosen_manipulator<ScaleManipulator>() ? selectedColor : defaultColor);
    ImGui::SetCursorPosX(widgetPosition.x);
    if (ImGui::Button(ICON_FA_COMPRESS, buttonSize)) {
        choose_manipulator<ScaleManipulator>();
    }
    ImGui::PopStyleColor();
}

/// Frane the viewport using the bounding box of the selection
void Viewport::frame_selection(const Selection &selection) {// Camera manipulator ???
    if (get_current_stage() && !selection.is_selection_empty(get_current_stage())) {
        UsdGeomBBoxCache bboxcache(_imagingSettings.frame, UsdGeomImageable::GetOrderedPurposeTokens());
        GfBBox3d bbox;
        for (const auto &primPath : selection.get_selected_paths(get_current_stage())) {
            bbox = GfBBox3d::Combine(bboxcache.ComputeWorldBound(get_current_stage()->GetPrimAtPath(primPath)), bbox);
        }
        auto defaultPrim = get_current_stage()->GetDefaultPrim();
        _cameraManipulator.frame_bounding_box(get_current_camera(), bbox);
    }
}

/// Frame the viewport using the bounding box of the root prim
void Viewport::frame_root_prim() {
    if (get_current_stage()) {
        UsdGeomBBoxCache bboxcache(_imagingSettings.frame, UsdGeomImageable::GetOrderedPurposeTokens());
        auto defaultPrim = get_current_stage()->GetDefaultPrim();
        if (defaultPrim) {
            _cameraManipulator.frame_bounding_box(get_current_camera(), bboxcache.ComputeWorldBound(defaultPrim));
        } else {
            auto rootPrim = get_current_stage()->GetPrimAtPath(SdfPath("/"));
            _cameraManipulator.frame_bounding_box(get_current_camera(), bboxcache.ComputeWorldBound(rootPrim));
        }
    }
}

GfVec2d Viewport::get_picking_boundary_size() const {
    const GfVec2i renderSize = _drawTarget->GetSize();
    const double width = static_cast<double>(renderSize[0]);
    const double height = static_cast<double>(renderSize[1]);
    return GfVec2d(20.0 / width, 20.0 / height);
}

//
double Viewport::compute_scale_factor(const GfVec3d &objectPos, const double multiplier) const {
    double scale = 1.0;
    const auto &frustum = get_current_camera().GetFrustum();
    auto ray = frustum.ComputeRay(GfVec2d(0, 0));// camera axis
    ray.FindClosestPoint(objectPos, &scale);
    const float focalLength = get_current_camera().GetFocalLength();
    scale /= focalLength == 0 ? 1.f : focalLength;
    scale /= multiplier;
    scale *= 2;
    return scale;
}

inline bool IsModifierDown() {
    return ImGui::GetIO().KeyMods != 0;
}

void Viewport::handle_keyboard_shortcut() {
    if (ImGui::IsItemHovered()) {
        ImGuiIO &io = ImGui::GetIO();
        static bool SelectionManipulatorPressedOnce = true;
        if (ImGui::IsKeyDown(ImGuiKey_Q) && !IsModifierDown()) {
            if (SelectionManipulatorPressedOnce) {
                choose_manipulator<MouseHoverManipulator>();
                SelectionManipulatorPressedOnce = false;
            }
        } else {
            SelectionManipulatorPressedOnce = true;
        }

        static bool PositionManipulatorPressedOnce = true;
        if (ImGui::IsKeyDown(ImGuiKey_W) && !IsModifierDown()) {
            if (PositionManipulatorPressedOnce) {
                choose_manipulator<PositionManipulator>();
                PositionManipulatorPressedOnce = false;
            }
        } else {
            PositionManipulatorPressedOnce = true;
        }

        static bool RotationManipulatorPressedOnce = true;
        if (ImGui::IsKeyDown(ImGuiKey_E) && !IsModifierDown()) {
            if (RotationManipulatorPressedOnce) {
                choose_manipulator<RotationManipulator>();
                RotationManipulatorPressedOnce = false;
            }
        } else {
            RotationManipulatorPressedOnce = true;
        }

        static bool ScaleManipulatorPressedOnce = true;
        if (ImGui::IsKeyDown(ImGuiKey_R) && !IsModifierDown()) {
            if (ScaleManipulatorPressedOnce) {
                choose_manipulator<ScaleManipulator>();
                ScaleManipulatorPressedOnce = false;
            }
        } else {
            ScaleManipulatorPressedOnce = true;
        }
        if (_playing) {
            add_shortcut<EditorStopPlayback, ImGuiKey_Space>();
        } else {
            add_shortcut<EditorStartPlayback, ImGuiKey_Space>();
        }
    }
}

void Viewport::handle_manipulation_events() {

    ImGuiContext *g = ImGui::GetCurrentContext();
    ImGuiIO &io = ImGui::GetIO();

    // Check the mouse is over this widget
    if (ImGui::IsItemHovered()) {
        const GfVec2i drawTargetSize = _drawTarget->GetSize();
        if (drawTargetSize[0] == 0 || drawTargetSize[1] == 0) return;
        _mousePosition[0] = 2.0 * (static_cast<double>(io.MousePos.x - (g->LastItemData.Rect.Min.x)) /
                                   static_cast<double>(drawTargetSize[0])) -
                            1.0;
        _mousePosition[1] = -2.0 * (static_cast<double>(io.MousePos.y - (g->LastItemData.Rect.Min.y)) /
                                    static_cast<double>(drawTargetSize[1])) +
                            1.0;

        /// This works like a Finite state machine
        /// where every manipulator/editor is a state
        if (!_currentEditingState) {
            _currentEditingState = get_manipulator<MouseHoverManipulator>();
            _currentEditingState->on_begin_edition(*this);
        }

        auto newState = _currentEditingState->on_update(*this);
        if (newState != _currentEditingState) {
            _currentEditingState->on_end_edition(*this);
            _currentEditingState = newState;
            _currentEditingState->on_begin_edition(*this);
        }
    } else {// Mouse is outside of the viewport, reset the state
        if (_currentEditingState) {
            _currentEditingState->on_end_edition(*this);
            _currentEditingState = nullptr;
        }
    }
}

GfCamera &Viewport::get_current_camera() { return *_renderCamera; }

const GfCamera &Viewport::get_current_camera() const { return *_renderCamera; }

UsdGeomCamera Viewport::get_usd_geom_camera() { return UsdGeomCamera::Get(get_current_stage(), get_camera_path()); }

void Viewport::set_camera_path(const SdfPath &cameraPath) {
    _selectedCameraPath = cameraPath;

    _renderCamera = &_perspectiveCamera;// by default
    if (get_current_stage()) {
        const auto selectedCameraPrim = UsdGeomCamera::Get(get_current_stage(), _selectedCameraPath);
        if (selectedCameraPrim) {
            _renderCamera = &_stageCamera;
            _stageCamera = selectedCameraPrim.GetCamera(_imagingSettings.frame);
        }
    }
}

void Viewport::begin_hydra_ui(int width, int height) {
    // Create a ImGui windows to render the gizmos in
    ImGui_ImplOpenGL3_NewFrame();
    ImGuiIO &io = ImGui::GetIO();
    io.DisplaySize = ImVec2((float)width, (float)height);
    ImGui::NewFrame();
    static bool alwaysOpened = true;
    constexpr ImGuiDockNodeFlags dockFlags = ImGuiDockNodeFlags_None;
    constexpr ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
                                             ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                                             ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    ImGuiViewport *viewport = ImGui::GetMainViewport();
    // Full screen invisible window
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::SetNextWindowBgAlpha(0.0);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("HydraHUD", &alwaysOpened, windowFlags);
    ImGui::PopStyleVar(3);
}

void Viewport::end_hydra_ui() { ImGui::End(); }

void Viewport::render() {
    GfVec2i renderSize = _drawTarget->GetSize();
    int width = renderSize[0];
    int height = renderSize[1];

    if (width == 0 || height == 0)
        return;

    // Draw active manipulator and HUD
    begin_hydra_ui(width, height);
    get_active_manipulator().on_draw_frame(*this);
    // DrawHUD(this);
    end_hydra_ui();

    _drawTarget->Bind();
    glEnable(GL_DEPTH_TEST);
    glClearColor(_imagingSettings.clearColor[0], _imagingSettings.clearColor[1], _imagingSettings.clearColor[2],
                 _imagingSettings.clearColor[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, width, height);

    if (_renderer && get_current_stage()) {
        // Render hydra
        // Set camera and lighting state
        _imagingSettings.set_light_position_from_camera(get_current_camera());
        _renderer->SetLightingState(_imagingSettings.get_lights(), _imagingSettings._material, _imagingSettings._ambient);

        // Clipping planes
        _imagingSettings.clipPlanes.clear();
        for (int i = 0; i < get_current_camera().GetClippingPlanes().size(); ++i) {
            _imagingSettings.clipPlanes.emplace_back(get_current_camera().GetClippingPlanes()[i]);// convert float to double
        }

        GfVec4d viewport(0, 0, width, height);
        GfRect2i renderBufferRect(GfVec2i(0, 0), width, height);
        GfRange2f displayWindow(GfVec2f(viewport[0], height - viewport[1] - viewport[3]),
                                GfVec2f(viewport[0] + viewport[2], height - viewport[1]));
        GfRect2i dataWindow = renderBufferRect.GetIntersection(
            GfRect2i(GfVec2i(viewport[0], height - viewport[1] - viewport[3]),
                     viewport[2], viewport[3]));
        CameraUtilFraming framing(displayWindow, dataWindow);
        _renderer->SetRenderBufferSize(renderSize);
        _renderer->SetFraming(framing);
        _renderer->SetOverrideWindowPolicy(std::make_pair(true, CameraUtilConformWindowPolicy::CameraUtilMatchVertically));

        // If using a usd camera, use SetCameraPath renderer.SetCameraPath(sceneCam.GetPath())
        // else set camera state
        auto usdCamera = get_usd_geom_camera();
        if (usdCamera) {
            _renderer->SetCameraPath(usdCamera.GetPath());
        } else {
            _renderer->SetCameraState(get_current_camera().GetFrustum().ComputeViewMatrix(),
                                      get_current_camera().GetFrustum().ComputeProjectionMatrix());
        }
        _renderer->Render(get_current_stage()->GetPseudoRoot(), _imagingSettings);
    } else {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    _drawTarget->Unbind();
}

void Viewport::set_current_time_code(const UsdTimeCode &tc) {
    _imagingSettings.frame = tc;
}

/// Update anything that could have change after a frame render
void Viewport::update() {
    if (get_current_stage()) {
        auto whichRenderer = _renderers.find(get_current_stage());/// We expect a very limited number of opened stages
        if (whichRenderer == _renderers.end()) {
            SdfPathVector excludedPaths;
            _renderer = new UsdImagingGLEngine(get_current_stage()->GetPseudoRoot().GetPath(), excludedPaths);
            if (_renderers.empty()) {
                frame_root_prim();
            }
            _renderers[get_current_stage()] = _renderer;
            _cameraManipulator.set_z_is_up(UsdGeomGetStageUpAxis(get_current_stage()) == "Z");
            initialize_renderer_aov(*_renderer);
        } else if (whichRenderer->second != _renderer) {
            _renderer = whichRenderer->second;
            _cameraManipulator.set_z_is_up(UsdGeomGetStageUpAxis(get_current_stage()) == "Z");
        }

        // This should be extracted in a Playback module,
        // also the current code is not providing the exact frame rate, it doesn't take into account when the frame is
        // displayed. This is a first implementation to get an idea of how it should interact with the rest of the application.
        if (_playing) {
            auto current = clk::steady_clock::now();
            const auto timesCodePerSec = get_current_stage()->GetTimeCodesPerSecond();
            const auto timeDifference = std::chrono::duration<double>(current - _lastFrameTime);
            double newFrame =
                _imagingSettings.frame.GetValue() + timesCodePerSec * timeDifference.count();// for now just increment the frame
            if (newFrame > get_current_stage()->GetEndTimeCode()) {
                newFrame = get_current_stage()->GetStartTimeCode();
            } else if (newFrame < get_current_stage()->GetStartTimeCode()) {
                newFrame = get_current_stage()->GetStartTimeCode();
            }
            _imagingSettings.frame = UsdTimeCode(newFrame);
            _lastFrameTime = current;
        }

        // Camera -- TODO: is it slow to query the camera at each frame ?
        //                 the manipulator does is as well
        const auto stageCameraPrim = get_usd_geom_camera();
        if (stageCameraPrim) {
            _stageCamera = stageCameraPrim.GetCamera(get_current_time_code());
            _renderCamera = &_stageCamera;
        }
    }

    const GfVec2i &currentSize = _drawTarget->GetSize();
    if (currentSize != _textureSize) {
        _drawTarget->Bind();
        _drawTarget->SetSize(_textureSize);
        _drawTarget->Unbind();
    }

    // This is useful when a different camera is selected, or when the focal length is changed
    // But ideally we shouldn't update the camera at every frame
    if (get_current_camera().GetProjection() == GfCamera::Perspective) {
        get_current_camera().SetPerspectiveFromAspectRatioAndFieldOfView(double(_textureSize[0]) / double(_textureSize[1]),
                                                                         _renderCamera->GetFieldOfView(GfCamera::FOVHorizontal),
                                                                         GfCamera::FOVHorizontal);
    } else {// assuming ortho
        get_current_camera().SetOrthographicFromAspectRatioAndSize(double(_textureSize[0]) / double(_textureSize[1]),
                                                                   _renderCamera->GetVerticalAperture() * GfCamera::APERTURE_UNIT,
                                                                   GfCamera::FOVVertical);
    }
    if (_renderer && _selection.update_selection_hash(get_current_stage(), _lastSelectionHash)) {
        _renderer->ClearSelected();
        _renderer->SetSelected(_selection.get_selected_paths(get_current_stage()));

        // Tell the manipulators the selection has changed
        _positionManipulator.on_selection_change(*this);
        _rotationManipulator.on_selection_change(*this);
        _scaleManipulator.on_selection_change(*this);
    }
}

bool Viewport::test_intersection(GfVec2d clickedPoint, SdfPath &outHitPrimPath, SdfPath &outHitInstancerPath, int &outHitInstanceIndex) {
    GfVec2i renderSize = _drawTarget->GetSize();
    double width = static_cast<double>(renderSize[0]);
    double height = static_cast<double>(renderSize[1]);

    GfFrustum pixelFrustum = get_current_camera().GetFrustum().ComputeNarrowedFrustum(clickedPoint, GfVec2d(1.0 / width, 1.0 / height));
    GfVec3d outHitPoint;
    GfVec3d outHitNormal;
    return (_renderer && get_current_stage() && _renderer->TestIntersection(get_current_camera().GetFrustum().ComputeViewMatrix(), pixelFrustum.ComputeProjectionMatrix(), get_current_stage()->GetPseudoRoot(), _imagingSettings, &outHitPoint, &outHitNormal, &outHitPrimPath, &outHitInstancerPath, &outHitInstanceIndex));
}

void Viewport::start_playback() {
    _playing = true;
    _lastFrameTime = clk::steady_clock::now();
}

void Viewport::stop_playback() {
    _playing = false;
    // cast to nearest frame
    _imagingSettings.frame = UsdTimeCode(int(_imagingSettings.frame.GetValue()));
}

}// namespace vox
