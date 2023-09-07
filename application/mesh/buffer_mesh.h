//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "mesh/mesh.h"

namespace vox {
class BufferMesh : public Mesh {
public:
    void setInstanceCount(uint32_t newValue);

    /**
     * Vertex buffer binding collection.
     */
    const std::vector<std::shared_ptr<MTL::Buffer>> &vertexBufferBindings();

    /**
     * Vertex layout collection.
     */
    const std::shared_ptr<MTL::VertexDescriptor> &vertexDescriptor();

    /**
     * Set vertex layouts.
     * @param layouts - Vertex layouts collection
     */
    void setVertexLayouts(const std::shared_ptr<MTL::VertexDescriptor> &layouts);

    /**
     * Set vertex buffer binding.
     * @param buffer - Vertex buffer binding
     * @param index - Vertex buffer index, the default value is 0
     */
    void setVertexBufferBinding(const std::shared_ptr<MTL::Buffer> &buffer, size_t index = 0);

    /**
     * Set vertex buffer binding.
     * @param vertexBufferBindings - Vertex buffer binding
     * @param firstIndex - First vertex buffer index, the default value is 0
     */
    void setVertexBufferBindings(const std::vector<std::shared_ptr<MTL::Buffer>> &vertexBufferBindings, size_t firstIndex = 0);
};

}// namespace vox