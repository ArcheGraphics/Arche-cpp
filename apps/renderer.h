//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <Metal/Metal.hpp>
#include <MetalKit/MetalKit.hpp>

#include <pxr/base/gf/vec3f.h>
#include <pxr/usd/usd/stage.h>
#include <pxr/imaging/hdx/tokens.h>
#include <pxr/imaging/hdx/types.h>
#include <pxr/imaging/hgi/blitCmdsOps.h>
#include <pxr/usdImaging/usdImagingGL/engine.h>
#include <pxr/usd/usdGeom/bboxCache.h>

namespace vox {
class Camera;
class Renderer {
public:
    explicit Renderer(MTL::Device *pDevice);
    ~Renderer();

    void draw(MTK::View *pView);

    void drawableSizeWillChange(MTK::View *pView, CGSize size);

    /// Increases a counter that the draw method uses to determine if a frame needs to be rendered.
    void requestFrame();

    /// Loads the scene from the provided URL and prepares the camera.
    void setupScene(const std::string &url);

    [[nodiscard]] bool isZUp() const { return _isZUp; }

private:
    /// Sets an initial material for the scene.
    void initializeMaterial();

    /// Prepares the Metal objects for copying to the view.
    void loadMetal();

    void blitToView(MTK::View *view, MTL::CommandBuffer *commandBuffer, MTL::Texture *texture);

    /// Requests the bounding box cache from Hydra.
    pxr::UsdGeomBBoxCache computeBboxCache();

    /// Initializes the Storm engine.
    void initializeEngine();

    /// Draws the scene using Hydra.
    pxr::HgiTextureHandle drawWithHydra(double timeCode, CGSize viewSize);

    /// Draw the scene, and blit the result to the view.
    /// Returns false if the engine wasn't initialized.
    bool drawMainView(MTK::View *view, double timeCode);

    /// Uses Hydra to load the USD or USDZ file.
    bool loadStage(const std::string &filePath);

    /// Determine the size of the world so the camera will frame its entire bounding box.
    void calculateWorldCenterAndSize();

    /// Sets a camera up so that it sees the entire scene.
    void setupCamera();

    /// Gets important information about the scene, such as frames per second and if the z-axis points up.
    void getSceneInformation();

    /// Updates the animation timing variables.
    double updateTime();

private:
    MTL::Device *_device;
    std::shared_ptr<MTL::RenderPipelineState> _blitToViewPSO{};
    dispatch_semaphore_t _inFlightSemaphore{};

    double _startTimeInSeconds{};
    double _timeCodesPerSecond{};
    double _startTimeCode{};
    double _endTimeCode{};
    pxr::GfVec3d _worldCenter{};
    double _worldSize{};
    int32_t _requestedFrames{};
    bool _sceneSetup{};

    pxr::GlfSimpleMaterial _material;
    pxr::GfVec4f _sceneAmbient{};

    pxr::HgiUniquePtr _hgi;
    std::shared_ptr<pxr::UsdImagingGLEngine> _engine;
    pxr::UsdStageRefPtr _stage;

    bool _isZUp{};
    std::unique_ptr<Camera> _viewCamera;
};
}// namespace vox