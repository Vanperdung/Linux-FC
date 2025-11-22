#include "Utils.h"

#include <unistd.h>

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
    usleep(ms * 1000);
}
