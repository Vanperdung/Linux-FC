#include "I2CDevice.h"

using namespace FC;
using namespace FC::HAL;
using namespace FC::HAL::Linux;

I2CDevice::I2CDevice(std::weak_ptr<I2CBus> bus)
    : bus_(std::move(bus))
{
}