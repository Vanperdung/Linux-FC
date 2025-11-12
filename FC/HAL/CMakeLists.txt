option(BUILD_HAL_LINUX "Build HAL for Linux platform" OFF)

add_library(FC-hal INTERFACE)

if (BUILD_HAL_LINUX)
    add_subdirectory(linux)
    target_link_libraries(FC-hal INTERFACE
        FC-linux
    )
endif()

