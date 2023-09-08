//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "math/vec.h"

void *alloc_host(size_t s);
void *alloc_pinned(size_t s);
void *alloc_device(void *context, size_t s);

void free_host(void *ptr);
void free_pinned(void *ptr);
void free_device(void *context, void *ptr);

// all memcpys are performed asynchronously
void memcpy_h2h(void *dest, void *src, size_t n);
void memcpy_h2d(void *context, void *dest, void *src, size_t n);
void memcpy_d2h(void *context, void *dest, void *src, size_t n);
void memcpy_d2d(void *context, void *dest, void *src, size_t n);
void memcpy_peer(void *context, void *dest, void *src, size_t n);

// all memsets are performed asynchronously
void memset_host(void *dest, int value, size_t n);
void memset_device(void *context, void *dest, int value, size_t n);

// takes srcsize bytes starting at src and repeats them n times at dst (writes srcsize * n bytes in total):
void memtile_host(void *dest, const void *src, size_t srcsize, size_t n);
void memtile_device(void *context, void *dest, const void *src, size_t srcsize, size_t n);

int cuda_driver_version(); // CUDA driver version
int cuda_toolkit_version();// CUDA Toolkit version used to build Warp

int cuda_device_get_count();
void *cuda_device_primary_context_retain(int ordinal);
void cuda_device_primary_context_release(int ordinal);
const char *cuda_device_get_name(int ordinal);
int cuda_device_get_arch(int ordinal);
int cuda_device_is_uva(int ordinal);

void *cuda_context_get_current();
void cuda_context_set_current(void *context);
void cuda_context_push_current(void *context);
void cuda_context_pop_current();
void *cuda_context_create(int device_ordinal);
void cuda_context_destroy(void *context);
int cuda_context_get_device_ordinal(void *context);
int cuda_context_is_primary(void *context);
void *cuda_context_get_stream(void *context);
void cuda_context_set_stream(void *context, void *stream);
int cuda_context_can_access_peer(void *context, void *peer_context);
int cuda_context_enable_peer_access(void *context, void *peer_context);

// ensures all device side operations have completed in the current context
void cuda_context_synchronize(void *context);

// return cudaError_t code
uint64_t cuda_context_check(void *context);

void *cuda_stream_create(void *context);
void cuda_stream_destroy(void *context, void *stream);
void cuda_stream_synchronize(void *context, void *stream);
void *cuda_stream_get_current();
void cuda_stream_wait_event(void *context, void *stream, void *event);
void cuda_stream_wait_stream(void *context, void *stream, void *other_stream, void *event);

void *cuda_event_create(void *context, unsigned flags);
void cuda_event_destroy(void *context, void *event);
void cuda_event_record(void *context, void *event, void *stream);

void cuda_graph_begin_capture(void *context);
void *cuda_graph_end_capture(void *context);
void cuda_graph_launch(void *context, void *graph);
void cuda_graph_destroy(void *context, void *graph);

void *cuda_get_kernel(void *context, void *module, const char *name);
size_t cuda_launch_kernel(void *context, void *kernel, size_t dim, void **args);

void cuda_set_context_restore_policy(bool always_restore);
int cuda_get_context_restore_policy();

void cuda_graphics_map(void *context, void *resource);
void cuda_graphics_unmap(void *context, void *resource);
void cuda_graphics_device_ptr_and_size(void *context, void *resource, uint64_t *ptr, size_t *size);
void cuda_graphics_unregister_resource(void *context, void *resource);

// generic copy supporting non-contiguous arrays
size_t array_copy_host(void *dst, void *src, int dst_type, int src_type, int elem_size);
size_t array_copy_device(void *context, void *dst, void *src, int dst_type, int src_type, int elem_size);

// generic fill for non-contiguous arrays
void array_fill_host(void *arr, int arr_type, const void *value, int value_size);
void array_fill_device(void *context, void *arr, int arr_type, const void *value, int value_size);