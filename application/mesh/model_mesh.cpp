//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "model_mesh.h"
#include "shader/shader_common.h"
#include "framework/common/metal_helpers.h"

namespace vox {
bool ModelMesh::accessible() {
    return _accessible;
}

size_t ModelMesh::vertexCount() {
    return _vertexCount;
}

ModelMesh::ModelMesh(MTL::Device &device, const std::string &name) : Mesh(),
                                                                     _device(device) {
}

void ModelMesh::setPositions(const std::vector<Vector3F> &positions) {
    if (!_accessible) {
        assert(false && "Not allowed to access data while accessible is false.");
    }

    auto count = positions.size();
    _positions = positions;
    _vertexChangeFlag |= ValueChanged::Position;

    if (_vertexCount != count) {
        _vertexCount = count;
    }
}

const std::vector<Vector3F> &ModelMesh::positions() {
    if (!_accessible) {
        assert(false && "Not allowed to access data while accessible is false.");
    }

    return _positions;
}

void ModelMesh::setNormals(const std::vector<Vector3F> &normals) {
    if (!_accessible) {
        assert(false && "Not allowed to access data while accessible is false.");
    }

    if (normals.size() != _vertexCount) {
        assert(false && "The array provided needs to be the same size as vertex count.");
    }

    _vertexChangeFlag |= ValueChanged::Normal;
    _normals = normals;
}

const std::vector<Vector3F> &ModelMesh::normals() {
    if (!_accessible) {
        assert(false && "Not allowed to access data while accessible is false.");
    }
    return _normals;
}

void ModelMesh::setColors(const std::vector<Color> &colors) {
    if (!_accessible) {
        assert(false && "Not allowed to access data while accessible is false.");
    }

    if (colors.size() != _vertexCount) {
        assert(false && "The array provided needs to be the same size as vertex count.");
    }

    _vertexChangeFlag |= ValueChanged::Color;
    _colors = colors;
}

const std::vector<Color> &ModelMesh::colors() {
    if (!_accessible) {
        assert(false && "Not allowed to access data while accessible is false.");
    }
    return _colors;
}

void ModelMesh::setTangents(const std::vector<Vector4F> &tangents) {
    if (!_accessible) {
        assert(false && "Not allowed to access data while accessible is false.");
    }

    if (tangents.size() != _vertexCount) {
        assert(false && "The array provided needs to be the same size as vertex count.");
    }

    _vertexChangeFlag |= ValueChanged::Tangent;
    _tangents = tangents;
}

const std::vector<Vector4F> &ModelMesh::tangents() {
    if (!_accessible) {
        assert(false && "Not allowed to access data while accessible is false.");
    }
    return _tangents;
}

void ModelMesh::setUVs(const std::vector<Vector2F> &uv, int channelIndex) {
    if (!_accessible) {
        assert(false && "Not allowed to access data while accessible is false.");
    }

    if (uv.size() != _vertexCount) {
        assert(false && "The array provided needs to be the same size as vertex count.");
    }

    switch (channelIndex) {
        case 0:
            _vertexChangeFlag |= ValueChanged::UV;
            _uv = uv;
            break;
        case 1:
            _vertexChangeFlag |= ValueChanged::UV1;
            _uv1 = uv;
            break;
        case 2:
            _vertexChangeFlag |= ValueChanged::UV2;
            _uv2 = uv;
            break;
        case 3:
            _vertexChangeFlag |= ValueChanged::UV3;
            _uv3 = uv;
            break;
        case 4:
            _vertexChangeFlag |= ValueChanged::UV4;
            _uv4 = uv;
            break;
        case 5:
            _vertexChangeFlag |= ValueChanged::UV5;
            _uv5 = uv;
            break;
        case 6:
            _vertexChangeFlag |= ValueChanged::UV6;
            _uv6 = uv;
            break;
        case 7:
            _vertexChangeFlag |= ValueChanged::UV7;
            _uv7 = uv;
            break;
        default:
            assert(false && "The index of channel needs to be in range [0 - 7].");
    }
}

const std::vector<Vector2F> &ModelMesh::uvs(int channelIndex) {
    if (!_accessible) {
        assert(false && "Not allowed to access data while accessible is false.");
    }
    switch (channelIndex) {
        case 0:
            return _uv;
        case 1:
            return _uv1;
        case 2:
            return _uv2;
        case 3:
            return _uv3;
        case 4:
            return _uv4;
        case 5:
            return _uv5;
        case 6:
            return _uv6;
        case 7:
            return _uv7;
        default:
            assert(false && "The index of channel needs to be in range [0 - 7].");
            throw std::exception();
    }
}

void ModelMesh::setIndices(const std::vector<uint32_t> &indices, MTL::PrimitiveType type) {
    if (!_accessible) {
        assert(false && "Not allowed to access data while accessible is false.");
    }
    _indices = indices;
    _primitiveType = type;
}

const std::vector<uint32_t> ModelMesh::indices() {
    return _indices;
}

void ModelMesh::uploadData(bool noLongerAccessible) {
    if (!_accessible) {
        assert(false && "Not allowed to access data while accessible is false.");
    }

    _vertexDescriptor = _updateVertexDescriptor();
    _vertexChangeFlag = ValueChanged::All;

    auto vertexFloatCount = _elementCount * _vertexCount;
    auto vertices = std::vector<float>(vertexFloatCount);
    _updateVertices(vertices);

    auto newVertexBuffer = CLONE_METAL_CUSTOM_DELETER(MTL::Buffer, _device.newBuffer(vertices.data(),
                                                                                     vertexFloatCount * sizeof(float),
                                                                                     MTL::ResourceOptionCPUCacheModeDefault));
    _setVertexBufferBinding(0, newVertexBuffer);

    const auto indexBuffer = CLONE_METAL_CUSTOM_DELETER(MTL::Buffer, _device.newBuffer(_indices.data(),
                                                                                       _indices.size() * sizeof(uint32_t),
                                                                                       MTL::ResourceOptionCPUCacheModeDefault));
    addSubMesh(_primitiveType, MTL::IndexTypeUInt32, _indices.size(), indexBuffer);

    if (noLongerAccessible) {
        _accessible = false;
        _releaseCache();
    }
}

std::shared_ptr<MTL::VertexDescriptor> ModelMesh::_updateVertexDescriptor() {
    auto descriptor = CLONE_METAL_CUSTOM_DELETER(MTL::VertexDescriptor, MTL::VertexDescriptor::alloc()->init());
    descriptor->attributes()->object(Attributes::Position)->setFormat(MTL::VertexFormatFloat3);
    descriptor->attributes()->object(Attributes::Position)->setOffset(0);
    descriptor->attributes()->object(Attributes::Position)->setBufferIndex(0);

    size_t offset = 12;
    size_t elementCount = 3;
    if (!_normals.empty()) {
        descriptor->attributes()->object(Attributes::Normal)->setFormat(MTL::VertexFormatFloat3);
        descriptor->attributes()->object(Attributes::Normal)->setOffset(offset);
        descriptor->attributes()->object(Attributes::Normal)->setBufferIndex(0);
        offset += sizeof(float) * 3;
        elementCount += 3;
    }

    if (!_colors.empty()) {
        descriptor->attributes()->object(Attributes::Color_0)->setFormat(MTL::VertexFormatFloat4);
        descriptor->attributes()->object(Attributes::Color_0)->setOffset(offset);
        descriptor->attributes()->object(Attributes::Color_0)->setBufferIndex(0);
        offset += sizeof(float) * 4;
        elementCount += 4;
    }
    if (!_boneWeights.empty()) {
        descriptor->attributes()->object(Attributes::Weights_0)->setFormat(MTL::VertexFormatFloat4);
        descriptor->attributes()->object(Attributes::Weights_0)->setOffset(offset);
        descriptor->attributes()->object(Attributes::Weights_0)->setBufferIndex(0);
        offset += sizeof(float) * 4;
        elementCount += 4;
    }
    if (!_boneIndices.empty()) {
        descriptor->attributes()->object(Attributes::Joints_0)->setFormat(MTL::VertexFormatShort4);
        descriptor->attributes()->object(Attributes::Joints_0)->setOffset(offset);
        descriptor->attributes()->object(Attributes::Joints_0)->setBufferIndex(0);
        offset += sizeof(short) * 4;
        elementCount += 1;
    }
    if (!_tangents.empty()) {
        descriptor->attributes()->object(Attributes::Tangent)->setFormat(MTL::VertexFormatFloat4);
        descriptor->attributes()->object(Attributes::Tangent)->setOffset(offset);
        descriptor->attributes()->object(Attributes::Tangent)->setBufferIndex(0);
        offset += sizeof(float) * 4;
        elementCount += 4;
    }
    if (!_uv.empty()) {
        descriptor->attributes()->object(Attributes::UV_0)->setFormat(MTL::VertexFormatFloat2);
        descriptor->attributes()->object(Attributes::UV_0)->setOffset(offset);
        descriptor->attributes()->object(Attributes::UV_0)->setBufferIndex(0);
        offset += sizeof(float) * 2;
        elementCount += 2;
    }
    if (!_uv1.empty()) {
        descriptor->attributes()->object(Attributes::UV_1)->setFormat(MTL::VertexFormatFloat2);
        descriptor->attributes()->object(Attributes::UV_1)->setOffset(offset);
        descriptor->attributes()->object(Attributes::UV_1)->setBufferIndex(0);
        offset += sizeof(float) * 2;
        elementCount += 2;
    }
    if (!_uv2.empty()) {
        descriptor->attributes()->object(Attributes::UV_2)->setFormat(MTL::VertexFormatFloat2);
        descriptor->attributes()->object(Attributes::UV_2)->setOffset(offset);
        descriptor->attributes()->object(Attributes::UV_2)->setBufferIndex(0);
        offset += sizeof(float) * 2;
        elementCount += 2;
    }
    if (!_uv3.empty()) {
        descriptor->attributes()->object(Attributes::UV_3)->setFormat(MTL::VertexFormatFloat2);
        descriptor->attributes()->object(Attributes::UV_3)->setOffset(offset);
        descriptor->attributes()->object(Attributes::UV_3)->setBufferIndex(0);
        offset += sizeof(float) * 2;
        elementCount += 2;
    }
    if (!_uv4.empty()) {
        descriptor->attributes()->object(Attributes::UV_4)->setFormat(MTL::VertexFormatFloat2);
        descriptor->attributes()->object(Attributes::UV_4)->setOffset(offset);
        descriptor->attributes()->object(Attributes::UV_4)->setBufferIndex(0);
        offset += sizeof(float) * 2;
        elementCount += 2;
    }
    if (!_uv5.empty()) {
        descriptor->attributes()->object(Attributes::UV_5)->setFormat(MTL::VertexFormatFloat2);
        descriptor->attributes()->object(Attributes::UV_5)->setOffset(offset);
        descriptor->attributes()->object(Attributes::UV_5)->setBufferIndex(0);
        offset += sizeof(float) * 2;
        elementCount += 2;
    }
    if (!_uv6.empty()) {
        descriptor->attributes()->object(Attributes::UV_6)->setFormat(MTL::VertexFormatFloat2);
        descriptor->attributes()->object(Attributes::UV_6)->setOffset(offset);
        descriptor->attributes()->object(Attributes::UV_6)->setBufferIndex(0);
        offset += sizeof(float) * 2;
        elementCount += 2;
    }
    if (!_uv7.empty()) {
        descriptor->attributes()->object(Attributes::UV_7)->setFormat(MTL::VertexFormatFloat2);
        descriptor->attributes()->object(Attributes::UV_7)->setOffset(offset);
        descriptor->attributes()->object(Attributes::UV_7)->setBufferIndex(0);
        offset += sizeof(float) * 2;
        elementCount += 2;
    }
    descriptor->layouts()->object(0)->setStride(offset);

    _elementCount = elementCount;
    return descriptor;

    return descriptor;
}

void ModelMesh::_updateVertices(std::vector<float> &vertices) {
    if ((_vertexChangeFlag & ValueChanged::Position) != 0) {
        for (size_t i = 0; i < _vertexCount; i++) {
            auto start = _elementCount * i;
            const auto &position = _positions[i];
            vertices[start] = position.x;
            vertices[start + 1] = position.y;
            vertices[start + 2] = position.z;
        }
    }

    size_t offset = 3;

    if (!_normals.empty()) {
        if ((_vertexChangeFlag & ValueChanged::Normal) != 0) {
            for (size_t i = 0; i < _vertexCount; i++) {
                auto start = _elementCount * i + offset;
                const auto &normal = _normals[i];
                vertices[start] = normal.x;
                vertices[start + 1] = normal.y;
                vertices[start + 2] = normal.z;
            }
        }
        offset += 3;
    }

    if (!_colors.empty()) {
        if ((_vertexChangeFlag & ValueChanged::Color) != 0) {
            for (size_t i = 0; i < _vertexCount; i++) {
                auto start = _elementCount * i + offset;
                const auto &color = _colors[i];
                vertices[start] = color.r;
                vertices[start + 1] = color.g;
                vertices[start + 2] = color.b;
                vertices[start + 3] = color.a;
            }
        }
        offset += 4;
    }

    if (!_tangents.empty()) {
        if ((_vertexChangeFlag & ValueChanged::Tangent) != 0) {
            for (size_t i = 0; i < _vertexCount; i++) {
                auto start = _elementCount * i + offset;
                const auto &tangent = _tangents[i];
                vertices[start] = tangent.x;
                vertices[start + 1] = tangent.y;
                vertices[start + 2] = tangent.z;
            }
        }
        offset += 4;
    }
    if (!_uv.empty()) {
        if ((_vertexChangeFlag & ValueChanged::UV) != 0) {
            for (size_t i = 0; i < _vertexCount; i++) {
                auto start = _elementCount * i + offset;
                const auto &uv = _uv[i];
                vertices[start] = uv.x;
                vertices[start + 1] = uv.y;
            }
        }
        offset += 2;
    }
    if (!_uv1.empty()) {
        if ((_vertexChangeFlag & ValueChanged::UV1) != 0) {
            for (size_t i = 0; i < _vertexCount; i++) {
                auto start = _elementCount * i + offset;
                const auto &uv = _uv1[i];
                vertices[start] = uv.x;
                vertices[start + 1] = uv.y;
            }
        }
        offset += 2;
    }
    if (!_uv2.empty()) {
        if ((_vertexChangeFlag & ValueChanged::UV2) != 0) {
            for (size_t i = 0; i < _vertexCount; i++) {
                auto start = _elementCount * i + offset;
                const auto &uv = _uv2[i];
                vertices[start] = uv.x;
                vertices[start + 1] = uv.y;
            }
        }
        offset += 2;
    }
    if (!_uv3.empty()) {
        if ((_vertexChangeFlag & ValueChanged::UV3) != 0) {
            for (size_t i = 0; i < _vertexCount; i++) {
                auto start = _elementCount * i + offset;
                const auto &uv = _uv3[i];
                vertices[start] = uv.x;
                vertices[start + 1] = uv.y;
            }
        }
        offset += 2;
    }
    if (!_uv4.empty()) {
        if ((_vertexChangeFlag & ValueChanged::UV4) != 0) {
            for (size_t i = 0; i < _vertexCount; i++) {
                auto start = _elementCount * i + offset;
                const auto &uv = _uv4[i];
                vertices[start] = uv.x;
                vertices[start + 1] = uv.y;
            }
        }
        offset += 2;
    }
    if (!_uv5.empty()) {
        if ((_vertexChangeFlag & ValueChanged::UV5) != 0) {
            for (size_t i = 0; i < _vertexCount; i++) {
                auto start = _elementCount * i + offset;
                const auto &uv = _uv5[i];
                vertices[start] = uv.x;
                vertices[start + 1] = uv.y;
            }
        }
        offset += 2;
    }
    if (!_uv6.empty()) {
        if ((_vertexChangeFlag & ValueChanged::UV6) != 0) {
            for (size_t i = 0; i < _vertexCount; i++) {
                auto start = _elementCount * i + offset;
                const auto &uv = _uv6[i];
                vertices[start] = uv.x;
                vertices[start + 1] = uv.y;
            }
        }
        offset += 2;
    }
    if (!_uv7.empty()) {
        if ((_vertexChangeFlag & ValueChanged::UV7) != 0) {
            for (size_t i = 0; i < _vertexCount; i++) {
                auto start = _elementCount * i + offset;
                const auto &uv = _uv7[i];
                vertices[start] = uv.x;
                vertices[start + 1] = uv.y;
            }
        }
        offset += 2;
    }

    _vertexChangeFlag = 0;
}

void ModelMesh::_releaseCache() {
    _vertices.clear();
    _positions.clear();
    _tangents.clear();
    _normals.clear();
    _colors.clear();
    _uv.clear();
    _uv1.clear();
    _uv2.clear();
    _uv3.clear();
    _uv4.clear();
    _uv5.clear();
    _uv6.clear();
    _uv7.clear();
}

}// namespace vox
