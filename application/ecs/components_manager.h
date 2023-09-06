//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef component_manager_hpp
#define component_manager_hpp

#include "matrix4x4.h"
#include "bounding_frustum.h"
#include "input_events.h"
#include "scene_forward.h"
#include "rendering/render_element.h"
#include <typeindex>
#include <unordered_map>
#include <vector>

namespace vox {
/**
 * The manager of the components.
 */
class ComponentsManager {
public:
    void addOnStartScript(Script *script);

    void removeOnStartScript(Script *script);

    void addOnUpdateScript(Script *script);

    void removeOnUpdateScript(Script *script);

    void addDestroyComponent(Script *component);

    void callScriptOnStart();

    void callScriptOnUpdate(float deltaTime);

    void callScriptOnLateUpdate(float deltaTime);

    void callScriptInputEvent(const InputEvent &inputEvent);

    void callScriptResize(uint32_t win_width, uint32_t win_height,
                          uint32_t fb_width, uint32_t fb_height);

    void callComponentDestroy();

public:
    void addRenderer(Renderer *renderer);

    void removeRenderer(Renderer *renderer);

    void callRendererOnUpdate(float deltaTime);

    void callRender(Camera *camera,
                    std::vector<RenderElement> &opaqueQueue,
                    std::vector<RenderElement> &alphaTestQueue,
                    std::vector<RenderElement> &transparentQueue);

    void callRender(const BoundingFrustum &frustrum,
                    std::vector<RenderElement> &opaqueQueue,
                    std::vector<RenderElement> &alphaTestQueue,
                    std::vector<RenderElement> &transparentQueue);

public:
    void addOnUpdateAnimators(Animator *animator);

    void removeOnUpdateAnimators(Animator *animator);

    void callAnimatorUpdate(float deltaTime);

    void addOnUpdateSceneAnimators(SceneAnimator *animator);

    void removeOnUpdateSceneAnimators(SceneAnimator *animator);

    void callSceneAnimatorUpdate(float deltaTime);

public:
    void callCameraOnBeginRender(Camera *camera);

    void callCameraOnEndRender(Camera *camera);

    std::vector<Component *> getActiveChangedTempList();

    void putActiveChangedTempList(std::vector<Component *> &componentContainer);

private:
    // Script
    std::vector<Script *> _onStartScripts;
    std::vector<Script *> _onUpdateScripts;
    std::vector<Script *> _destroyComponents;

    // Render
    std::vector<Renderer *> _renderers;

    // Delay dispose active/inActive Pool
    std::vector<std::vector<Component *>> _componentsContainerPool;

    // Animatior
    std::vector<Animator *> _onUpdateAnimators;
    std::vector<SceneAnimator *> _onUpdateSceneAnimators;
};

}// namespace vox

#endif /* component_manager_hpp */
