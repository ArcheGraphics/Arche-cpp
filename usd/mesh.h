//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <cgltf.h>

#include <pxr/base/vt/array.h>

using namespace PXR_NS;

namespace vox {
bool createGeometryRepresentation(const cgltf_primitive *prim,
                                  const VtIntArray &inIndices,
                                  VtIntArray &outIndices,
                                  VtIntArray &faceVertexCounts);

void createFlatNormals(const VtIntArray &indices,
                       const VtVec3fArray &positions,
                       VtVec3fArray &normals);

bool createTangents(const VtIntArray &indices,
                    const VtVec3fArray &positions,
                    const VtVec3fArray &normals,
                    const VtVec2fArray &texcoords,
                    VtFloatArray &signs,
                    VtVec3fArray &tangents);
}// namespace vox
