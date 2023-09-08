//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "math/vec.h"

namespace wp {
// ---------------------------------------------------------------------------------------
struct MarchingCubes {
    MarchingCubes() {
        memset(this, 0, sizeof(MarchingCubes));
    }

    __device__ __host__ int cell_index(int xi, int yi, int zi) const {
        return (xi * ny + yi) * nz + zi;
    }
    __device__ __host__ void cell_coord(int cell_index, int &xi, int &yi, int &zi) const {
        zi = cell_index % nz;
        cell_index /= nz;
        yi = cell_index % ny;
        xi = cell_index / ny;
    }

    // grid
    int nx{};
    int ny{};
    int nz{};

    int *first_cell_vert{};
    int *first_cell_tri{};
    int *cell_verts{};

    int num_cells{};
    int max_cells{};

    void *context{};
};
}// namespace wp

uint64_t marching_cubes_create_device(void *context);
void marching_cubes_destroy_device(wp::MarchingCubes *mc);
int marching_cubes_surface_device(wp::MarchingCubes &mc, const float *field, int nx, int ny, int nz, float threshold,
                                  wp::vec3 *verts, int *triangles, int max_verts, int max_tris, int *out_num_verts, int *out_num_tris);
