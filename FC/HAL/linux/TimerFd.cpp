#include "TimerFd.h"

#include <cstring>

#include <errno.h>
#include <unistd.h>
#include <sys/timerfd.h>

#define LOG_LEVEL LOG_INFO_LEVEL
#include "utils/Types.h"
#include "utils/Guard.h"

using namespace FC;
using namespace FC::HAL;
using namespace FC::HAL::Linux;

TimerFd::TimerFd()
    : fd_(-1)
{
}

TimerFd::~TimerFd()
{
    stop();
}

FCReturnCode TimerFd::start(uint32_t intervalNs)
{
    fd_ = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    CHECK_PRINT_RET(fd_ < 0, FAILED, 
                    "Failed to create timerfd instance: %s", strerror(errno));

    if (setInterval(intervalNs) != SUCCESS)
    {
        ::close(fd_);
        fd_ = -1;

        return FAILED;
    }

    return SUCCESS;
}

FCReturnCode TimerFd::setInterval(uint32_t intervalNs)
{
    CHECK_PRINT_RET(fd_ < 0, FAILED, "The timerfd instance is not created yet");

    struct itimerspec interval;

    // Interval for the first expiration
    interval.it_value.tv_sec = 0;
    interval.it_value.tv_nsec = intervalNs; 
    
    // Interval for the next expirations after the first expiration
    interval.it_interval.tv_sec = 0;
    interval.it_interval.tv_nsec = intervalNs;

    int ret = ::timerfd_settime(fd_, 0, &interval, nullptr);
    CHECK_PRINT_RET(ret < 0, FAILED, 
                    "Failed to set interval for timerfd instance: %s", strerror(errno));

    return SUCCESS;
}

void TimerFd::stop()
{
    if (fd_ >= 0)
        ::close(fd_);
    
    fd_ = -1;
}