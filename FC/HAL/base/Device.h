#pragma once

#include <cstdint>
#include <functional>

#include "utils/Types.h"

namespace FC
{
namespace HAL
{
namespace Base
{

class Device
{
public:
    enum Speed
    {
        SPEED_LOW,
        SPEED_HIGH
    };

    using Functor = std::function<void(void*)>;

    Device() = default;
    virtual ~Device() = default;

    virtual FCReturnCode lock() { return UNSUPPORTED; }
    virtual FCReturnCode unlock() { return UNSUPPORTED; }

    virtual FCReturnCode transfer(const uint8_t *tx_buffer, uint32_t tx_size,
                                    uint8_t *rx_buffer, uint32_t rx_size)
    {
        return UNSUPPORTED;
    }
    virtual FCReturnCode transferFullDuplex(const uint8_t *tx_buffer,
                                            uint8_t *rx_buffer, uint32_t size)
    {
        return UNSUPPORTED;
    }

    /**
     * @brief Register a periodic callback function.
     * Some I2C devices may need periodic actions, such as polling the FIFO to 
     * avoid overflow. This function allows us to register a callback function
     * that will be called periodically.
     */
    virtual FCReturnCode registerPeriodicCallback(Functor callback, 
                                                  void *context,
                                                  uint32_t interval_ns)
    {
        return UNSUPPORTED;
    }

    virtual FCReturnCode setSpeed(Speed speed) { return UNSUPPORTED; }
    virtual FCReturnCode setAddress(uint8_t address) { return UNSUPPORTED; }
    virtual FCReturnCode getAddress(uint8_t &address) { return UNSUPPORTED; }
    virtual FCReturnCode setChipSelect(bool set) { return UNSUPPORTED; }
};

}
}
}