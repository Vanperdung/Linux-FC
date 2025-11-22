set(FC_CORE_SENSOR_SRCS
    ImuBackend.cpp
    ImuBackendFactory.cpp
    ImuMPU9250.cpp
    ImuSensor.cpp
)

add_library(FC-sensor STATIC
    ${FC_CORE_SENSOR_SRCS}
)

target_link_libraries(FC-sensor PRIVATE
    FC-board
    FC-hal
    FC-inc
    FC-utils
)
