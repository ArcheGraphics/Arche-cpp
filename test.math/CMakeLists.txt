cmake_minimum_required(VERSION 3.12)

project(test.math LANGUAGES C CXX)

file(GLOB sources
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_executable(${PROJECT_NAME} ${sources})

# Link third party libraries
target_link_libraries(${PROJECT_NAME} PRIVATE math
        gtest gmock gtest_main gmock_main)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})