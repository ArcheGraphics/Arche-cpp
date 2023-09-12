//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <pxr/usdImaging/usdImagingGL/engine.h>
#include <pxr/usdImaging/usdImagingGL/renderParams.h>
#include <pxr/imaging/glf/simpleLight.h>
#include <pxr/usd/usdGeom/camera.h>

PXR_NAMESPACE_USING_DIRECTIVE

namespace vox {
struct ImagingSettings : UsdImagingGLRenderParams {
    ImagingSettings();

    void set_light_position_from_camera(const GfCamera &);

    const GlfSimpleLightVector &get_lights();

    // Defaults GL lights and materials
    bool enableCameraLight;

    GlfSimpleMaterial _material;
    GfVec4f _ambient;

    // Viewport
    bool showGrid;

private:
    GlfSimpleLightVector _lights;
};

/// We keep track of the selected AOV in the UI, unfortunately the selected AOV is not awvailable in
/// UsdImagingGLEngine, so we need the initialize the UI data with this function
void initialize_renderer_aov(UsdImagingGLEngine &);

///
void draw_renderer_selection_combo(UsdImagingGLEngine &);
void draw_renderer_selection_list(UsdImagingGLEngine &);
void draw_renderer_controls(UsdImagingGLEngine &);
void draw_renderer_commands(UsdImagingGLEngine &);
void draw_renderer_settings(UsdImagingGLEngine &, ImagingSettings &);
void draw_imaging_settings(UsdImagingGLEngine &, ImagingSettings &);
void draw_aov_settings(UsdImagingGLEngine &);
void draw_color_correction(UsdImagingGLEngine &, ImagingSettings &);

}// namespace vox