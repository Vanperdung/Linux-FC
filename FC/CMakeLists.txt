add_library(FC-inc INTERFACE)

target_include_directories(FC-inc INTERFACE 
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_subdirectory(HAL)
add_subdirectory(utils)

add_library(FC-libs INTERFACE)

target_link_libraries(FC-libs INTERFACE
    # FC-core
    FC-hal
    FC-utils
    FC-inc
)
