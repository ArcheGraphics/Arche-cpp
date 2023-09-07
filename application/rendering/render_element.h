//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <memory>

namespace vox {
class Renderer;
class Mesh;
class SubMesh;
class Material;

/**
 * Render element.
 */
struct RenderElement {
    using MeshPtr = std::shared_ptr<Mesh>;
    using MaterialPtr = std::shared_ptr<Material>;

    /** Render component. */
    Renderer *renderer;
    /** Mesh. */
    MeshPtr mesh;
    /** Sub mesh. */
    const SubMesh *sub_mesh;
    /** Material. */
    MaterialPtr material;

    RenderElement(Renderer *renderer, MeshPtr mesh, const SubMesh *sub_mesh, MaterialPtr material);
};

}// namespace vox
