//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "subpass.h"
#include "material/material.h"
#include "components/renderer.h"

namespace vox {
bool Subpass::_compareFromNearToFar(const RenderElement &a, const RenderElement &b) {
    return (a.material->renderQueueType < b.material->renderQueueType) ||
           (a.renderer->get_distance_for_sort() < b.renderer->get_distance_for_sort());
}

bool Subpass::_compareFromFarToNear(const RenderElement &a, const RenderElement &b) {
    return (a.material->renderQueueType < b.material->renderQueueType) ||
           (b.renderer->get_distance_for_sort() < a.renderer->get_distance_for_sort());
}

Subpass::Subpass(RenderContext *context,
                 Scene *scene,
                 Camera *camera)
    : _context(context),
      _scene(scene),
      _camera(camera) {
}

void Subpass::setRenderPass(RenderPass *pass) {
    _pass = pass;
    prepare();
}

}// namespace vox
