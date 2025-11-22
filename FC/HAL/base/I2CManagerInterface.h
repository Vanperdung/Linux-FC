#pragma once

#include <memory>

namespace FC
{
namespace HAL
{
namespace Base
{

class DeviceInterface;

class I2CManagerInterface
{
public:
    I2CManagerInterface() = default;
    virtual ~I2CManagerInterface() = default;
    virtual std::unique_ptr<DeviceInterface> createDevice(int bus_number) = 0;
};

}
}
}
