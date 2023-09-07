#  Copyright (c) 2023 Feng Yang
#
#  I am making my contributions/submissions to this project solely in my
#  personal capacity and am not conveying any rights to any intellectual
#  property of any third parties.

cmake_minimum_required(VERSION 3.16)

project(application LANGUAGES C CXX)

set(COMMON_FILES
        # Header Files
        application.h
        application.cpp
        graphics_application.h
        graphics_application.cpp
        forward_application.h
        forward_application.cpp
)

set(BASE_FILES
        base/background.h
        base/layer.h
        base/singleton.h
        base/update_flag.h
        base/update_flag.cpp
        base/update_flag_manager.h
        base/update_flag_manager.cpp)

set(TEXTURE_FILES
        texture/texture.cpp
        texture/texture.h
        texture/stb.cpp
        texture/stb.h
        texture/texture_manager.h
        texture/texture_manager.cpp
)

set(MESH_FILES
        mesh/sub_mesh.h
        mesh/sub_mesh.cpp
        mesh/mesh_manager.h
        mesh/mesh_manager.cpp
        mesh/mesh.h
        mesh/mesh.cpp
        mesh/model_mesh.h
        mesh/model_mesh.cpp
        mesh/primitive_mesh.h
        mesh/primitive_mesh.cpp
        mesh/buffer_mesh.h
        mesh/buffer_mesh.cpp
)

set(SHADER_FILES
        shader/shader_data.h
        shader/shader_data.cpp
        shader/shader_macro_collection.h
        shader/shader_macro_collection.cpp
        shader/shader_uniform.h
        shader/state/blend_state.cpp
        shader/state/blend_state.h
        shader/state/depth_state.cpp
        shader/state/depth_state.h
        shader/state/raster_state.cpp
        shader/state/raster_state.h
        shader/state/render_state.cpp
        shader/state/render_state.h
        shader/state/renderTarget_blend_state.h
        shader/state/stencil_state.cpp
        shader/state/stencil_state.h
)

set(MATERIAL_FILES
        material/material.h
        material/material.cpp
        material/base_material.h
        material/base_material.cpp
        material/unlit_material.h
        material/unlit_material.cpp)

set(COMPONENTS_FILES
        components/camera.h
        components/camera.cpp
        components/script.h
        components/script.cpp
        components/transform.h
        components/transform.cpp
        components/renderer.h
        components/renderer.cpp
        components/mesh_renderer.h
        components/mesh_renderer.cpp)

set(ECS_FILES
        ecs/component.h
        ecs/component.cpp
        ecs/components_manager.h
        ecs/components_manager.cpp
        ecs/entity.h
        ecs/entity.cpp
        ecs/scene.h
        ecs/scene.cpp
        ecs/scene_manager.h
        ecs/scene_manager.cpp
)

set(LIGHT_FILES
        light/ambient_light.h
        light/ambient_light.cpp
        light/direct_light.h
        light/direct_light.cpp
        light/light.h
        light/light.cpp
        light/light_manager.h
        light/light_manager.cpp
        light/point_light.h
        light/point_light.cpp
        light/spot_light.h
        light/spot_light.cpp)

set(CONTROL_FILES
        controls/spherical.h
        controls/spherical.cpp
        controls/orbit_control.h
        controls/orbit_control.cpp
        controls/free_control.h
        controls/free_control.cpp)

set(RENDERING_FILES
        rendering/compute_pass.cpp
        rendering/compute_pass.h
        rendering/compute_pipeline_state.cpp
        rendering/compute_pipeline_state.h
        rendering/render_element.cpp
        rendering/render_element.h
        rendering/render_pass.h
        rendering/render_pass.cpp
        rendering/render_pipeline_state.cpp
        rendering/render_pipeline_state.h
        rendering/resource_cache.cpp
        rendering/resource_cache.h
        rendering/subpass.cpp
        rendering/subpass.h
        rendering/subpasses/forward_subpass.cpp
        rendering/subpasses/forward_subpass.h
)

set(GUI_FILES
        gui/imgui_impl_glfw.cpp
        gui/imgui_impl_metal.mm
        gui/gui.cpp
)

source_group("\\" FILES ${FRAMEWORK_FILES})
source_group("common\\" FILES ${COMMON_FILES})

set(PROJECT_FILES
        ${COMMON_FILES}
        ${BASE_FILES}
        ${TEXTURE_FILES}
        ${MESH_FILES}
        ${SHADER_FILES}
        ${MATERIAL_FILES}
        ${ECS_FILES}
        ${COMPONENTS_FILES}
        ${LIGHT_FILES}
        ${CONTROL_FILES}
        ${RENDERING_FILES}
        ${GUI_FILES}
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
        math
        framework
        spdlog
        stb
        imgui
        glfw
)

if (${NEED_LINK_ATOMIC})
    target_link_libraries(${PROJECT_NAME} PUBLIC atomic)
endif ()

if (METAL_DO_CLANG_TIDY)
    set_target_properties(framework PROPERTIES CXX_CLANG_TIDY "${METAL_DO_CLANG_TIDY}")
endif ()