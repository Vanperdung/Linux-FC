#pragma once

#include <vector>
#include <memory>

#include "I2CDevice.h"
#include "I2CBus.h"
#include "utils/Types.h"

namespace FC
{
namespace HAL
{
namespace Linux
{

/**
 * @brief A singleton class to manage creation/deletion of I2CBus.
 * The I2CDevice instances are only created by I2CManager, if not it cannot
 * access the I2CBus instance for the communitation with off-chip peripherals.
 */
class I2CManager
{
public:
    static I2CManager &getInstance();
    I2CManager(const I2CManager &) = delete;
    I2CManager &operator=(const I2CManager &) = delete;

    std::unique_ptr<Base::Device> createDevice(int bus_number);
    FCReturnCode cleanup();

private:
    I2CManager();
    ~I2CManager();

    std::vector<std::shared_ptr<I2CBus>> buses_;
};

}
}
}