#pragma once

#include <memory>

#include "Device.h"

namespace FC
{
namespace HAL
{
namespace Base
{

class I2CManager
{
public:
    virtual ~I2CManager() = default;
    virtual std::unique_ptr<Device> createDevice(int bus_number) = 0;

protected:
    I2CManager() = default;
};

}
}
}
