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

    I2CBus(int bus_number = -1);
    ~I2CBus();

    FCReturnCode open();
    FCReturnCode close();

    int getBusNumber() const { return bus_number_; }

    FCReturnCode acquire() { return lock_.lock(); }
    FCReturnCode release() { return lock_.unlock(); }

    /**
     * @brief This function performs an I2C transfer to the specified target address.
     * An I2C transfer can consist of a TX operation followed by a RX operation.
     * If tx_buffer and tx_size are non-zero, a TX operation is performed.
     * If rx_buffer and rx_size are non-zero, a RX operation is performed.
     * If none of TX or RX is performed, the function returns FCReturnCode::FAILED.
     * 
     * @param target_address The I2C address of the target device.
     * @param tx_buffer Pointer to the data to be transmitted.
     * @param tx_size Size of the data to be transmitted in bytes.
     * @param rx_buffer Pointer to the buffer to store received data.
     * @param rx_size Size of the data to be received in bytes.
     * @return FCReturnCode indicating success or failure of the operation.
     */
    FCReturnCode transfer(uint8_t target_address, 
                          const uint8_t *tx_buffer, uint32_t tx_size, 
                          uint8_t *rx_buffer, uint32_t rx_size);

    FCReturnCode registerPeriodicCallback(int slot_fd,
                                          Functor callback,
                                          void *context);
    
private:
    int bus_number_;
    int fd_;
    Mutex lock_;
    EpollThread poller_;
};

}
}
}
