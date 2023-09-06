//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "mesh.h"

namespace vox {
uint32_t Mesh::instanceCount() const {
    return _instanceCount;
}

const SubMesh *Mesh::subMesh() const {
    if (_subMeshes.size() > 0) {
        return &_subMeshes[0];
    } else {
        return nullptr;
    }
}

SubMesh *Mesh::subMesh() {
    if (_subMeshes.size() > 0) {
        return &_subMeshes[0];
    } else {
        return nullptr;
    }
}

const std::vector<SubMesh> &Mesh::subMeshes() const {
    return _subMeshes;
}

void Mesh::addSubMesh(SubMesh subMesh) {
    _subMeshes.push_back(subMesh);
}

void Mesh::addSubMesh(MTL::PrimitiveType primitiveType,
                      MTL::IndexType indexType,
                      NS::UInteger indexCount,
                      const std::shared_ptr<MTL::Buffer> &indexBuffer) {
    _subMeshes.push_back(SubMesh(primitiveType, indexType, indexCount, indexBuffer));
}

void Mesh::clearSubMesh() {
    _subMeshes.clear();
}

std::unique_ptr<UpdateFlag> Mesh::registerUpdateFlag() {
    return _updateFlagManager.registration();
}

void Mesh::_setVertexDescriptor(const std::shared_ptr<MTL::VertexDescriptor> &descriptor) {
    _vertexDescriptor = descriptor;
}

void Mesh::_setVertexBufferBinding(size_t index, const std::shared_ptr<MTL::Buffer> &binding) {
    _vertexBufferBindings.insert(_vertexBufferBindings.begin() + index, binding);
}

const std::vector<std::shared_ptr<MTL::Buffer>> &Mesh::vertexBufferBindings() const {
    return _vertexBufferBindings;
}

const std::shared_ptr<MTL::VertexDescriptor> &Mesh::vertexDescriptor() const {
    return _vertexDescriptor;
}

}// namespace vox
