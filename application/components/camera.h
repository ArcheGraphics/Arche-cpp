//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "ecs/component.h"
#include "shader/shader_data.h"
#include "base/layer.h"
#include "math/ray3.h"
#include "math/matrix4x4.h"
#include "math/bounding_frustum.h"
#include "base/update_flag.h"

namespace vox {
class Transform;

/**
 * Camera clear flags enumeration.
 */
enum class CameraClearFlags {
    /* Clear depth and color from background. */
    DepthColor,
    /* Clear depth only. */
    Depth,
    /* Do nothing. */
    None
};

/**
 * Camera component, as the entrance to the three-dimensional world.
 */
class Camera : public Component {
public:
    struct alignas(16) CameraData {
        Matrix4x4F view_mat;
        Matrix4x4F proj_mat;
        Matrix4x4F vp_mat;
        Matrix4x4F view_inv_mat;
        Matrix4x4F proj_inv_mat;
        Point3F camera_pos;
    };

    /** Shader data. */
    ShaderData shaderData;

    /** Rendering priority - A Camera with higher priority will be rendered on top of a camera with lower priority. */
    int priority = 0;

    /** Whether to enable frustum culling, it is enabled by default. */
    bool enableFrustumCulling = true;

    /**
     * Determining what to clear when rendering by a Camera.
     * @defaultValue `CameraClearFlags.DepthColor`
     */
    CameraClearFlags clearFlags = CameraClearFlags::DepthColor;

    /**
     * Culling mask - which layers the camera renders.
     * @remarks Support bit manipulation, corresponding to Entity's layer.
     */
    Layer cullingMask = Layer::EVERYTHING;

    /**
     * Create the Camera component.
     * @param entity - Entity
     */
    explicit Camera(Entity *entity);

    [[nodiscard]] const BoundingFrustum &get_frustum() const;

    /**
     * Near clip plane - the closest point to the camera when rendering occurs.
     */
    [[nodiscard]] float nearClipPlane() const;

    void setNearClipPlane(float value);

    /**
     * Far clip plane - the furthest point to the camera when rendering occurs.
     */
    [[nodiscard]] float farClipPlane() const;

    void setFarClipPlane(float value);

    /**
     * The camera's view angle. activating when camera use perspective projection.
     */
    [[nodiscard]] float fieldOfView() const;

    void setFieldOfView(float value);

    /**
     * Aspect ratio. The default is automatically calculated by the viewport's aspect ratio. If it is manually set,
     * the manual value will be kept. Call resetAspectRatio() to restore it.
     */
    [[nodiscard]] float aspectRatio() const;

    void setAspectRatio(float value);

    /**
     * Viewport, normalized expression, the upper left corner is (0, 0), and the lower right corner is (1, 1).
     * @remarks Re-assignment is required after modification to ensure that the modification takes effect.
     */
    [[nodiscard]] Vector4F viewport() const;

    void setViewport(const Vector4F &value);

    /**
     * Whether it is orthogonal, the default is false. True will use orthographic projection, false will use perspective projection.
     */
    [[nodiscard]] bool isOrthographic() const;

    void setIsOrthographic(bool value);

    /**
     * Half the size of the camera in orthographic mode.
     */
    [[nodiscard]] float orthographicSize() const;

    void setOrthographicSize(float value);

    /**
     * View matrix.
     */
    Matrix4x4F viewMatrix();

    /**
     * The projection matrix is ​​calculated by the relevant parameters of the camera by default.
     * If it is manually set, the manual value will be maintained. Call resetProjectionMatrix() to restore it.
     */
    void setProjectionMatrix(const Matrix4x4F &value);

    Matrix4x4F projectionMatrix();

    /**
     * Whether to enable HDR.
     * @todo When render pipeline modification
     */
    bool enableHDR();

    void setEnableHDR(bool value);

public:
    /**
     * Restore the automatic calculation of projection matrix through fieldOfView, nearClipPlane and farClipPlane.
     */
    void resetProjectionMatrix();

    /**
     * Restore the automatic calculation of the aspect ratio through the viewport aspect ratio.
     */
    void resetAspectRatio();

