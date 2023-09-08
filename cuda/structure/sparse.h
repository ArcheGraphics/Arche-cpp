//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "math/vec.h"

int bsr_matrix_from_triplets_float_host(
    int rows_per_block,
    int cols_per_block,
    int row_count,
    int nnz,
    uint64_t tpl_rows,
    uint64_t tpl_columns,
    uint64_t tpl_values,
    uint64_t bsr_offsets,
    uint64_t bsr_columns,
    uint64_t bsr_values);
int bsr_matrix_from_triplets_double_host(
    int rows_per_block,
    int cols_per_block,
    int row_count,
    int nnz,
    uint64_t tpl_rows,
    uint64_t tpl_columns,
    uint64_t tpl_values,
    uint64_t bsr_offsets,
    uint64_t bsr_columns,
    uint64_t bsr_values);

int bsr_matrix_from_triplets_float_device(
    int rows_per_block,
    int cols_per_block,
    int row_count,
    int nnz,
    uint64_t tpl_rows,
    uint64_t tpl_columns,
    uint64_t tpl_values,
    uint64_t bsr_offsets,
    uint64_t bsr_columns,
    uint64_t bsr_values);
int bsr_matrix_from_triplets_double_device(
    int rows_per_block,
    int cols_per_block,
    int row_count,
    int nnz,
    uint64_t tpl_rows,
    uint64_t tpl_columns,
    uint64_t tpl_values,
    uint64_t bsr_offsets,
    uint64_t bsr_columns,
    uint64_t bsr_values);

void bsr_transpose_float_host(int rows_per_block, int cols_per_block,
                              int row_count, int col_count, int nnz,
                              uint64_t bsr_offsets, uint64_t bsr_columns,
                              uint64_t bsr_values,
                              uint64_t transposed_bsr_offsets,
                              uint64_t transposed_bsr_columns,
                              uint64_t transposed_bsr_values);
void bsr_transpose_double_host(int rows_per_block, int cols_per_block,
                               int row_count, int col_count, int nnz,
                               uint64_t bsr_offsets, uint64_t bsr_columns,
                               uint64_t bsr_values,
                               uint64_t transposed_bsr_offsets,
                               uint64_t transposed_bsr_columns,
                               uint64_t transposed_bsr_values);

void bsr_transpose_float_device(int rows_per_block, int cols_per_block,
                                int row_count, int col_count, int nnz,
                                uint64_t bsr_offsets, uint64_t bsr_columns,
                                uint64_t bsr_values,
                                uint64_t transposed_bsr_offsets,
                                uint64_t transposed_bsr_columns,
                                uint64_t transposed_bsr_values);
void bsr_transpose_double_device(int rows_per_block, int cols_per_block,
                                 int row_count, int col_count, int nnz,
                                 uint64_t bsr_offsets, uint64_t bsr_columns,
                                 uint64_t bsr_values,
                                 uint64_t transposed_bsr_offsets,
                                 uint64_t transposed_bsr_columns,
                                 uint64_t transposed_bsr_values);