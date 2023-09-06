//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "wireframe_primitive_mesh.h"

namespace vox {
ModelMeshPtr WireframePrimitiveMesh::createCuboidWireFrame(MTL::Device &device, float width, float height, float depth) {
    auto mesh = std::make_shared<ModelMesh>(device);

    auto halfWidth = width / 2;
    auto halfHeight = height / 2;
    auto halfDepth = depth / 2;

    auto positions = std::vector<Vector3F>(24);
    // Up
    positions[0] = Vector3F(-halfWidth, halfHeight, -halfDepth);
    positions[1] = Vector3F(halfWidth, halfHeight, -halfDepth);
    positions[2] = Vector3F(halfWidth, halfHeight, halfDepth);
    positions[3] = Vector3F(-halfWidth, halfHeight, halfDepth);

    // Down
    positions[4] = Vector3F(-halfWidth, -halfHeight, -halfDepth);
    positions[5] = Vector3F(halfWidth, -halfHeight, -halfDepth);
    positions[6] = Vector3F(halfWidth, -halfHeight, halfDepth);
    positions[7] = Vector3F(-halfWidth, -halfHeight, halfDepth);

    // Left
    positions[8] = Vector3F(-halfWidth, halfHeight, -halfDepth);
    positions[9] = Vector3F(-halfWidth, halfHeight, halfDepth);
    positions[10] = Vector3F(-halfWidth, -halfHeight, halfDepth);
    positions[11] = Vector3F(-halfWidth, -halfHeight, -halfDepth);

    // Right
    positions[12] = Vector3F(halfWidth, halfHeight, -halfDepth);
    positions[13] = Vector3F(halfWidth, halfHeight, halfDepth);
    positions[14] = Vector3F(halfWidth, -halfHeight, halfDepth);
    positions[15] = Vector3F(halfWidth, -halfHeight, -halfDepth);

    // Front
    positions[16] = Vector3F(-halfWidth, halfHeight, halfDepth);
    positions[17] = Vector3F(halfWidth, halfHeight, halfDepth);
    positions[18] = Vector3F(halfWidth, -halfHeight, halfDepth);
    positions[19] = Vector3F(-halfWidth, -halfHeight, halfDepth);

    // Back
    positions[20] = Vector3F(-halfWidth, halfHeight, -halfDepth);
    positions[21] = Vector3F(halfWidth, halfHeight, -halfDepth);
    positions[22] = Vector3F(halfWidth, -halfHeight, -halfDepth);
    positions[23] = Vector3F(-halfWidth, -halfHeight, -halfDepth);

    auto indices = std::vector<uint32_t>(48);
    // Up
    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 1;
    indices[3] = 2;
    indices[4] = 2;
    indices[5] = 3;
    indices[6] = 3;
    indices[7] = 0;

    // Down
    indices[8] = 4;
    indices[9] = 5;
    indices[10] = 5;
    indices[11] = 6;
    indices[12] = 6;
    indices[13] = 7;
    indices[14] = 7;
    indices[15] = 4;
    // Left
    indices[16] = 8;
    indices[17] = 9;
    indices[18] = 9;
    indices[19] = 10;
    indices[20] = 10;
    indices[21] = 11;
    indices[22] = 11;
    indices[23] = 8;
    // Right
    indices[24] = 12;
    indices[25] = 13;
    indices[26] = 13;
    indices[27] = 14;
    indices[28] = 14;
    indices[29] = 15;
    indices[30] = 15;
    indices[31] = 12;
    // Front
    indices[32] = 16;
    indices[33] = 17;
    indices[34] = 17;
    indices[35] = 18;
    indices[36] = 18;
    indices[37] = 19;
    indices[38] = 19;
    indices[39] = 16;
    // Back
    indices[40] = 20;
    indices[41] = 21;
    indices[42] = 21;
    indices[43] = 22;
    indices[44] = 22;
    indices[45] = 23;
    indices[46] = 23;
    indices[47] = 20;

    mesh->setPositions(positions);
    mesh->setIndices(indices, MTL::PrimitiveTypeLine);
    mesh->uploadData(true);

    return mesh;
}

ModelMeshPtr WireframePrimitiveMesh::createSphereWireFrame(MTL::Device &device, float radius) {
    auto mesh = std::make_shared<ModelMesh>(device);

    auto vertexCount = 40;
    auto shift = Vector3F();

    auto positions = std::vector<Vector3F>(vertexCount * 3);
    auto indices = std::vector<uint32_t>(vertexCount * 6);
    // X
    _createCircleWireFrame(radius, 0, vertexCount, 0, shift, positions, indices);

    // Y
    _createCircleWireFrame(radius, vertexCount, vertexCount, 1, shift, positions, indices);

    // Z
    _createCircleWireFrame(radius, 2 * vertexCount, vertexCount, 2, shift, positions, indices);

    mesh->setPositions(positions);
    mesh->setIndices(indices, MTL::PrimitiveTypeLine);
    mesh->uploadData(true);

    return mesh;
}

ModelMeshPtr WireframePrimitiveMesh::createCapsuleWireFrame(MTL::Device &device, float radius, float height) {
    auto mesh = std::make_shared<ModelMesh>(device);

    auto vertexCount = 40;
    auto shift = Vector3F();
    auto halfHeight = height / 2;
    auto positions = std::vector<Vector3F>(vertexCount * 4);
    auto indices = std::vector<uint32_t>(vertexCount * 8);

    // Y-Top
    shift.y = halfHeight;
    _createCircleWireFrame(radius, 0, vertexCount, 1, shift, positions, indices);

    // Y-Bottom
    shift.y = -halfHeight;
    _createCircleWireFrame(radius, vertexCount, vertexCount, 1, shift, positions, indices);

    // X-Elliptic
    _createEllipticWireFrame(radius, halfHeight, vertexCount * 2, vertexCount, 2, positions, indices);

    // Z-Elliptic
    _createEllipticWireFrame(radius, halfHeight, vertexCount * 3, vertexCount, 0, positions, indices);

    mesh->setPositions(positions);
    mesh->setIndices(indices, MTL::PrimitiveTypeLine);
    mesh->uploadData(true);

    return mesh;
}

void WireframePrimitiveMesh::_createCircleWireFrame(float radius,
                                                    uint32_t vertexBegin,
                                                    uint32_t vertexCount,
                                                    uint8_t axis,
                                                    Vector3F shift,
                                                    std::vector<Vector3F> &positions,
                                                    std::vector<uint32_t> &indices) {
    auto countReciprocal = 1.0 / vertexCount;
    for (uint32_t i = 0; i < vertexCount; ++i) {
        float v = i * countReciprocal;
        float thetaDelta = v * M_PI * 2;

        uint32_t globalIndex = i + vertexBegin;
        switch (axis) {
            case 0:
                positions[globalIndex] = Vector3F(shift.x,
                                                  radius * std::cos(thetaDelta) + shift.y,
                                                  radius * std::sin(thetaDelta) + shift.z);
                break;
            case 1:
                positions[globalIndex] = Vector3F(radius * std::cos(thetaDelta) + shift.x,
                                                  shift.y,
                                                  radius * std::sin(thetaDelta) + shift.z);
                break;
            case 2:
                positions[globalIndex] = Vector3F(radius * std::cos(thetaDelta) + shift.x,
                                                  radius * std::sin(thetaDelta) + shift.y,
                                                  shift.z);
                break;
        }

        if (i < vertexCount - 1) {
            indices[2 * globalIndex] = globalIndex;
            indices[2 * globalIndex + 1] = globalIndex + 1;
        } else {
            indices[2 * globalIndex] = globalIndex;
            indices[2 * globalIndex + 1] = vertexBegin;
        }
    }
}

void WireframePrimitiveMesh::_createEllipticWireFrame(float radius,
                                                      float height,
                                                      uint32_t vertexBegin,
                                                      uint32_t vertexCount,
                                                      uint8_t axis,
                                                      std::vector<Vector3F> &positions,
                                                      std::vector<uint32_t> &indices) {
    float countReciprocal = 1.0 / vertexCount;
    for (uint32_t i = 0; i < vertexCount; ++i) {
        float v = i * countReciprocal;
        float thetaDelta = v * M_PI * 2;

        uint32_t globalIndex = i + vertexBegin;
        switch (axis) {
            case 0:
                positions[globalIndex] = Vector3F(0,
                                                  radius * std::sin(thetaDelta) + height,
                                                  radius * std::cos(thetaDelta));
                break;
            case 1:
                positions[globalIndex] = Vector3F(radius * std::cos(thetaDelta), height, radius * std::sin(thetaDelta));
                break;
            case 2:
                positions[globalIndex] = Vector3F(radius * std::cos(thetaDelta),
                                                  radius * std::sin(thetaDelta) + height,
                                                  0);
                break;
        }

        if (i == vertexCount / 2) {
            height = -height;
        }

        if (i < vertexCount - 1) {
            indices[2 * globalIndex] = globalIndex;
            indices[2 * globalIndex + 1] = globalIndex + 1;
        } else {
            indices[2 * globalIndex] = globalIndex;
            indices[2 * globalIndex + 1] = vertexBegin;
        }
    }
}
}// namespace vox
