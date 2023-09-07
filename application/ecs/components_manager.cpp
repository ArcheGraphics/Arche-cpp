//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "components_manager.h"
#include "components/script.h"
#include "components/renderer.h"
#include "entity.h"
#include "components/camera.h"

namespace vox {
void ComponentsManager::addOnStartScript(Script *script) {
    script->_onStartIndex = _onStartScripts.size();
    _onStartScripts.push_back(script);
}

void ComponentsManager::removeOnStartScript(Script *script) {
    _onStartScripts.erase(_onStartScripts.begin() + script->_onStartIndex);
    script->_onStartIndex = -1;
}

void ComponentsManager::addOnUpdateScript(Script *script) {
    script->_onUpdateIndex = _onUpdateScripts.size();
    _onUpdateScripts.push_back(script);
}

void ComponentsManager::removeOnUpdateScript(Script *script) {
    _onUpdateScripts.erase(_onUpdateScripts.begin() + script->_onUpdateIndex);
    script->_onUpdateIndex = -1;
}

void ComponentsManager::addDestroyComponent(Script *component) {
    _destroyComponents.push_back(component);
}

void ComponentsManager::callComponentDestroy() {
    if (_destroyComponents.size() > 0) {
        for (size_t i = 0; i < _destroyComponents.size(); i++) {
            _destroyComponents[i]->onDestroy();
        }
        _destroyComponents.clear();
    }
}

void ComponentsManager::callScriptOnStart() {
    if (_onStartScripts.size() > 0) {
        // The 'onStartScripts.length' maybe add if you add some Script with addComponent() in some Script's onStart()
        for (size_t i = 0; i < _onStartScripts.size(); i++) {
            const auto &script = _onStartScripts[i];
            script->_started = true;
            script->_onStartIndex = -1;
            script->onStart();
        }
        _onStartScripts.clear();
    }
}

void ComponentsManager::callScriptOnUpdate(float deltaTime) {
    for (size_t i = 0; i < _onUpdateScripts.size(); i++) {
        const auto &element = _onUpdateScripts[i];
        if (element->_started) {
            element->onUpdate(deltaTime);
        }
    }
}

void ComponentsManager::callScriptOnLateUpdate(float deltaTime) {
    for (size_t i = 0; i < _onUpdateScripts.size(); i++) {
        const auto &element = _onUpdateScripts[i];
        if (element->_started) {
            element->onLateUpdate(deltaTime);
        }
    }
}

void ComponentsManager::callScriptInputEvent(const InputEvent &inputEvent) {
    for (size_t i = 0; i < _onUpdateScripts.size(); i++) {
        const auto &element = _onUpdateScripts[i];
        if (element->_started) {
            element->inputEvent(inputEvent);
        }
    }
}

void ComponentsManager::callScriptResize(uint32_t win_width, uint32_t win_height,
                                         uint32_t fb_width, uint32_t fb_height) {
    for (size_t i = 0; i < _onUpdateScripts.size(); i++) {
        const auto &element = _onUpdateScripts[i];
        if (element->_started) {
            element->resize(win_width, win_height, fb_width, fb_height);
        }
    }
}

//MARK: -
void ComponentsManager::addRenderer(Renderer *renderer) {
    renderer->_rendererIndex = _renderers.size();
    _renderers.push_back(renderer);
}

void ComponentsManager::removeRenderer(Renderer *renderer) {
    _renderers.erase(_renderers.begin() + renderer->_rendererIndex);
    renderer->_rendererIndex = -1;
}

void ComponentsManager::callRendererOnUpdate(float deltaTime) {
    for (size_t i = 0; i < _renderers.size(); i++) {
        _renderers[i]->update(deltaTime);
    }
}

void ComponentsManager::callRender(Camera *camera,
                                   std::vector<RenderElement> &opaqueQueue,
                                   std::vector<RenderElement> &alphaTestQueue,
                                   std::vector<RenderElement> &transparentQueue) {
    for (size_t i = 0; i < _renderers.size(); i++) {
        const auto &element = _renderers[i];

        // filter by camera culling mask.
        if (!(camera->cullingMask & element->_entity->layer)) {
            continue;
        }

        // filter by camera frustum.
        if (camera->enableFrustumCulling) {
            element->isCulled = !camera->_frustum.intersectsBox(element->bounds());
            if (element->isCulled) {
                continue;
            }
        }

        const auto &transform = camera->entity()->transform;
        const auto position = transform->worldPosition();
        auto center = element->bounds().midPoint();
        if (camera->isOrthographic()) {
            const auto forward = transform->worldForward();
            const auto offset = center - position;
            element->setDistanceForSort(offset.dot(forward));
        } else {
            element->setDistanceForSort(center.distanceSquaredTo(position));
        }

        element->_render(opaqueQueue, alphaTestQueue, transparentQueue);
    }
}

void ComponentsManager::callRender(const BoundingFrustum &frustrum,
                                   std::vector<RenderElement> &opaqueQueue,
                                   std::vector<RenderElement> &alphaTestQueue,
                                   std::vector<RenderElement> &transparentQueue) {
    for (size_t i = 0; i < _renderers.size(); i++) {
        const auto &renderer = _renderers[i];
        // filter by renderer castShadow and frustrum cull
        if (frustrum.intersectsBox(renderer->bounds())) {
            renderer->_render(opaqueQueue, alphaTestQueue, transparentQueue);
        }
    }
}

//MARK: -
void ComponentsManager::callCameraOnBeginRender(Camera *camera) {
    const auto &camComps = camera->entity()->_components;
    for (size_t i = 0; i < camComps.size(); i++) {
        const auto &camComp = camComps[i].get();
        auto pointer = dynamic_cast<Script *>(camComp);
        if (pointer != nullptr) {
            pointer->onBeginRender(camera);
        }
    }
}

void ComponentsManager::callCameraOnEndRender(Camera *camera) {
    const auto &camComps = camera->entity()->_components;
    for (size_t i = 0; i < camComps.size(); i++) {
        const auto &camComp = camComps[i].get();
        auto pointer = dynamic_cast<Script *>(camComp);
        if (pointer != nullptr) {
            pointer->onEndRender(camera);
        }
    }
}

std::vector<Component *> ComponentsManager::getActiveChangedTempList() {
    return _componentsContainerPool.size() ? *(_componentsContainerPool.end() - 1) : std::vector<Component *>{};
}

void ComponentsManager::putActiveChangedTempList(std::vector<Component *> &componentContainer) {
    componentContainer.clear();
    _componentsContainerPool.push_back(componentContainer);
}

}// namespace vox
