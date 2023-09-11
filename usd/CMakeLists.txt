#  Copyright (c) 2023 Feng Yang
#
#  I am making my contributions/submissions to this project solely in my
#  personal capacity and am not conveying any rights to any intellectual
#  property of any third parties.

cmake_minimum_required(VERSION 3.16)

project(usd-framework LANGUAGES C CXX)

set(COMMON_FILES
        usd_renderer.h
        usd_renderer.cpp
        # GLTF
        cgltf_util.cpp
        converter.cpp
        debug_codes.cpp
        file_format.cpp
        guc.cpp
        image.cpp
        materialx.cpp
        mesh.cpp
        naming.cpp
        usd_preview_surface.cpp
)

source_group("common\\" FILES ${COMMON_FILES})

set(PROJECT_FILES
        ${COMMON_FILES}
)

#NB: switch this to shared library and things stop working. (there is likely two copies of volk somewhere.
add_library(${PROJECT_NAME} OBJECT ${PROJECT_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)

if (${METAL_VULKAN_DEBUG})
    target_compile_definitions(${PROJECT_NAME} PUBLIC METAL_VULKAN_DEBUG)
endif ()

# silent c++20 warning
target_compile_options(${PROJECT_NAME} PRIVATE -Wno-deprecated)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ../)

# Link third party libraries
target_link_libraries(${PROJECT_NAME} PUBLIC
        spdlog
        mikktspace
        cgltf
        ${PXR_LIBRARY_NAMES}
)

if (${NEED_LINK_ATOMIC})
    target_link_libraries(${PROJECT_NAME} PUBLIC atomic)
endif ()

if (METAL_DO_CLANG_TIDY)
    set_target_properties(framework PROPERTIES CXX_CLANG_TIDY "${METAL_DO_CLANG_TIDY}")
endif ()