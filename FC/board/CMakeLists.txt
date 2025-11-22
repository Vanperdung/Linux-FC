if (FC_BOARD_BLUE)
    set(FC_BOARD_SRCS 
        blue/BoardDescriptor.cpp
    )

    set(FC_HAL_LINUX ON PARENT_SCOPE)
endif()

add_library(FC-board STATIC
    ${FC_BOARD_SRCS}
)

target_link_libraries(FC-board PRIVATE
    FC-inc
    FC-utils
)
