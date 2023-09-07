# Metal cpp
add_library(metal-cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/definition.cpp
        )

target_include_directories(metal-cpp PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/metal-cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/metal-cpp-extensions"
        )

target_link_libraries(metal-cpp
        "-framework Metal"
        "-framework MetalKit"
        "-framework AppKit"
        "-framework Foundation"
        "-framework QuartzCore"
        )

# googletest
add_subdirectory(googletest)

# google bechmark
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
set(BENCHMARK_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory(benchmark EXCLUDE_FROM_ALL)

# tinygltf
add_subdirectory(tinygltf)

# imgui
set(IMGUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/imgui")
set(IMGUI_FILES
        "${IMGUI_DIR}/imgui.cpp"
        "${IMGUI_DIR}/imgui_demo.cpp"
        "${IMGUI_DIR}/imgui_draw.cpp"
        "${IMGUI_DIR}/imgui_tables.cpp"
        "${IMGUI_DIR}/imgui_widgets.cpp"
        "${IMGUI_DIR}/imconfig.h"
        "${IMGUI_DIR}/imgui.h"
        "${IMGUI_DIR}/imgui_internal.h"
        "${IMGUI_DIR}/imstb_rectpack.h"
        "${IMGUI_DIR}/imstb_textedit.h"
        "${IMGUI_DIR}/imstb_truetype.h")
add_library(imgui STATIC ${IMGUI_FILES})
target_include_directories(imgui PUBLIC ${IMGUI_DIR})
set_target_properties(imgui PROPERTIES FOLDER "ThirdParty" POSITION_INDEPENDENT_CODE ON)

# stb
add_library(stb INTERFACE)
target_include_directories(stb INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/stb")

# spdlog
add_subdirectory(spdlog)

# GLFW
option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_TESTS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)
option(GLFW_INSTALL OFF)
option(BUILD_SHARED_LIBS ON)
add_subdirectory(glfw)
set_property(TARGET glfw PROPERTY FOLDER "ThirdParty")