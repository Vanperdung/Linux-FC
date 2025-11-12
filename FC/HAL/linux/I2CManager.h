#pragma once

#include <vector>
#include <memory>

#include "I2CDevice.h"
#include "I2CBus.h"
#include "utils/types.h"

namespace FC
{
namespace HAL
{
namespace Linux
{

class I2CManager
{
public:
    static I2CManager &getInstance();
    I2CManager(const I2CManager &) = delete;
    I2CManager &operator=(const I2CManager &) = delete;

    I2CDevice *createDevice(int busNumber);

private:
    I2CManager();
    ~I2CManager();

    std::vector<std::shared_ptr<I2CBus>> buses_;
};

}
}
}