add_subdirectory(controller)
add_subdirectory(sensor)

add_library(FC-core INTERFACE)

target_link_libraries(FC-core INTERFACE
    FC-controller
    -Wl,--whole-archive FC-sensor -Wl,--no-whole-archive
)
