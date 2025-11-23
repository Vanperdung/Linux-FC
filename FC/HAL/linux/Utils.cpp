#include "Utils.h"

#include <time.h>

using namespace FC;
using namespace FC::HAL;
using namespace FC::HAL::Linux;

Utils &Utils::getInstance()
{
    static Utils instance;
    return instance;
}

void Utils::delayMs(uint16_t ms)
{
    struct timespec delay = {};
    delay.tv_sec = ms / 1000;
    delay.tv_nsec = (ms % 1000) * 1000000;
    nanosleep(&delay, nullptr);
}

void Utils::getTimestamp(std::string &timestamp)
{
    struct timespec ts = {};
    clock_gettime(CLOCK_MONOTONIC, &ts);
    timestamp = std::to_string(static_cast<uint64_t>(ts.tv_sec)) + "." +
                std::to_string(static_cast<uint64_t>(ts.tv_nsec) / 1000000);
}

