//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "math/vec.h"
#include <cassert>

namespace wp {

struct HashGrid {
    float cell_width{};
    float cell_width_inv{};

    int *point_cells{nullptr};// cell id of a point
    int *point_ids{nullptr};  // index to original point

    int *cell_starts{nullptr};// start index of a range of indices belonging to a cell, dim_x*dim_y*dim_z in length
    int *cell_ends{nullptr};  // end index of a range of indices belonging to a cell, dim_x*dim_y*dim_z in length

    int dim_x{};
    int dim_y{};
    int dim_z{};

    int num_points{};
    int max_points{};

    void *context{};
};

// convert a virtual (world) cell coordinate to a physical one
__device__ int hash_grid_index(const HashGrid &grid, int x, int y, int z);

__device__ inline int hash_grid_index(const HashGrid &grid, const vec3 &p) {
    return hash_grid_index(grid,
                           int(p[0] * grid.cell_width_inv),
                           int(p[1] * grid.cell_width_inv),
                           int(p[2] * grid.cell_width_inv));
}

// stores state required to traverse neighboring cells of a point
struct hash_grid_query_t {
    __device__ hash_grid_query_t() = default;
    __device__ explicit hash_grid_query_t(int) {}// for backward pass

    int x_start{};
    int y_start{};
    int z_start{};

    int x_end{};
    int y_end{};
    int z_end{};

    int x{};
    int y{};
    int z{};

    int cell{};
    int cell_index{};// offset in the current cell (index into cell_indices)
    int cell_end{};  // index following the end of this cell

    int current{};// index of the current iterator value

    HashGrid grid;
};

__device__ hash_grid_query_t hash_grid_query(const HashGrid *grid, wp::vec3 pos, float radius);

__device__ inline bool hash_grid_query_next(hash_grid_query_t &query, int &index) {
    const HashGrid &grid = query.grid;
    if (!grid.point_cells)
        return false;

    while (true) {
        if (query.cell_index < query.cell_end) {
            // write output index
            index = grid.point_ids[query.cell_index++];
            return true;
        } else {
            query.x++;
            if (query.x > query.x_end) {
                query.x = query.x_start;
                query.y++;
            }

            if (query.y > query.y_end) {
                query.y = query.y_start;
                query.z++;
            }

            if (query.z > query.z_end) {
                // finished lookup grid
                return false;
            }

            // update cell pointers
            const int cell = hash_grid_index(grid, query.x, query.y, query.z);

            query.cell_index = grid.cell_starts[cell];
            query.cell_end = grid.cell_ends[cell];
        }
    }
}

__device__ inline int iter_next(hash_grid_query_t &query) {
    return query.current;
}

__device__ inline bool iter_cmp(hash_grid_query_t &query) {
    bool finished = hash_grid_query_next(query, query.current);
    return finished;
}

__device__ inline hash_grid_query_t iter_reverse(const hash_grid_query_t &query) {
    // can't reverse grid queries, users should not rely on neighbor ordering
    return query;
}

__device__ inline int hash_grid_point_id(const HashGrid *grid, int &index) {
    if (grid->point_ids == nullptr)
        return -1;
    return grid->point_ids[index];
}

}// namespace wp

uint64_t hash_grid_create_host(int dim_x, int dim_y, int dim_z);
void hash_grid_reserve_host(wp::HashGrid *grid, int num_points);
void hash_grid_destroy_host(wp::HashGrid *grid);
void hash_grid_update_host(wp::HashGrid *grid, float cell_width, const wp::vec3 *positions, int num_points);

uint64_t hash_grid_create_device(void *context, int dim_x, int dim_y, int dim_z);
void hash_grid_reserve_device(uint64_t id, int num_points);
void hash_grid_destroy_device(uint64_t id);
void hash_grid_update_device(uint64_t id, float cell_width, const wp::vec3 *positions, int num_points);