//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef sub_mesh_hpp
#define sub_mesh_hpp

#include <Metal/Metal.hpp>

namespace vox {
// App specific submesh class containing data to draw a submesh
struct SubMesh {
public:
    SubMesh(MTL::PrimitiveType primitiveType,
            MTL::IndexType indexType,
            NS::UInteger indexCount,
            const std::shared_ptr<MTL::Buffer> &indexBuffer);

    SubMesh(const SubMesh &rhs) = default;

    SubMesh(SubMesh &&rhs) = default;

    SubMesh &operator=(SubMesh &rhs) = default;

    SubMesh &operator=(SubMesh &&rhs) = default;

    ~SubMesh() = default;

    MTL::PrimitiveType primitiveType() const;

    MTL::IndexType indexType() const;

    NS::UInteger indexCount() const;

    const std::shared_ptr<MTL::Buffer> &indexBuffer() const;

private:
    MTL::PrimitiveType _primitiveType;

    MTL::IndexType _indexType;

    NS::UInteger _indexCount;

    std::shared_ptr<MTL::Buffer> _indexBuffer{nullptr};
};

inline MTL::PrimitiveType SubMesh::primitiveType() const {
    return _primitiveType;
}

inline MTL::IndexType SubMesh::indexType() const {
    return _indexType;
}

inline NS::UInteger SubMesh::indexCount() const {
    return _indexCount;
}

inline const std::shared_ptr<MTL::Buffer> &SubMesh::indexBuffer() const {
    return _indexBuffer;
}

}// namespace vox

#endif /* sub_mesh_hpp */
