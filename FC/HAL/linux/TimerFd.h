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

    FCReturnCode start(uint32_t interval_ns);
    void stop();
    FCReturnCode setInterval(uint32_t interval_ns);

    int getFd() const { return fd_; }

private:
    int fd_;
};

}
}
}
