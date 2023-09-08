#  Copyright (c) 2023 Feng Yang
#
#  I am making my contributions/submissions to this project solely in my
#  personal capacity and am not conveying any rights to any intellectual
#  property of any third parties.

cmake_minimum_required(VERSION 3.10)
project(apps)

set(HYD_PUBLIC_HEADERS
        camera.h
        view_delegate.h
        app_delegate.h
        view_controller.h
        renderer.h
)

set(RESOURCES
        Base.lproj/Main.storyboard
        Assets.xcassets)

add_executable(${PROJECT_NAME} MACOSX_BUNDLE
        main.mm
        app_delegate.mm
        view_controller.mm
        view_delegate.mm
        renderer.mm
        ${RESOURCES}
)

add_definitions(-DMACOS)
add_definitions(-DPXR_METAL_SUPPORT_ENABLED)

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

set(EMBEDDED_BINARIES_PLUGIN)
file(GLOB_RECURSE temp
        "${PXR_CMAKE_DIR}/plugin/*.dylib"
        "${PXR_CMAKE_DIR}/plugin/*.glslfx"
        "${PXR_CMAKE_DIR}/plugin/*.oso"
        "${PXR_CMAKE_DIR}/plugin/*.json"
        "${PXR_CMAKE_DIR}/plugin/*.usda")

foreach (_item ${temp}) # Iterate over list of absolute paths
    file(RELATIVE_PATH _t "${PXR_CMAKE_DIR}/plugin/" ${_item})
    list(APPEND EMBEDDED_BINARIES_PLUGIN ${_t})
endforeach()

set(EMBEDDED_BINARIES_LIB)
file(GLOB temp "${PXR_CMAKE_DIR}/lib/*.dylib")
foreach (_item ${temp}) # Iterate over list of absolute paths
    get_filename_component(_t ${_item} NAME)
    list(APPEND EMBEDDED_BINARIES_LIB ${_t})
endforeach()

set(EMBEDDED_BINARIES_LIB_USD)
file(GLOB_RECURSE temp "${PXR_CMAKE_DIR}/lib/usd/*.*")
foreach (_item ${temp}) # Iterate over list of absolute paths
    file(RELATIVE_PATH _t "${PXR_CMAKE_DIR}/lib/" ${_item})
    list(APPEND EMBEDDED_BINARIES_LIB_USD ${_t})
endforeach()

set(PATHS "@executable_path/plugin/usd @executable_path/lib @executable_path/lib/usd")

set(PLIST_TEMPLATE ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.in)

set_target_properties(${PROJECT_NAME} PROPERTIES
        MACOSX_BUNDLE TRUE
        RESOURCE "${RESOURCES}"
        MACOSX_BUNDLE_INFO_PLIST ${PLIST_TEMPLATE}
        PUBLIC_HEADER "${HYD_PUBLIC_HEADERS}"
        XCODE_ATTRIBUTE_LD_RUNPATH_SEARCH_PATHS "${PATHS}"
        XCODE_ATTRIBUTE_ASSETCATALOG_COMPILER_APPICON_NAME "AppIcon"
        XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.apple.sample-code.HydraApps"
        XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC YES)

set(LIB_FILE_MASK "*.dylib")

add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
        # Create destination directory structure
        COMMAND ${CMAKE_COMMAND} -E make_directory
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/lib
        COMMAND ${CMAKE_COMMAND} -E make_directory
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/plugin
        COMMAND ${CMAKE_COMMAND} -E make_directory
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/plugin/usd

        # Copy the default USD dylibs into the package
        COMMAND ${CMAKE_COMMAND} -E copy
        ${PXR_CMAKE_DIR}/lib/${LIB_FILE_MASK} $<TARGET_FILE_DIR:${PROJECT_NAME}>/lib

        # Copy the hydra dylib plugins to the package too
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${PXR_CMAKE_DIR}/plugin/usd $<TARGET_FILE_DIR:${PROJECT_NAME}>/plugin/usd

        # And finally, copy the usd plugin resources into the package
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${PXR_CMAKE_DIR}/lib/usd $<TARGET_FILE_DIR:${PROJECT_NAME}>/lib/usd
)

if (DEFINED ENV{HYDRA_PLAYER_FORCE_AD_HOC})
    set(CMAKE_XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "-")
    set(CODE_SIGN_IDENTITY "-")
    set(CMAKE_XCODE_ATTRIBUTE_DEVELOPMENT_TEAM "")
elseif (DEFINED ENV{XCODE_ATTRIBUTE_CODE_SIGN_ID} AND NOT "$ENV{XCODE_ATTRIBUTE_CODE_SIGN_ID}" STREQUAL "-")
    execute_process(COMMAND security find-identity -v -p codesigning OUTPUT_VARIABLE codeSignIDs)
    string(REGEX MATCHALL $ENV{XCODE_ATTRIBUTE_CODE_SIGN_ID} ID_MATCHES "${codeSignIDs}")
    list(LENGTH ID_MATCHES NUM_MATCHES)

    if (NOT NUM_MATCHES EQUAL 1)
        message(FATAL_ERROR "Unable to identify code signing identity $ENV{XCODE_ATTRIBUTE_CODE_SIGN_ID}. "
                "Please specify by setting the XCODE_ATTRIBUTE_CODE_SIGN_ID environment "
                "variable to the one you'd like to use. Options are:\n${codeSignIDs}")
    endif()

    set(CODE_SIGN_IDENTITY $ENV{XCODE_ATTRIBUTE_CODE_SIGN_ID})
else()
    set(CODE_SIGN_IDENTITY "-")
endif()

set(RESOURCE "${RESOURCES}")

foreach (_item ${EMBEDDED_BINARIES_LIB})
    add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD COMMAND codesign --remove-signature $<TARGET_FILE_DIR:${PROJECT_NAME}>/lib/${_item})
    add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD COMMAND codesign -f -s ${CODE_SIGN_IDENTITY} $<TARGET_FILE_DIR:${PROJECT_NAME}>/lib/${_item})
endforeach()

foreach (_item ${EMBEDDED_BINARIES_LIB_USD})
    add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD COMMAND codesign --remove-signature $<TARGET_FILE_DIR:${PROJECT_NAME}>/lib/${_item})
    add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD COMMAND codesign -f -s ${CODE_SIGN_IDENTITY} $<TARGET_FILE_DIR:${PROJECT_NAME}>/lib/${_item})
endforeach()

foreach (_item ${EMBEDDED_BINARIES_PLUGIN})
    add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD COMMAND codesign --remove-signature $<TARGET_FILE_DIR:${PROJECT_NAME}>/plugin/${_item})
    add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD COMMAND codesign -f -s ${CODE_SIGN_IDENTITY} $<TARGET_FILE_DIR:${PROJECT_NAME}>/plugin/${_item})
endforeach()