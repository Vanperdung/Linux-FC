#pragma once

#include <memory>

#include "TimerFd.h"
#include "HAL/base/Device.h"
#include "utils/Types.h"

namespace FC
{
namespace HAL
{
namespace Linux
{

class I2CBus;

/**
 * @brief A class representing an I2C device connected to an I2C bus.
 * All the I2C hardware in Linux-based FC platforms should be accessed via this class.
 */
class I2CDevice : public Base::Device
{
public:
    using Functor = std::function<void(void*)>;

    I2CDevice(std::weak_ptr<I2CBus> bus);
    ~I2CDevice();

    FCReturnCode setAddress(uint8_t address) override;
    FCReturnCode getAddress(uint8_t &address) override;

    FCReturnCode transfer(const uint8_t *txBuffer, uint32_t txSize,
                          uint8_t *rxBuffer, uint32_t rxSize) override;
    
    FCReturnCode registerPeriodicCallback(Functor callback, 
                                          void *context,
                                          uint32_t intervalNs) override;

private:
    std::weak_ptr<I2CBus> bus_; // Use weak_ptr to avoid circular reference
    uint8_t address_;
    TimerFd timer_;
};

}
}
}