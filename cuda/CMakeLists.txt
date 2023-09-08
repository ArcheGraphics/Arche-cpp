#  Copyright (c) 2023 Feng Yang
#
#  I am making my contributions/submissions to this project solely in my
#  personal capacity and am not conveying any rights to any intellectual
#  property of any third parties.

cmake_minimum_required(VERSION 3.16)

project(cuda-framework LANGUAGES C CXX CUDA)

set(UTILS_FILES
        utils/helper_multiprocess.cpp
        utils/helper_multiprocess.h
        utils/helper_cuda.h
        utils/helper_string.h
)

set(CORE_FILES
        core/cuda_device.h
        core/cuda_device.cpp
        core/cuda_stream.h
        core/cuda_stream.cpp
)

set(MATH_FILES
        math/cuda_math_utils.cpp
)

set(STRUCTURE_FILES
        structure/cuda_util.cpp
        structure/cuda_context.cpp
        structure/cuda_context.cu
        structure/sort.cpp
        structure/sort.cu
        structure/scan.cu
        structure/scan.cpp
        structure/hashgrid.cpp
        structure/hashgrid.cu
        structure/reduce.cpp
        structure/reduce.cu
        structure/sparse.cpp
        structure/sparse.cu
        structure/runlength_encode.cpp
        structure/runlength_encode.cu
        structure/marching.cu
        structure/bvh.cpp
        structure/bvh.cu
        structure/mesh.cpp
        structure/mesh.cu
)

set(SOLVER_FILES
        solver/sine_wave_simulation.cu
        solver/monte_carlo_pi.cu
)

set(PROJECT_FILES
        ${UTILS_FILES}
        ${CORE_FILES}
        ${MATH_FILES}
        ${STRUCTURE_FILES}
        ${SOLVER_FILES}
)

# mask out the min/max macros from minwindef.h
if (MSVC)
    add_definitions(-DNOMINMAX)
endif ()

#NB: switch this to shared library and things stop working. (there is likely two copies of volk somewhere.
add_library(${PROJECT_NAME} OBJECT ${PROJECT_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ../)

# Link third party libraries
target_link_libraries(${PROJECT_NAME} PUBLIC
        framework
        spdlog
        CUDA::cuda_driver
        nanovdb
)

if (${NEED_LINK_ATOMIC})
    target_link_libraries(${PROJECT_NAME} PUBLIC atomic)
endif ()
