//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "renderer.h"

namespace vox {
class MeshRenderer : public Renderer {
public:
    using MeshPtr = std::shared_ptr<Mesh>;

    explicit MeshRenderer(Entity *entity);

    /**
     * Mesh assigned to the renderer.
     */
    void setMesh(const MeshPtr &mesh);

    MeshPtr mesh();

private:
    void render(std::vector<RenderElement> &opaqueQueue,
                std::vector<RenderElement> &alphaTestQueue,
                std::vector<RenderElement> &transparentQueue) override;

    void update_bounds(BoundingBox3F &worldBounds) override;

private:
    MeshPtr _mesh;
    std::unique_ptr<UpdateFlag> _meshUpdateFlag;
};

}// namespace vox