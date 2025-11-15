#pragma once

#include <cstdint>
#include <functional>

#include "Mutex.h"
#include "EpollThread.h"
#include "utils/Types.h"

namespace FC
{
namespace HAL
{
namespace Linux
{

/**
 * @brief A dedicated class to manage I2C bus operations on Linux.
 * If Other instances (e.g. I2CDevice) want to perform I2C operations, they must
 * use the functions provided by this class.
 */
class I2CBus
{
public:
    using Functor = std::function<void(void*)>;

    I2CBus(int busNumber = -1);
    ~I2CBus();

    FCReturnCode open();
    FCReturnCode close();

    int getBusNumber() const { return busNumber_; }

    FCReturnCode acquire() { return lock_.lock(); }
    FCReturnCode release() { return lock_.unlock(); }

    /**
     * @brief This function performs an I2C transfer to the specified target address.
     * An I2C transfer can consist of a TX operation followed by a RX operation.
     * If txBuffer and txSize are non-zero, a TX operation is performed.
     * If rxBuffer and rxSize are non-zero, a RX operation is performed.
     * If none of TX or RX is performed, the function returns FCReturnCode::FAILED.
     * 
     * @param targetAddress The I2C address of the target device.
     * @param txBuffer Pointer to the data to be transmitted.
     * @param txSize Size of the data to be transmitted in bytes.
     * @param rxBuffer Pointer to the buffer to store received data.
     * @param rxSize Size of the data to be received in bytes.
     * @return FCReturnCode indicating success or failure of the operation.
     */
    FCReturnCode transfer(uint8_t targetAddress, 
                          const uint8_t *txBuffer, uint32_t txSize, 
                          uint8_t *rxBuffer, uint32_t rxSize);

    FCReturnCode registerPeriodicCallback(int timerFd,
                                          Functor callback,
                                          void *context);
    
private:
    int busNumber_;
    int fd_;
    Mutex lock_;
    EpollThread poller_;
};

}
}
}