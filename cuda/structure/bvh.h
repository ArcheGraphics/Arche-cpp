//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "math/bounds.h"
#include "math/intersect.h"
#include "math/cuda_math_utils.h"

namespace wp {

struct BVHPackedNodeHalf {
    float x;
    float y;
    float z;
    unsigned int i : 31;
    unsigned int b : 1;
};

struct BVH {
    BVHPackedNodeHalf *node_lowers;
    BVHPackedNodeHalf *node_uppers;

    // used for fast refits
    int *node_parents;
    int *node_counts;

    int max_depth;
    int max_nodes;
    int num_nodes;

    int root;

    vec3 *lowers;
    vec3 *uppers;
    bounds3 *bounds;
    int num_bounds;

    void *context;
};

BVH bvh_create(const bounds3 *bounds, int num_bounds);

void bvh_destroy_host(BVH &bvh);
void bvh_destroy_device(BVH &bvh);

void bvh_refit_host(BVH &bvh, const bounds3 *bounds);
void bvh_refit_device(BVH &bvh, const bounds3 *bounds);

// copy host BVH to device
BVH bvh_clone(void *context, const BVH &bvh_host);

__device__ inline BVHPackedNodeHalf make_node(const vec3 &bound, int child, bool leaf) {
    BVHPackedNodeHalf n{};
    n.x = bound[0];
    n.y = bound[1];
    n.z = bound[2];
    n.i = (unsigned int)child;
    n.b = (unsigned int)(leaf ? 1 : 0);

    return n;
}

// variation of make_node through volatile pointers used in BuildHierarchy
__device__ inline void make_node(volatile BVHPackedNodeHalf *n, const vec3 &bound, int child, bool leaf) {
    n->x = bound[0];
    n->y = bound[1];
    n->z = bound[2];
    n->i = (unsigned int)child;
    n->b = (unsigned int)(leaf ? 1 : 0);
}

__device__ inline int clz(int x) {
    int n;
    if (x == 0) return 32;
    for (n = 0; ((x & 0x80000000) == 0); n++, x <<= 1)
        ;
    return n;
}

__device__ inline uint32_t part1by2(uint32_t n) {
    n = (n ^ (n << 16)) & 0xff0000ff;
    n = (n ^ (n << 8)) & 0x0300f00f;
    n = (n ^ (n << 4)) & 0x030c30c3;
    n = (n ^ (n << 2)) & 0x09249249;

    return n;
}

// Takes values in the range [0, 1] and assigns an index based Morton codes of length 3*lwp2(dim) bits
template<int dim>
__device__ inline uint32_t morton3(float x, float y, float z) {
    uint32_t ux = clamp(int(x * dim), 0, dim - 1);
    uint32_t uy = clamp(int(y * dim), 0, dim - 1);
    uint32_t uz = clamp(int(z * dim), 0, dim - 1);

    return (part1by2(uz) << 2) | (part1by2(uy) << 1) | part1by2(ux);
}

__device__ inline int bvh_get_num_bounds(const BVH &bvh) {
    return bvh.num_bounds;
}

// stores state required to traverse the BVH nodes that
// overlap with a query AABB.
struct bvh_query_t {
    __device__ bvh_query_t() = default;
    __device__ explicit bvh_query_t(int) {}

    BVH bvh{};

    // BVH traversal stack:
    int stack[32]{};
    int count{};

    // inputs
    bool is_ray{};
    wp::vec3 input_lower;// start for ray
    wp::vec3 input_upper;// dir for ray

