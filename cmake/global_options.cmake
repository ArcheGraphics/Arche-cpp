#[[
 Copyright (c) 2019-2023, Arm Limited and Contributors

 SPDX-License-Identifier: Apache-2.0

 Licensed under the Apache License, Version 2.0 the "License";
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.

 ]]

# Enable group projects in folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake")

set(METAL_WARNINGS_AS_ERRORS ON CACHE BOOL "Enable Warnings as Errors")
set(METAL_BUILD_TESTS OFF CACHE BOOL "Enable generation and building of Vulkan best practice tests.")
set(METAL_CLANG_TIDY OFF CACHE STRING "Use CMake Clang Tidy integration")
set(METAL_CLANG_TIDY_EXTRAS "-header-filter=framework,samples,app;-checks=-*,google-*,-google-runtime-references;--fix;--fix-errors" CACHE STRING "Clang Tidy Parameters")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "bin/${CMAKE_BUILD_TYPE}/${TARGET_ARCH}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "lib/${CMAKE_BUILD_TYPE}/${TARGET_ARCH}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "lib/${CMAKE_BUILD_TYPE}/${TARGET_ARCH}")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED 20)
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

string(LENGTH "${CMAKE_SOURCE_DIR}/" ROOT_PATH_SIZE)
add_definitions(-DROOT_PATH_SIZE=${ROOT_PATH_SIZE})

set(CMAKE_C_FLAGS_DEBUG   "-DDEBUG=0 ${CMAKE_C_FLAGS_DEBUG}")
set(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG=0 ${CMAKE_CXX_FLAGS_DEBUG}")

if (METAL_CLANG_TIDY)
    find_program(CLANG_TIDY "clang-tidy" "clang-tidy-15" REQUIRED)
    set(METAL_DO_CLANG_TIDY ${CLANG_TIDY} ${METAL_CLANG_TIDY_EXTRAS})
endif()