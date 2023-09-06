//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "render_element.h"

namespace vox {
RenderElement::RenderElement(Renderer *renderer, MeshPtr mesh,
                             const SubMesh *subMesh, MaterialPtr material) : renderer(renderer),
                                                                             mesh(mesh),
                                                                             subMesh(subMesh),
                                                                             material(material) {
}

}// namespace vox
