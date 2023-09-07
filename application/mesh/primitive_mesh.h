//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "mesh/model_mesh.h"
#include "math/vector3.h"

namespace vox {
/**
 * Used to generate common primitive meshes.
 */
class PrimitiveMesh {
public:
    /**
     * Create a sphere mesh.
     * @param device - Engine
     * @param radius - Sphere radius
     * @param segments - Number of segments
     * @param noLongerAccessible - No longer access the vertices of the mesh after creation
     * @returns Sphere model mesh
     */
    static MeshPtr createSphere(MTL::Device &device,
                                float radius = 0.5,
                                size_t segments = 18,
                                bool noLongerAccessible = true);

    /**
     * Create a cuboid mesh.
     * @param device - Engine
     * @param width - Cuboid width
     * @param height - Cuboid height
     * @param depth - Cuboid depth
     * @param noLongerAccessible - No longer access the vertices of the mesh after creation
     * @returns Cuboid model mesh
     */
    static MeshPtr createCuboid(MTL::Device &device,
                                float width = 1,
                                float height = 1,
                                float depth = 1,
                                bool noLongerAccessible = true);

    /**
     * Create a plane mesh.
     * @param device - Engine
     * @param width - Plane width
     * @param height - Plane height
     * @param horizontalSegments - Plane horizontal segments
     * @param verticalSegments - Plane vertical segments
     * @param noLongerAccessible - No longer access the vertices of the mesh after creation
     * @returns Plane model mesh
     */
    static MeshPtr createPlane(MTL::Device &device,
                               float width = 1,
                               float height = 1,
                               size_t horizontalSegments = 1,
                               size_t verticalSegments = 1,
                               bool noLongerAccessible = true);

    /**
     * Create a cylinder mesh.
     * @param device - Engine
     * @param radiusTop - The radius of top cap
     * @param radiusBottom - The radius of bottom cap
     * @param height - The height of torso
     * @param radialSegments - Cylinder radial segments
     * @param heightSegments - Cylinder height segments
     * @param noLongerAccessible - No longer access the vertices of the mesh after creation
     * @returns Cylinder model mesh
     */
    static MeshPtr createCylinder(MTL::Device &device,
                                  float radiusTop = 0.5,
                                  float radiusBottom = 0.5,
                                  float height = 2,
                                  size_t radialSegments = 20,
                                  size_t heightSegments = 1,
                                  bool noLongerAccessible = true);

    /**
     * Create a torus mesh.
     * @param device - Engine
     * @param radius - Torus radius
     * @param tubeRadius - Torus tube
     * @param radialSegments - Torus radial segments
     * @param tubularSegments - Torus tubular segments
     * @param arc - Central angle
     * @param noLongerAccessible - No longer access the vertices of the mesh after creation
     * @returns Torus model mesh
     */
    static MeshPtr createTorus(MTL::Device &device,
                               float radius = 0.5,
                               float tubeRadius = 0.1,
                               size_t radialSegments = 30,
                               size_t tubularSegments = 30,
                               float arc = 360,
                               bool noLongerAccessible = true);

    /**
     * Create a cone mesh.
     * @param device - Engine
     * @param radius - The radius of cap
     * @param height - The height of torso
     * @param radialSegments - Cylinder radial segments
     * @param heightSegments - Cylinder height segments
     * @param noLongerAccessible - No longer access the vertices of the mesh after creation
     * @returns Cone model mesh
     */
    static MeshPtr createCone(MTL::Device &device,
                              float radius = 0.5,
                              float height = 2,
                              size_t radialSegments = 20,
                              size_t heightSegments = 1,
                              bool noLongerAccessible = true);

    /**
     * Create a capsule mesh.
     * @param device - Engine
     * @param radius - The radius of the two hemispherical ends
     * @param height - The height of the cylindrical part, measured between the centers of the hemispherical ends
     * @param radialSegments - Hemispherical end radial segments
     * @param heightSegments - Cylindrical part height segments
     * @param noLongerAccessible - No longer access the vertices of the mesh after creation
     * @returns Capsule model mesh
     */
    static MeshPtr createCapsule(MTL::Device &device,
                                 float radius = 0.5,
                                 float height = 2,
                                 size_t radialSegments = 6,
                                 size_t heightSegments = 1,
                                 bool noLongerAccessible = true);

private:
    static void _createCapsuleCap(float radius,
                                  float height,
                                  size_t radialSegments,
                                  float capAlphaRange,
                                  size_t offset,
                                  ssize_t posIndex,
                                  std::vector<Vector3F> &positions,
                                  std::vector<Vector3F> &normals,
                                  std::vector<Vector2F> &uvs,
                                  std::vector<uint32_t> &indices,
                                  size_t indicesOffset);

    static void _initialize(const ModelMeshPtr &mesh,
                            const std::vector<Vector3F> &positions,
                            const std::vector<Vector3F> &normals,
                            std::vector<Vector2F> &uvs,
                            const std::vector<uint32_t> &indices,
                            bool noLongerAccessible = true);
};

}// namespace vox