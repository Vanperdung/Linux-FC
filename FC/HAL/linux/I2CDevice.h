#pragma once

#include <memory>

#include "HAL/base/Device.h"

namespace FC
{
namespace HAL
{
namespace Linux
{

class I2CBus;

class I2CDevice : public Base::Device
{
public:
    I2CDevice(std::weak_ptr<I2CBus> bus);
    ~I2CDevice() = default;

private:
    std::weak_ptr<I2CBus> bus_; // Use weak_ptr to avoid circular reference
};

}
}
}