#  Copyright (c) 2023 Feng Yang
#
#  I am making my contributions/submissions to this project solely in my
#  personal capacity and am not conveying any rights to any intellectual
#  property of any third parties.

cmake_minimum_required(VERSION 3.16)

project(usd-framework LANGUAGES C CXX)

set(BASE_FILES
        editor/base/geometric_functions.h
        editor/base/usd_helpers.cpp
        editor/base/imgui_helpers.cpp
)

set(MANIPULATOR_FILES
        editor/manipulators/manipulator.h
        editor/manipulators/camera_rig.h
        editor/manipulators/camera_rig.cpp
        editor/manipulators/camera_manipulator.cpp
        editor/manipulators/mouse_hover_manipulator.cpp
        editor/manipulators/playblast.cpp
        editor/manipulators/position_manipulator.cpp
        editor/manipulators/rotation_manipulator.cpp
        editor/manipulators/scale_manipulator.cpp
        editor/manipulators/selection_manipulator.cpp
        editor/manipulators/viewport.cpp
        editor/manipulators/imaging_settings.cpp
)

set(COMMAND_FILES
        editor/commands/attribute_commands.cpp
        editor/commands/command_stack.cpp
        editor/commands/commands_impl.cpp
        editor/commands/layer_commands.cpp
        editor/commands/prim_commands.cpp
        editor/commands/sdf_command_group.cpp
        editor/commands/sdf_command_group_recorder.cpp
        editor/commands/sdf_layer_instructions.cpp
        editor/commands/sdf_undo_redo_recorder.cpp
        editor/commands/undo_layer_state_delegate.cpp
)

set(WIDGETS_FILES
        editor/widgets/composition_editor.cpp
        editor/widgets/connection_editor.cpp
        editor/widgets/content_browser.cpp
        editor/widgets/debug.cpp
        editor/widgets/file_browser.cpp
        editor/widgets/launcher_bar.cpp
        editor/widgets/modal_dialogs.cpp
        editor/widgets/sdf_attribute_editor.cpp
        editor/widgets/sdf_layer_editor.cpp
        editor/widgets/sdf_layer_scene_graph_editor.cpp
        editor/widgets/sdf_prim_editor.cpp
        editor/widgets/stage_layer_editor.cpp
        editor/widgets/stage_outliner.cpp
        editor/widgets/text_editor.cpp
        editor/widgets/text_filter.cpp
        editor/widgets/timeline.cpp
        editor/widgets/usd_prim_editor.cpp
        editor/widgets/variant_editor.cpp
        editor/widgets/vt_array_editor.cpp
        editor/widgets/vt_dictionary_editor.cpp
        editor/widgets/vt_value_editor.cpp
)

set(FONT_FILES
        editor/fonts/resources_loader.cpp)

set(EDITOR_FILES
        editor/entry/editor_settings.cpp
)

set(COMMON_FILES
        usd_primitive.h
        usd_primitive.cpp
)

source_group("common\\" FILES ${COMMON_FILES})

set(PROJECT_FILES
        ${COMMON_FILES}
        # Editor
        editor/selection.cpp
        editor/blueprints.cpp
        editor/editor.cpp
        ${BASE_FILES}
        ${COMMAND_FILES}
        ${MANIPULATOR_FILES}
        ${WIDGETS_FILES}
        ${FONT_FILES}
        ${EDITOR_FILES}
)

#NB: switch this to shared library and things stop working. (there is likely two copies of volk somewhere.
add_library(${PROJECT_NAME} OBJECT ${PROJECT_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)

if (${METAL_VULKAN_DEBUG})
    target_compile_definitions(${PROJECT_NAME} PUBLIC METAL_VULKAN_DEBUG)
endif ()

# silent c++20 warning
target_compile_options(${PROJECT_NAME} PRIVATE -Wno-deprecated)

target_include_directories(${PROJECT_NAME} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/editor
        ../
)

# Link third party libraries
target_link_libraries(${PROJECT_NAME} PUBLIC
        spdlog
        mikktspace
        cgltf
        imgui
        glfw
        ${PXR_LIBRARY_NAMES}
)

if (${NEED_LINK_ATOMIC})
    target_link_libraries(${PROJECT_NAME} PUBLIC atomic)
endif ()

if (METAL_DO_CLANG_TIDY)
    set_target_properties(framework PROPERTIES CXX_CLANG_TIDY "${METAL_DO_CLANG_TIDY}")
endif ()