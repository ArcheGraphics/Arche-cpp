//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "renderer.h"
#include "usd/camera.h"
#include "common/metal_helpers.h"
#include "common/logging.h"
#include "common/filesystem.h"

#include <pxr/pxr.h>
#include <pxr/base/gf/camera.h>
#include <pxr/base/plug/plugin.h>
#include "pxr/base/plug/registry.h"
#include <pxr/usd/usd/prim.h>
#include <pxr/usd/usd/primRange.h>
#include "pxr/usd/usdGeom/camera.h"
#include "pxr/usd/usdGeom/metrics.h"
#include <pxr/imaging/hgi/blitCmdsOps.h>
#include <pxr/imaging/hgiMetal/hgi.h>
#include <pxr/imaging/hgiMetal/texture.h>
#include <MetalKit/MetalKit.h>
#import <CoreImage/CIContext.h>

#include <spdlog/async_logger.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <cmath>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

using namespace pxr;

namespace vox {
namespace {
const double AAPLDefaultFocalLength = 18.0;
const uint32_t AAPLMaxBuffersInFlight = 3;

/// Returns the current time in seconds from the system high-resolution clock.
inline double getCurrentTimeInSeconds() {
    using Clock = std::chrono::high_resolution_clock;
    using Ns = std::chrono::nanoseconds;
    std::chrono::time_point<Clock, Ns> tp = std::chrono::high_resolution_clock::now();
    return tp.time_since_epoch().count() / 1e9;
}

/// Returns true if the bounding box has infinite floating point values.
bool isInfiniteBBox(const GfBBox3d &bbox) {
    return (isinf(bbox.GetRange().GetMin().GetLength()) ||
            isinf(bbox.GetRange().GetMax().GetLength()));
}

/// Creates a light source located at the camera position.
GlfSimpleLight computeCameraLight(const GfMatrix4d &cameraTransform) {
    GfVec3f cameraPosition = GfVec3f(cameraTransform.ExtractTranslation());

    GlfSimpleLight light;
    light.SetPosition(GfVec4f(cameraPosition[0], cameraPosition[1], cameraPosition[2], 1));

    return light;
}

/// Computes all light sources for the scene.
GlfSimpleLightVector computeLights(const GfMatrix4d &cameraTransform) {
    GlfSimpleLightVector lights;
    lights.push_back(computeCameraLight(cameraTransform));

    return lights;
}

/// Checks if the USD prim derives from the requested schema type.
bool primDerivesFromSchemaType(UsdPrim const &prim, TfType const &schemaType) {
    // Check if the schema `TfType` is defined.
    if (schemaType.IsUnknown()) {
        return false;
    }

    // Get the primitive `TfType` string to query the USD plugin registry instance.
    const std::string &typeName = prim.GetTypeName().GetString();

    // Return `true` if the prim's schema type is found in the plugin registry.
    return !typeName.empty() &&
           PlugRegistry::GetInstance().FindDerivedTypeByName<UsdSchemaBase>(typeName).IsA(schemaType);
}

/// Queries the USD for all the prims that derive from the requested schema type.
std::vector<UsdPrim> getAllPrimsOfType(UsdStagePtr const &stage,
                                       TfType const &schemaType) {
    std::vector<UsdPrim> result;
    UsdPrimRange range = stage->Traverse();
    std::copy_if(range.begin(), range.end(), std::back_inserter(result),
                 [schemaType](UsdPrim const &prim) {
                     return primDerivesFromSchemaType(prim, schemaType);
                 });
    return result;
}

/// Computes a frustum from the camera and the current view size.
GfFrustum computeFrustum(const GfMatrix4d &cameraTransform,
                         CGSize viewSize,
                         const CameraParams &cameraParams) {
    GfCamera camera;
    camera.SetTransform(cameraTransform);
    GfFrustum frustum = camera.GetFrustum();
    camera.SetFocalLength(cameraParams.focalLength);

    if (cameraParams.projection == Projection::Perspective) {
        double targetAspect = double(viewSize.width) / double(viewSize.height);
        float filmbackWidthMM = 24.0;
        double hFOVInRadians = 2.0 * atan(0.5 * filmbackWidthMM / cameraParams.focalLength);
        double fov = (180.0 * hFOVInRadians) / M_PI;
        frustum.SetPerspective(fov, targetAspect, 1.0, 100000.0);
    } else {
        double left = cameraParams.leftBottomNear[0] * cameraParams.scaleViewport;
        double right = cameraParams.rightTopFar[0] * cameraParams.scaleViewport;
        double bottom = cameraParams.leftBottomNear[1] * cameraParams.scaleViewport;
        double top = cameraParams.rightTopFar[1] * cameraParams.scaleViewport;
        double nearPlane = cameraParams.leftBottomNear[2];
        double farPlane = cameraParams.rightTopFar[2];

        frustum.SetOrthographic(left, right, bottom, top, nearPlane, farPlane);
    }

    return frustum;
}
}// namespace

/// Sets an initial material for the scene.
void Renderer::initializeMaterial() {
    float kA = 0.2f;
    float kS = 0.1f;
    _material.SetAmbient(GfVec4f(kA, kA, kA, 1.0f));
    _material.SetSpecular(GfVec4f(kS, kS, kS, 1.0f));
    _material.SetShininess(32.0);

    _sceneAmbient = GfVec4f(0.01f, 0.01f, 0.01f, 1.0f);
}

/// Requests the bounding box cache from Hydra.
pxr::UsdGeomBBoxCache Renderer::computeBboxCache() {
    TfTokenVector purposes;
    purposes.push_back(UsdGeomTokens->default_);
    purposes.push_back(UsdGeomTokens->proxy);

    // Extent hints are sometimes authored as an optimization to avoid
    // computing bounds. They are particularly useful for some tests where
    // there's no bound on the first frame.
    bool useExtentHints = true;
    UsdTimeCode timeCode = UsdTimeCode::Default();
    if (_stage->HasAuthoredTimeCodeRange()) {
        timeCode = _stage->GetStartTimeCode();
    }
    UsdGeomBBoxCache bboxCache(timeCode, purposes, useExtentHints);
    return bboxCache;
}

/// Initializes the Storm engine.
void Renderer::initializeEngine() {
    _inFlightSemaphore = dispatch_semaphore_create(AAPLMaxBuffersInFlight);

    SdfPathVector excludedPaths;
    _hgi = Hgi::CreatePlatformDefaultHgi();
    HdDriver driver{HgiTokens->renderDriver, VtValue(_hgi.get())};

    _engine = std::make_shared<pxr::UsdImagingGLEngine>(_stage->GetPseudoRoot().GetPath(),
                                                        excludedPaths, SdfPathVector(),
                                                        SdfPath::AbsoluteRootPath(), driver);

    _engine->SetEnablePresentation(false);
    _engine->SetRendererAov(HdAovTokens->color);
}

/// Draws the scene using Hydra.
pxr::HgiTextureHandle Renderer::drawWithHydra(double timeCode, CGSize viewSize) {
    // Camera projection setup.
    GfMatrix4d cameraTransform = _viewCamera->getTransform();
    CameraParams cameraParams = _viewCamera->getShaderParams();
    GfFrustum frustum = computeFrustum(cameraTransform, viewSize, cameraParams);
    GfMatrix4d modelViewMatrix = frustum.ComputeViewMatrix();
    GfMatrix4d projMatrix = frustum.ComputeProjectionMatrix();
    _engine->SetCameraState(modelViewMatrix, projMatrix);

    // Viewport setup.
    GfVec4d viewport(0, 0, viewSize.width, viewSize.height);
    _engine->SetRenderViewport(viewport);
    _engine->SetWindowPolicy(CameraUtilMatchVertically);

    // Light and material setup.
    GlfSimpleLightVector lights = computeLights(cameraTransform);
    _engine->SetLightingState(lights, _material, _sceneAmbient);

    // Nondefault render parameters.
    UsdImagingGLRenderParams params;
    params.clearColor = GfVec4f(0.0f, 0.0f, 0.0f, 0.0f);
    params.colorCorrectionMode = HdxColorCorrectionTokens->sRGB;
    params.frame = timeCode;

    // Render the frame.
    TfErrorMark mark;
    _engine->Render(_stage->GetPseudoRoot(), params);
    TF_VERIFY(mark.IsClean(), "Errors occurred while rendering!");

    // Return the color output.
    return _engine->GetAovTexture(HdAovTokens->color);
}

/// Draw the scene, and blit the result to the view.
/// Returns false if the engine wasn't initialized.
bool Renderer::drawMainView(double timeCode) {
    if (!_engine) {
        return false;
    }

    // Start the next frame.
    dispatch_semaphore_wait(_inFlightSemaphore, DISPATCH_TIME_FOREVER);
    auto *hgi = static_cast<HgiMetal *>(_hgi.get());
    hgi->StartFrame();

    // Draw the scene using Hydra, and recast the result to a MTLTexture.
    CGSize viewSize = _swapchain->layer()->drawableSize();
    HgiTextureHandle hgiTexture = drawWithHydra(timeCode, viewSize);
    auto texture = static_cast<HgiMetalTexture *>(hgiTexture.Get())->GetTextureId();

    // Create a command buffer to blit the texture to the view.
    id<MTLCommandBuffer> commandBuffer = hgi->GetPrimaryCommandBuffer();
    __block dispatch_semaphore_t blockSemaphore = _inFlightSemaphore;
    [commandBuffer addCompletedHandler:^(id<MTLCommandBuffer> buffer) {
        dispatch_semaphore_signal(blockSemaphore);
    }];

    // Copy the rendered texture to the view.
    _swapchain->present((__bridge MTL::CommandBuffer *)(commandBuffer), (__bridge MTL::Texture *)(texture));

    // Tell Hydra to commit the command buffer, and complete the work.
    hgi->CommitPrimaryCommandBuffer();
    hgi->EndFrame();

    return true;
}

/// Loads the scene from the provided URL and prepares the camera.
void Renderer::setupScene(const std::string &url) {
    // Load USD stage.
    if (!loadStage(url)) {
        // Failed to load stage. Nothing to render.
        return;
    }

    // Get scene information.
    getSceneInformation();

    // Set up the initial scene camera based on the loaded stage.
    setupCamera();

    _sceneSetup = true;
}

/// Determine the size of the world so the camera will frame its entire bounding box.
void Renderer::calculateWorldCenterAndSize() {
    UsdGeomBBoxCache bboxCache = computeBboxCache();

    GfBBox3d bbox = bboxCache.ComputeWorldBound(_stage->GetPseudoRoot());

    // Copy the behavior of usdView.
    // If the bounding box is empty or infinite, set it to a default size.
    if (bbox.GetRange().IsEmpty() || isInfiniteBBox(bbox)) {
        bbox = {{{-10, -10, -10}, {10, 10, 10}}};
    }

    GfRange3d world = bbox.ComputeAlignedRange();

    _worldCenter = (world.GetMin() + world.GetMax()) / 2.0;
    _worldSize = world.GetSize().GetLength();
}

/// Sets a camera up so that it sees the entire scene.
void Renderer::setupCamera() {
    calculateWorldCenterAndSize();

    std::vector<UsdPrim> sceneCameras = getAllPrimsOfType(_stage, TfType::Find<UsdGeomCamera>());

    if (sceneCameras.empty()) {
        _viewCamera = std::make_unique<Camera>(this);
        _viewCamera->setRotation({0.0, 0.0, 0.0});
        _viewCamera->setFocus(_worldCenter);
        _viewCamera->setDistance(_worldSize);

        if (_worldSize <= 16.0) {
            _viewCamera->setScaleBias(1.0);
        } else {
            _viewCamera->setScaleBias(std::log(_worldSize / 16.0 * 1.8) / std::log(1.8));
        }

        _viewCamera->setFocalLength(AAPLDefaultFocalLength);
        _viewCamera->setStandardFocalLength(AAPLDefaultFocalLength);
    } else {
        UsdPrim sceneCamera = sceneCameras[0];
        UsdGeomCamera geomCamera = UsdGeomCamera(sceneCamera);
        GfCamera camera = geomCamera.GetCamera(_startTimeCode);
        _viewCamera = std::make_unique<Camera>(camera, this);
    }
}

/// Gets important information about the scene, such as frames per second and if the z-axis points up.
void Renderer::getSceneInformation() {
    _timeCodesPerSecond = _stage->GetFramesPerSecond();
    if (_stage->HasAuthoredTimeCodeRange()) {
        _startTimeCode = _stage->GetStartTimeCode();
        _endTimeCode = _stage->GetEndTimeCode();
    }
    _isZUp = (UsdGeomGetStageUpAxis(_stage) == UsdGeomTokens->z);
}

/// Updates the animation timing variables.
double Renderer::updateTime() {
    double currentTimeInSeconds = getCurrentTimeInSeconds();

    // Store the ticks for the first frame.
    if (_startTimeInSeconds == 0) {
        _startTimeInSeconds = currentTimeInSeconds;
    }

    // Calculate the elapsed time in seconds from the start.
    double elapsedTimeInSeconds = currentTimeInSeconds - _startTimeInSeconds;

    // Loop the animation if it is past the end.
    double timeCode = _startTimeCode + elapsedTimeInSeconds * _timeCodesPerSecond;
    if (timeCode > _endTimeCode) {
        timeCode = _startTimeCode;
        _startTimeInSeconds = currentTimeInSeconds;
    }

    return timeCode;
}

void Renderer::draw() {
    // There's nothing to render until the scene is set up.
    if (!_sceneSetup) {
        return;
    }

    // There's nothing to render if there isn't a frame requested or the stage isn't animated.
    if (_requestedFrames == 0 && _startTimeCode == _endTimeCode) {
        return;
    }

    // Set up the engine the first time you attempt to render the stage.
    if (!_engine) {
        // Initialize the Storm render engine.
        initializeEngine();
    }

    double timeCode = updateTime();

    bool drawSucceeded = drawMainView(timeCode);

    if (drawSucceeded) {
        _requestedFrames--;
    }
}

/// Increases a counter that the draw method uses to determine if a frame needs to be rendered.
void Renderer::requestFrame() {
    _requestedFrames++;
}

/// Uses Hydra to load the USD or USDZ file.
bool Renderer::loadStage(const std::string &filePath) {
    _stage = UsdStage::Open(filePath);
    if (!_stage) {
        LOGE("Failed to load stage at {}", filePath)
        return false;
    }
    return true;
}

Renderer::Renderer(uint64_t window_handle, uint width, uint height) {
    _device = make_shared(MTL::CreateSystemDefaultDevice());
    _requestedFrames = 1;
    _startTimeInSeconds = 0;
    _sceneSetup = false;

    _swapchain = std::make_unique<metal::MetalSwapchain>(*_device, window_handle, width, height);

    initializeMaterial();

    // setup logger
    std::vector<spdlog::sink_ptr> sinks;
    sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());

    auto logger = std::make_shared<spdlog::logger>("logger", sinks.begin(), sinks.end());

#ifdef METAL_DEBUG
    logger->set_level(spdlog::level::debug);
#else
    logger->set_level(spdlog::level::info);
#endif

    logger->set_pattern(LOGGER_FORMAT);
    spdlog::set_default_logger(logger);
}

Renderer::~Renderer() {
    _device.reset();
    _engine.reset();
    _stage.Reset();
    _swapchain.reset();

    spdlog::drop_all();
}

}// namespace vox