    /**
     * Transform a point from world space to viewport space.
     * @param point - Point in world space
     * @return out - A point in the viewport space, X and Y are the viewport space coordinates,
     * Z is the viewport depth, the near clipping plane is 0, the far clipping plane is 1, and W is the world unit distance from the camera
     */
    Vector4F worldToViewportPoint(const Point3F &point);

    /**
     * Transform a point from viewport space to world space.
     * @param point - Point in viewport space, X and Y are the viewport space coordinates,
     * Z is the viewport depth. The near clipping plane is 0, and the far clipping plane is 1
     * @returns Point in world space
     */
    Point3F viewportToWorldPoint(const Vector3F &point);

    /**
     * Generate a ray by a point in viewport.
     * @param point - Point in viewport space, which is represented by normalization
     * @returns Ray
     */
    Ray3F viewportPointToRay(const Vector2F &point);

    /**
     * Transform the X and Y coordinates of a point from screen space to viewport space
     * @param point - Point in screen space
     * @returns Point in viewport space
     */
    Vector2F screenToViewportPoint(const Vector2F &point);

    Vector3F screenToViewportPoint(const Vector3F &point);

    /**
     * Transform the X and Y coordinates of a point from viewport space to screen space.
     * @param point - Point in viewport space
     * @returns Point in screen space
     */
    Vector2F viewportToScreenPoint(const Vector2F &point);

    Vector3F viewportToScreenPoint(const Vector3F &point);

    Vector4F viewportToScreenPoint(const Vector4F &point);

    /**
     * Transform a point from world space to screen space.
     * @param point - Point in world space
     * @returns Point of screen space
     */
    Vector4F worldToScreenPoint(const Point3F &point);

    /**
     * Transform a point from screen space to world space.
     * @param point - Screen space point
     * @returns Point in world space
     */
    Point3F screenToWorldPoint(const Vector3F &point);

    /**
     * Generate a ray by a point in screen.
     * @param point - Point in screen space, the unit is pixel
     * @returns Ray
     */
    Ray3F screenPointToRay(const Vector2F &point);

    void resize(uint32_t win_width, uint32_t win_height,
                uint32_t fb_width, uint32_t fb_height);

    [[nodiscard]] uint32_t width() const;

    [[nodiscard]] uint32_t height() const;

    [[nodiscard]] uint32_t framebufferWidth() const;

    [[nodiscard]] uint32_t framebufferHeight() const;

    void update();

public:
    void on_active() override;

    void on_inactive() override;

private:
    friend class ComponentsManager;

    void _projMatChange();

    Point3F _innerViewportToWorldPoint(const Vector3F &point, const Matrix4x4F &invViewProjMat);

    /**
     * The inverse matrix of view projection matrix.
     */
    Matrix4x4F invViewProjMat();

    /**
     * The inverse of the projection matrix.
     */
    Matrix4x4F inverseProjectionMatrix();

    CameraData _cameraData;
    std::string _cameraProperty = "u_camera";

    BoundingFrustum _frustum = BoundingFrustum();

    bool _isOrthographic = false;
    bool _isProjMatSetting = false;
    float _nearClipPlane = 0.1;
    float _farClipPlane = 500;
    float _fieldOfView = 45;
    float _orthographicSize = 10;
    bool _isProjectionDirty = true;
    bool _isInvProjMatDirty = true;
    bool _isFrustumProjectDirty = true;
    std::optional<float> _customAspectRatio = std::nullopt;

    std::unique_ptr<UpdateFlag> _frustumViewChangeFlag;
    Transform *_transform;
    std::unique_ptr<UpdateFlag> _isViewMatrixDirty;
    std::unique_ptr<UpdateFlag> _isInvViewProjDirty;
    Matrix4x4F _projectionMatrix = Matrix4x4F();
    Matrix4x4F _viewMatrix = Matrix4x4F();
    Vector4F _viewport = Vector4F(0, 0, 1, 1);
    Matrix4x4F _inverseProjectionMatrix = Matrix4x4F();
    Vector2F _lastAspectSize = Vector2F();
    Matrix4x4F _invViewProjMat = Matrix4x4F();

    uint32_t _width{0};
    uint32_t _height{0};
    uint32_t _fbWidth{0};
    uint32_t _fbHeight{0};
};

}// namespace vox