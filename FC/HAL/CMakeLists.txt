add_library(FC-hal INTERFACE)

if (FC_HAL_LINUX)
    add_subdirectory(linux)
    target_link_libraries(FC-hal INTERFACE
        FC-linux
    )
endif()