    int bounds_nr{};
};

__device__ inline bvh_query_t bvh_query(const BVH &bvh, bool is_ray, const vec3 &lower, const vec3 &upper) {
    // This routine traverses the BVH tree until it finds
    // the first overlapping bound.

    // initialize empty
    bvh_query_t query;

    query.bounds_nr = -1;

    query.bvh = bvh;
    query.is_ray = is_ray;

    // if no bvh nodes, return empty query.
    if (bvh.num_nodes == 0) {
        query.count = 0;
        return query;
    }

    // optimization: make the latest

    query.stack[0] = bvh.root;
    query.count = 1;
    query.input_lower = lower;
    query.input_upper = upper;

    wp::bounds3 input_bounds(query.input_lower, query.input_upper);

    // Navigate through the bvh, find the first overlapping leaf node.
    while (query.count) {
        const int node_index = query.stack[--query.count];

        BVHPackedNodeHalf node_lower = bvh.node_lowers[node_index];
        BVHPackedNodeHalf node_upper = bvh.node_uppers[node_index];

        wp::vec3 lower_pos(node_lower.x, node_lower.y, node_lower.z);
        wp::vec3 upper_pos(node_upper.x, node_upper.y, node_upper.z);
        wp::bounds3 current_bounds(lower_pos, upper_pos);

        if (query.is_ray) {
            float t = 0.0f;
            if (!intersect_ray_aabb(query.input_lower, query.input_upper, current_bounds.lower, current_bounds.upper, t))
                // Skip this box, it doesn't overlap with our ray.
                continue;
        } else {
            if (!input_bounds.overlaps(current_bounds))
                // Skip this box, it doesn't overlap with our target box.
                continue;
        }

        const int left_index = node_lower.i;
        const int right_index = node_upper.i;

        // Make bounds from this AABB
        if (node_lower.b) {
            // found very first leaf index.
            // Back up one level and return
            query.stack[query.count++] = node_index;
            return query;
        } else {
            query.stack[query.count++] = left_index;
            query.stack[query.count++] = right_index;
        }
    }

    return query;
}

__device__ inline bvh_query_t bvh_query_aabb(const BVH &bvh, const vec3 &lower, const vec3 &upper) {
    return bvh_query(bvh, false, lower, upper);
}

__device__ inline bvh_query_t bvh_query_ray(const BVH &bvh, const vec3 &start, const vec3 &dir) {
    return bvh_query(bvh, true, start, dir);
}

__device__ inline bool bvh_query_next(bvh_query_t &query, int &index) {
    BVH bvh = query.bvh;

    wp::bounds3 input_bounds(query.input_lower, query.input_upper);

    // Navigate through the bvh, find the first overlapping leaf node.
    while (query.count) {
        const int node_index = query.stack[--query.count];
        BVHPackedNodeHalf node_lower = bvh.node_lowers[node_index];
        BVHPackedNodeHalf node_upper = bvh.node_uppers[node_index];

        wp::vec3 lower_pos(node_lower.x, node_lower.y, node_lower.z);
        wp::vec3 upper_pos(node_upper.x, node_upper.y, node_upper.z);
        wp::bounds3 current_bounds(lower_pos, upper_pos);

        if (query.is_ray) {
            float t = 0.0f;
            if (!intersect_ray_aabb(query.input_lower, query.input_upper, current_bounds.lower, current_bounds.upper, t))
                // Skip this box, it doesn't overlap with our ray.
                continue;
        } else {
            if (!input_bounds.overlaps(current_bounds))
                // Skip this box, it doesn't overlap with our target box.
                continue;
        }

        const int left_index = node_lower.i;
        const int right_index = node_upper.i;

        if (node_lower.b) {
            // found leaf
            query.bounds_nr = left_index;
            index = left_index;
            return true;
        } else {

            query.stack[query.count++] = left_index;
            query.stack[query.count++] = right_index;
        }
    }
    return false;
}

__device__ inline int iter_next(bvh_query_t &query) {
    return query.bounds_nr;
}

__device__ inline bool iter_cmp(bvh_query_t &query) {
    bool finished = bvh_query_next(query, query.bounds_nr);
    return finished;
}

__device__ inline bvh_query_t iter_reverse(const bvh_query_t &query) {
    // can't reverse BVH queries, users should not rely on traversal ordering
    return query;
}

__device__ __host__ bool bvh_get_descriptor(uint64_t id, BVH &bvh);
__device__ void bvh_add_descriptor(uint64_t id, const BVH &bvh);
__device__ void bvh_rem_descriptor(uint64_t id);

}// namespace wp

uint64_t bvh_create_host(wp::vec3 *lowers, wp::vec3 *uppers, int num_bounds);
void bvh_destroy_host(wp::BVH *bvh);
void bvh_refit_host(wp::BVH *bvh);

uint64_t bvh_create_device(void *context, wp::vec3 *lowers, wp::vec3 *uppers, int num_bounds);
void bvh_destroy_device(uint64_t id);
void bvh_refit_device(uint64_t id);