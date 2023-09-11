//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

///
/// OpenGL/Hydra Viewport and its ImGui Window drawing functions.
/// This will eventually be split in 2 different files as the code
/// has grown too much and doing too many thing
///
#include <map>
#include <chrono>
#include "manipulator.h"
#include "camera_manipulator.h"
#include "position_manipulator.h"
#include "mouse_hover_manipulator.h"
#include "selection_manipulator.h"
#include "rotation_manipulator.h"
#include "scale_manipulator.h"
#include "selection.h"
#include <pxr/imaging/glf/drawTarget.h>
#include <pxr/usd/usd/stage.h>
#include <pxr/usdImaging/usdImagingGL/engine.h>

#include "imaging_settings.h"

namespace vox {
class Viewport final {
public:
    Viewport(UsdStageRefPtr stage, Selection &);
    ~Viewport();

    // Delete copy
    Viewport(const Viewport &) = delete;
    Viewport &operator=(const Viewport &) = delete;

    /// Render hydra
    void render();

    /// Update internal data: selection, current renderer
    void update();

    /// Draw the full widget
    void draw();

    /// Returns the time code of this viewport
    UsdTimeCode get_current_time_code() const { return _imagingSettings.frame; }
    void set_current_time_code(const UsdTimeCode &tc);

    /// Camera framing
    void frame_selection(const Selection &);
    void frame_root_prim();

    // Cameras
    /// Return the camera used to render the viewport
    GfCamera &get_current_camera();
    const GfCamera &get_current_camera() const;

    // Set the camera path
    void set_camera_path(const SdfPath &cameraPath);
    const SdfPath &get_camera_path() { return _selectedCameraPath; }
    // Returns a UsdGeom camera if the selected camera is in the stage
    UsdGeomCamera get_usd_geom_camera();

    CameraManipulator &get_camera_manipulator() { return _cameraManipulator; }

    // Picking
    bool test_intersection(GfVec2d clickedPoint, SdfPath &outHitPrimPath, SdfPath &outHitInstancerPath, int &outHitInstanceIndex);
    GfVec2d get_picking_boundary_size() const;

    // Utility function for compute a scale for the manipulators. It uses the distance between the camera
    // and objectPosition. TODO: remove multiplier, not useful anymore
    double compute_scale_factor(const GfVec3d &objectPosition, double multiplier = 1.0) const;

    /// All the manipulators are currently stored in this class, this might change, but right now
    /// GetManipulator is the function that will return the official manipulator based on its type ManipulatorT
    template<typename ManipulatorT>
    inline Manipulator *get_manipulator();
    Manipulator &get_active_manipulator() { return *_activeManipulator; }

    // The chosen manipulator is the one selected in the toolbar, Translate/Rotate/Scale/Select ...
    // Manipulator *_chosenManipulator;
    template<typename ManipulatorT>
    inline void choose_manipulator() { _activeManipulator = get_manipulator<ManipulatorT>(); };
    template<typename ManipulatorT>
    inline bool is_chosen_manipulator() {
        return _activeManipulator == get_manipulator<ManipulatorT>();
    };

    /// Draw manipulator toolbox, to select translate, rotate, scale
    void draw_manipulator_toolbox(const ImVec2 widgetPosition);

    /// Draw toolbar: camera selection, renderer options, viewport options ...
    void draw_tool_bar(const ImVec2 widgetPosition);

    /// Draw stage selector
    void draw_stage_selector(const ImVec2 widgetPosition);

    // Position of the mouse in the viewport in normalized unit
    // This is computed in HandleEvents

    GfVec2d get_mouse_position() const { return _mousePosition; }

    UsdStageRefPtr get_current_stage() { return _stage; }
    const UsdStageRefPtr get_current_stage() const { return _stage; };

    void set_current_stage(UsdStageRefPtr stage) { _stage = stage; }

    Selection &get_selection() { return _selection; }

    SelectionManipulator &get_selection_manipulator() { return _selectionManipulator; }

    /// Handle events is implemented as a finite state machine.
    /// The state are simply the current manipulator used.
    void handle_manipulation_events();
    void handle_keyboard_shortcut();

    /// Playback controls
    void start_playback();
    void stop_playback();

private:
    // Manipulators
    Manipulator *_currentEditingState;// Manipulator currently used by the FSM
    Manipulator *_activeManipulator;  // Manipulator chosen by the user
    CameraManipulator _cameraManipulator;
    PositionManipulator _positionManipulator;
    RotationManipulator _rotationManipulator;
    MouseHoverManipulator _mouseHover;
    ScaleManipulator _scaleManipulator;
    SelectionManipulator _selectionManipulator;

    Selection &_selection;
    SelectionHash _lastSelectionHash = 0;

    /// Cameras
    SdfPath _selectedCameraPath;
    GfCamera *_renderCamera;// Points to a valid camera, stage or perspective
    GfCamera _stageCamera;
    // TODO: if we want to have multiple viewport, the persp camera shouldn't belong to the viewport but
    // another shared object, CameraList or similar
    GfCamera _perspectiveCamera;// opengl

    // Hydra canvas
    void begin_hydra_ui(int width, int height);
    void end_hydra_ui();
    GfVec2i _textureSize;
    GfVec2d _mousePosition;

    UsdStageRefPtr _stage;

    // Renderer
    GLuint _textureId = 0;
    std::map<UsdStageRefPtr, UsdImagingGLEngine *> _renderers;
    UsdImagingGLEngine *_renderer = nullptr;
    ImagingSettings _imagingSettings;
    GlfDrawTargetRefPtr _drawTarget;

    // Playback controls
    bool _playing = false;
    std::chrono::time_point<std::chrono::steady_clock> _lastFrameTime;
};

template<>
inline Manipulator *Viewport::get_manipulator<PositionManipulator>() { return &_positionManipulator; }
template<>
inline Manipulator *Viewport::get_manipulator<RotationManipulator>() { return &_rotationManipulator; }
template<>
inline Manipulator *Viewport::get_manipulator<MouseHoverManipulator>() { return &_mouseHover; }
template<>
inline Manipulator *Viewport::get_manipulator<CameraManipulator>() { return &_cameraManipulator; }
template<>
inline Manipulator *Viewport::get_manipulator<SelectionManipulator>() { return &_selectionManipulator; }
template<>
inline Manipulator *Viewport::get_manipulator<ScaleManipulator>() { return &_scaleManipulator; }

}// namespace vox