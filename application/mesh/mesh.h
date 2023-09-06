//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef mesh_hpp
#define mesh_hpp

#include "sub_mesh.h"
#include "bounding_box3.h"
#include "update_flag_manager.h"
#include <string>

namespace vox {
struct Mesh {
public:
    /** Name. */
    std::string name = "";
    /** The bounding volume of the mesh. */
    BoundingBox3F bounds = BoundingBox3F();

    /**
     * Instanced count, disable instanced drawing when set zero.
     */
    uint32_t instanceCount() const;

    /**
     * First sub-mesh. Rendered using the first material.
     */
    const SubMesh *subMesh() const;

    SubMesh *subMesh();

    /**
     * A collection of sub-mesh, each sub-mesh can be rendered with an independent material.
     */
    const std::vector<SubMesh> &subMeshes() const;

    /**
     * Add sub-mesh, each sub-mesh can correspond to an independent material.
     * @param subMesh - Start drawing offset, if the index buffer is set,
     * it means the offset in the index buffer, if not set, it means the offset in the vertex buffer
     */
    void addSubMesh(SubMesh subMesh);

    /**
     * Add sub-mesh, each sub-mesh can correspond to an independent material.
     * @param indexCount - Drawing count, if the index buffer is set,
     * it means the count in the index buffer, if not set,
     * it means the count in the vertex buffer
     * @param primitiveType - Drawing topology, default is MeshTopology.Triangles
     */
    void addSubMesh(MTL::PrimitiveType primitiveType,
                    MTL::IndexType indexType,
                    NS::UInteger indexCount,
                    const std::shared_ptr<MTL::Buffer> &indexBuffer);

    /**
     * Clear all sub-mesh.
     */
    void clearSubMesh();

    /**
     * Register update flag, update flag will be true if the vertex element changes.
     * @returns Update flag
     */
    std::unique_ptr<UpdateFlag> registerUpdateFlag();

public:
    const std::vector<std::shared_ptr<MTL::Buffer>> &vertexBufferBindings() const;

    const std::shared_ptr<MTL::VertexDescriptor> &vertexDescriptor() const;

protected:
    void _setVertexDescriptor(const std::shared_ptr<MTL::VertexDescriptor> &descriptor);

    void _setVertexBufferBinding(size_t index, const std::shared_ptr<MTL::Buffer> &binding);

protected:
    std::vector<SubMesh> _subMeshes;
    UpdateFlagManager _updateFlagManager = UpdateFlagManager();

    std::vector<std::shared_ptr<MTL::Buffer>> _vertexBufferBindings;

    std::shared_ptr<MTL::VertexDescriptor> _vertexDescriptor;

    uint32_t _instanceCount = 1;
};

using MeshPtr = std::shared_ptr<Mesh>;

}// namespace vox

#endif /* mesh_hpp */
