#include "I2CDevice.h"
#include "I2CBus.h"

#define LOG_LEVEL LOG_INFO_LEVEL
#include "utils/Logger.h"
#include "utils/Guard.h"

using namespace FC;
using namespace FC::HAL;
using namespace FC::HAL::Linux;

I2CDevice::I2CDevice(std::weak_ptr<I2CBus> bus)
    : bus_(std::move(bus)),
      address_(0)
{
}

I2CDevice::~I2CDevice()
{
    timer_.stop();
}

FCReturnCode I2CDevice::setAddress(uint8_t address)
{
    address_ = address;
    return SUCCESS;
}

FCReturnCode I2CDevice::getAddress(uint8_t &address)
{
    address = address_;
    return SUCCESS;
}

FCReturnCode I2CDevice::transfer(const uint8_t *tx_buffer, uint32_t tx_size,
                                 uint8_t *rx_buffer, uint32_t rx_size)
{
    // Lock the bus to get the shared_ptr to I2CBus
    std::shared_ptr<I2CBus> bus = bus_.lock();

    CHECK_PRINT_RET(bus == nullptr, FAILED,
                    "I2C bus is not available for transfer");

    return bus->transfer(address_, tx_buffer, tx_size, rx_buffer, rx_size);
}

FCReturnCode I2CDevice::registerPeriodicCallback(Functor callback, 
                                                 void *context,
                                                 uint32_t interval_ns)
{
    if (timer_.getFd() < 0)
    {
        // Timer for this I2C device has not started yet
        CHECK_RET(timer_.start(interval_ns) != SUCCESS, FAILED);
    }

    std::shared_ptr<I2CBus> bus = bus_.lock();
    CHECK_PRINT_RET(bus == nullptr, FAILED, 
                    "The I2C device was not created by I2CManager");

    // Register the periodic callback to the I2C bus
    if (bus->registerPeriodicCallback(timer_.getFd(), callback, context) != SUCCESS)
    {
        timer_.stop();
        return FAILED;
    }

    return SUCCESS;
}