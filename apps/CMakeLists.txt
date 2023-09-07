#  Copyright (c) 2023 Feng Yang
#
#  I am making my contributions/submissions to this project solely in my
#  personal capacity and am not conveying any rights to any intellectual
#  property of any third parties.

add_executable(apps
        main.cpp
        primitive_app.cpp
)

target_link_libraries(apps
        metal-cpp
        framework
        application
)