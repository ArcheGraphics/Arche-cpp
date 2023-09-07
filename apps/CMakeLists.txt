#  Copyright (c) 2023 Feng Yang
#
#  I am making my contributions/submissions to this project solely in my
#  personal capacity and am not conveying any rights to any intellectual
#  property of any third parties.

add_executable(apps
        main.cpp
        delegate.cpp
        renderer.mm
)

set(PXR_LIBRARY_NAMES)
foreach (_item ${PXR_LIBRARIES}) # Iterate over list of absolute paths
    get_filename_component(_t ${_item} NAME)
    list(APPEND PXR_LIBRARY_NAMES ${_t})
endforeach()

target_link_libraries(apps
        metal-cpp
        framework
        ${PXR_LIBRARY_NAMES}
)