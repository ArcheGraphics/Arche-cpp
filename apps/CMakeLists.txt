#  Copyright (c) 2023 Feng Yang
#
#  I am making my contributions/submissions to this project solely in my
#  personal capacity and am not conveying any rights to any intellectual
#  property of any third parties.

cmake_minimum_required(VERSION 3.10)
project(apps)

set(BASE_FILES
        base/geometric_functions.h
        base/usd_helpers.cpp
        base/imgui_helpers.cpp
)

set(MANIPULATOR_FILES
        manipulators/manipulator.h
        manipulators/camera_rig.h
        manipulators/camera_rig.cpp
)

set(COMMAND_FILES
        commands/attribute_commands.cpp
        commands/command_stack.cpp
        commands/commands_impl.cpp
        commands/layer_commands.cpp
        commands/prim_commands.cpp
        commands/sdf_command_group.cpp
        commands/sdf_command_group_recorder.cpp
        commands/sdf_layer_instructions.cpp
        commands/sdf_undo_redo_recorder.cpp
        commands/undo_layer_state_delegate.cpp
)

set(EDITOR_FILES
        editor/editor_settings.cpp
)

add_executable(${PROJECT_NAME}
        main.cpp
        viewport.mm
        camera.cpp
        selection.cpp
        blueprints.cpp
        ${BASE_FILES}
        ${COMMAND_FILES}
        ${MANIPULATOR_FILES}
        ${EDITOR_FILES}
)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ../)

set(PXR_LIBRARY_NAMES)
foreach (_item ${PXR_LIBRARIES}) # Iterate over list of absolute paths
    get_filename_component(_t ${_item} NAME)
    list(APPEND PXR_LIBRARY_NAMES ${_t})
endforeach ()

target_link_libraries(${PROJECT_NAME}
        common
        metal-framework
        usd-framework
        ${PXR_LIBRARY_NAMES}
)