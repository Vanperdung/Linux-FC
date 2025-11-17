set(FC_HAL_LINUX_SRCS
    EpollThread.cpp
    I2CBus.cpp
    I2CDevice.cpp
    I2CManager.cpp
    Mutex.cpp
    Thread.cpp
    TimerFd.cpp
)

add_library(FC-linux STATIC 
    ${FC_HAL_LINUX_SRCS}
)

target_link_libraries(FC-linux PRIVATE
    FC-inc
    FC-utils
)
