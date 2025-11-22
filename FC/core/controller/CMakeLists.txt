set(FC_CORE_SRCS
    Controller.cpp
)

add_library(FC-controller STATIC
    ${FC_CORE_SRCS}
)

target_link_libraries(FC-controller PRIVATE
    FC-board
    FC-inc
    -Wl,--whole-archive FC-sensor -Wl,--no-whole-archive
    FC-utils
)
