#pragma once

#include <cstdint>
#include <functional>

#include "utils/types.h"

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

    using PeriodCallback = std::function<FCReturnCode(void*)>;

    Device() = default;
    virtual ~Device() = default;

    virtual FCReturnCode lock() { return UNSUPPORTED; }
    virtual FCReturnCode unlock() { return UNSUPPORTED; }
    
    virtual FCReturnCode transfer(const uint8_t *txBuffer, uint32_t txSize, 
                                  uint8_t *rxBuffer, uint32_t rxSize)
                                  { return UNSUPPORTED; }
    virtual FCReturnCode transferFullDuplex(const uint8_t *txBuffer, 
                                            uint8_t *rxBuffer, uint32_t size)
                                            { return UNSUPPORTED; }
    virtual FCReturnCode registerPeriodCallback(PeriodCallback callback, void *context)
                                            { return UNSUPPORTED; }

    virtual FCReturnCode setSpeed(Speed speed) { return UNSUPPORTED; }
    virtual FCReturnCode setAddress(uint8_t address) { return UNSUPPORTED; }
    virtual FCReturnCode getAddress(uint8_t &address) { return UNSUPPORTED; }
    virtual FCReturnCode setChipSelect(bool set) { return UNSUPPORTED; }
};

}
}
}