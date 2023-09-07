#  Copyright (c) 2023 Feng Yang
#
#  I am making my contributions/submissions to this project solely in my
#  personal capacity and am not conveying any rights to any intellectual
#  property of any third parties.

if (NOT DEFINED CMAKE_C_COMPILER_LAUNCHER AND NOT DEFINED CMAKE_CXX_COMPILER_LAUNCHER)
    find_program(CCACHE_FOUND ccache)
    find_program(SCCACHE_FOUND sccache)
    if (CCACHE_FOUND AND NOT SCCACHE_FOUND)
        message("setting CCACHE to ${CCACHE_FOUND}")
        set(CMAKE_C_COMPILER_LAUNCHER ${CCACHE_FOUND})
        set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_FOUND})
    elseif (SCCACHE_FOUND AND NOT CCACHE_FOUND)
        message("setting CCACHE to ${CCACHE_FOUND}")
        set(CMAKE_C_COMPILER_LAUNCHER ${SCCACHE_FOUND})
        set(CMAKE_CXX_COMPILER_LAUNCHER ${SCCACHE_FOUND})
    endif (CCACHE_FOUND AND NOT SCCACHE_FOUND)
endif ()

cmake_minimum_required(VERSION 3.16)

# globally add VKB_DEBUG for the debug build
add_compile_definitions($<$<CONFIG:DEBUG>:METAL_DEBUG>)

project(arche-cpp)

# Add path for local cmake scripts
list(APPEND CMAKE_MODULE_PATH
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/USDInstall)

include(global_options)
include(check_atomic)

# usd
include(pxrConfig)
include_directories(${PXR_INCLUDE_DIRS})

# Add third party libraries
add_subdirectory(third_party)

add_subdirectory(math)
add_subdirectory(test.math)

add_subdirectory(framework)
add_subdirectory(application)
add_subdirectory(apps)

add_subdirectory(benchmark)