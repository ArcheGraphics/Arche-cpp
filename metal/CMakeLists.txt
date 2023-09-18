#  Copyright (c) 2023 Feng Yang
#
#  I am making my contributions/submissions to this project solely in my
#  personal capacity and am not conveying any rights to any intellectual
#  property of any third parties.

cmake_minimum_required(VERSION 3.16)

project(metal-framework LANGUAGES C CXX)

set(COMMON_FILES
        metal_api.h
        metal_swapchain.cpp
        metal_swapchain_layer.mm
        metal_buffer.cpp
        metal_command_encoder.cpp
        metal_debug_capture.cpp
        metal_device.cpp
        metal_event.cpp
        metal_stream.cpp
        metal_stream.mm
        metal_texture.cpp
)

set(FG_FILES
        fg/framegraph.h
        fg/realize.h
        fg/render_task.h
        fg/render_task_base.h
        fg/render_task_builder.h
        fg/resource.h
        fg/resource_base.h
        # Source Files
        fg/framegraph.cpp
)

set(COMPUTE_FILES
        compute/data_type_util.h
        compute/data_type_util.cpp
        compute/status_util.h
        compute/status_util.cpp
        compute/buffer_utils.h
        compute/buffer_utils.cpp
        compute/image_utils.h
        compute/image_utils.cpp
        compute/gpu_counter.h
        compute/gpu_counter.cpp
        compute/gpu_capture.h
        compute/gpu_capture.cpp
)

source_group("\\" FILES ${FRAMEWORK_FILES})
source_group("compute\\" FILES ${COMPUTE_FILES})
source_group("framegraph\\" FILES ${FG_FILES})

set(PROJECT_FILES
        ${FG_FILES}
        ${COMPUTE_FILES}
        ${COMMON_FILES}
)

#NB: switch this to shared library and things stop working. (there is likely two copies of volk somewhere.
add_library(${PROJECT_NAME} OBJECT ${PROJECT_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)

if (${METAL_VULKAN_DEBUG})
    target_compile_definitions(${PROJECT_NAME} PUBLIC METAL_VULKAN_DEBUG)
endif ()

if (METAL_WARNINGS_AS_ERRORS)
    message(STATUS "Warnings as Errors Enabled")
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        target_compile_options(${PROJECT_NAME} PRIVATE -Werror)
    elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
        target_compile_options(${PROJECT_NAME} PRIVATE /W3 /WX)
    endif ()
endif ()

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ../)

# Link third party libraries
target_link_libraries(${PROJECT_NAME} PUBLIC
        spdlog
        metal-cpp
)

if (${NEED_LINK_ATOMIC})
    target_link_libraries(${PROJECT_NAME} PUBLIC atomic)
endif ()

if (METAL_DO_CLANG_TIDY)
    set_target_properties(framework PROPERTIES CXX_CLANG_TIDY "${METAL_DO_CLANG_TIDY}")
endif ()