//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "buffer_mesh.h"
#include <glog/logging.h>

namespace vox {
void BufferMesh::setInstanceCount(uint32_t newValue) {
    _instanceCount = newValue;
}

const std::vector<std::shared_ptr<MTL::Buffer>> &BufferMesh::vertexBufferBindings() {
    return _vertexBufferBindings;
}

const std::shared_ptr<MTL::VertexDescriptor> &BufferMesh::vertexDescriptor() {
    return _vertexDescriptor;
}

void BufferMesh::setVertexLayouts(const std::shared_ptr<MTL::VertexDescriptor> &descriptor) {
    _setVertexDescriptor(descriptor);
}

void BufferMesh::setVertexBufferBinding(const std::shared_ptr<MTL::Buffer> &buffer, size_t index) {
    if (_vertexBufferBindings.size() <= index) {
        _vertexBufferBindings.reserve(index + 1);
        for (size_t i = _vertexBufferBindings.size(); i < index; i++) {
            _vertexBufferBindings.push_back(nullptr);
        }
    }
    _setVertexBufferBinding(index, buffer);
}

void BufferMesh::setVertexBufferBindings(const std::vector<std::shared_ptr<MTL::Buffer>> &vertexBufferBindings, size_t firstIndex) {
    auto count = vertexBufferBindings.size();
    auto needLength = firstIndex + count;
    if (_vertexBufferBindings.size() < needLength) {
        _vertexBufferBindings.reserve(needLength);
        for (size_t i = _vertexBufferBindings.size(); i < needLength; i++) {
            _vertexBufferBindings.push_back(nullptr);
        }
    }
    for (size_t i = 0; i < count; i++) {
        _setVertexBufferBinding(firstIndex + i, vertexBufferBindings[i]);
    }
}

}// namespace vox
