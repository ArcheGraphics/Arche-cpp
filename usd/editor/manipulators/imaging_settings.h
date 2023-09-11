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

    void SetLightPositionFromCamera(const GfCamera &);

    // Defaults GL lights and materials
    bool enableCameraLight;
    const GlfSimpleLightVector &GetLights();

    GlfSimpleMaterial _material;
    GfVec4f _ambient;

    // Viewport
    bool showGrid;

private:
    GlfSimpleLightVector _lights;
};

/// We keep track of the selected AOV in the UI, unfortunately the selected AOV is not awvailable in
/// UsdImagingGLEngine, so we need the initialize the UI data with this function
void InitializeRendererAov(UsdImagingGLEngine &);

///
void DrawRendererSelectionCombo(UsdImagingGLEngine &);
void DrawRendererSelectionList(UsdImagingGLEngine &);
void DrawRendererControls(UsdImagingGLEngine &);
void DrawRendererCommands(UsdImagingGLEngine &);
void DrawRendererSettings(UsdImagingGLEngine &, ImagingSettings &);
void DrawImagingSettings(UsdImagingGLEngine &, ImagingSettings &);
void DrawAovSettings(UsdImagingGLEngine &);
void DrawColorCorrection(UsdImagingGLEngine &, ImagingSettings &);

}// namespace vox