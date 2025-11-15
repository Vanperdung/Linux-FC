#pragma once

#include <cstdint>

#include "utils/Types.h"

namespace FC
{
namespace HAL
{
namespace Linux
{

class TimerFd
{
public:
    TimerFd();
    ~TimerFd();

    FCReturnCode start(uint32_t intervalNs);
    void stop();
    FCReturnCode setInterval(uint32_t intervalNs);

    int getFd() const { return fd_; }

private:
    int fd_;
};

}
}
}