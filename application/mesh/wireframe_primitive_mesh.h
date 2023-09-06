//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef wireframe_primitive_mesh_hpp
#define wireframe_primitive_mesh_hpp

#include "model_mesh.h"
#include "vector3.h"

namespace vox {
/**
 * Used to generate common primitive meshes.
 */
class WireframePrimitiveMesh {
public:
    static ModelMeshPtr createCuboidWireFrame(MTL::Device &device, float width, float height, float depth);

    static ModelMeshPtr createSphereWireFrame(MTL::Device &device, float radius);

    static ModelMeshPtr createCapsuleWireFrame(MTL::Device &device, float radius, float height);

private:
    static void _createCircleWireFrame(float radius,
                                       uint32_t vertexBegin,
                                       uint32_t vertexCount,
                                       uint8_t axis, Vector3F shift,
                                       std::vector<Vector3F> &positions,
                                       std::vector<uint32_t> &indices);

    static void _createEllipticWireFrame(float radius, float height,
                                         uint32_t vertexBegin,
                                         uint32_t vertexCount,
                                         uint8_t axis,
                                         std::vector<Vector3F> &positions,
                                         std::vector<uint32_t> &indices);
};

}// namespace vox
#endif /* wireframe_primitive_mesh_hpp */
