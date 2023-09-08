//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "cuda_context.h"
#include "cuda_util.h"
#include "hashgrid.h"
#include "sort.h"

namespace wp {
// convert a virtual (world) cell coordinate to a physical one
__device__ int hash_grid_index(const HashGrid &grid, int x, int y, int z) {
    // offset to ensure positive coordinates (means grid dim should be less than 4096^3)
    const int origin = 1 << 20;

    x += origin;
    y += origin;
    z += origin;

    assert(0 < x);
    assert(0 < y);
    assert(0 < z);

    // clamp in case any particles fall outside the guard region (-10^20 cell index)
    x = ::max(0, x);
    y = ::max(0, y);
    z = ::max(0, z);

    // compute physical cell (assume pow2 grid dims)
    // int cx = x & (grid.dim_x-1);
    // int cy = y & (grid.dim_y-1);
    // int cz = z & (grid.dim_z-1);

    // compute physical cell (arbitrary grid dims)
    int cx = x % grid.dim_x;
    int cy = y % grid.dim_y;
    int cz = z % grid.dim_z;

    return cz * (grid.dim_x * grid.dim_y) + cy * grid.dim_x + cx;
}

__device__ hash_grid_query_t hash_grid_query(const HashGrid *grid, wp::vec3 pos, float radius) {
    hash_grid_query_t query;

    query.grid = *grid;

    // convert coordinate to grid
    query.x_start = int((pos[0] - radius) * query.grid.cell_width_inv);
    query.y_start = int((pos[1] - radius) * query.grid.cell_width_inv);
    query.z_start = int((pos[2] - radius) * query.grid.cell_width_inv);

    // do not want to visit any cells more than once, so limit large radius offset to one pass over each dimension
    query.x_end = ::min(int32_t((pos[0] + radius) * query.grid.cell_width_inv), query.x_start + query.grid.dim_x - 1);
    query.y_end = ::min(int32_t((pos[1] + radius) * query.grid.cell_width_inv), query.y_start + query.grid.dim_y - 1);
    query.z_end = ::min(int32_t((pos[2] + radius) * query.grid.cell_width_inv), query.z_start + query.grid.dim_z - 1);

    query.x = query.x_start;
    query.y = query.y_start;
    query.z = query.z_start;

    const int cell = hash_grid_index(query.grid, query.x, query.y, query.z);
    query.cell_index = query.grid.cell_starts[cell];
    query.cell_end = query.grid.cell_ends[cell];

    return query;
}

__global__ void compute_cell_indices(HashGrid grid, const wp::vec3 *points, int num_points) {
    const int tid = blockIdx.x * blockDim.x + threadIdx.x;

    if (tid < num_points) {
        grid.point_cells[tid] = hash_grid_index(grid, points[tid]);
        grid.point_ids[tid] = tid;
    }
}

__global__ void compute_cell_offsets(int *cell_starts, int *cell_ends, const int *point_cells, int num_points) {
    const int tid = blockIdx.x * blockDim.x + threadIdx.x;

    // compute cell start / end
    if (tid < num_points) {
        // scan the particle-cell array to find the start and end
        const int c = point_cells[tid];

        if (tid == 0)
            cell_starts[c] = 0;
        else {
            const int p = point_cells[tid - 1];

            if (c != p) {
                cell_starts[c] = tid;
                cell_ends[p] = tid;
            }
        }

        if (tid == num_points - 1) {
            cell_ends[c] = tid + 1;
        }
    }
}

void hash_grid_rebuild_device(const wp::HashGrid &grid, const wp::vec3 *points, int num_points) {
    ContextGuard guard(grid.context);

    wp_launch_device(WP_CURRENT_CONTEXT, wp::compute_cell_indices, num_points, (grid, points, num_points))

        radix_sort_pairs_device(WP_CURRENT_CONTEXT, grid.point_cells, grid.point_ids, num_points);

    const int num_cells = grid.dim_x * grid.dim_y * grid.dim_z;

    memset_device(WP_CURRENT_CONTEXT, grid.cell_starts, 0, sizeof(int) * num_cells);
    memset_device(WP_CURRENT_CONTEXT, grid.cell_ends, 0, sizeof(int) * num_cells);

    wp_launch_device(WP_CURRENT_CONTEXT, wp::compute_cell_offsets, num_points, (grid.cell_starts, grid.cell_ends, grid.point_cells, num_points))
}

}// namespace wp
