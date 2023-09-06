//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "sub_mesh.h"

namespace vox {
// Initialize a submesh without textures
SubMesh::SubMesh(MTL::PrimitiveType primitiveType,
                 MTL::IndexType indexType,
                 NS::UInteger indexCount,
                 const std::shared_ptr<MTL::Buffer> &indexBuffer)
    : _primitiveType(primitiveType), _indexType(indexType),
      _indexCount(indexCount), _indexBuffer(indexBuffer) {
}

}// namespace vox
