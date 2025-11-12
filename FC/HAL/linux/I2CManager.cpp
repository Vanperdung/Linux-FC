#include "I2CManager.h"

#define LOG_LEVEL LOG_INFO_LEVEL
#include "utils/logger.h"
#include "utils/guard.h"

using namespace FC;
using namespace FC::HAL;
using namespace FC::HAL::Linux;

I2CManager &I2CManager::getInstance()
{
    static I2CManager instance;
    return instance;
}

I2CManager::I2CManager()
{
}

I2CManager::~I2CManager()
{
    buses_.clear();
}

I2CDevice *I2CManager::createDevice(int busNumber)
{
    std::shared_ptr<I2CBus> bus = nullptr;

    for (auto b : buses_)
    {
        if (b->getBusNumber() == busNumber)
        {
            bus = b;
            break;
        }
    }

    if (bus == nullptr)
    {   
        bus = std::make_shared<I2CBus>(busNumber);
        CHECK_PRINT_RET(bus == nullptr, nullptr,
                        "Failed to allocate I2C bus %d", busNumber);
        CHECK_PRINT_RET(bus->open() != SUCCESS, nullptr,
                        "Failed to open I2C bus %d", busNumber);
        buses_.emplace_back(bus);
    }

    I2CDevice *newI2CDevice = new I2CDevice(bus);
    CHECK_PRINT_RET(newI2CDevice == nullptr, nullptr,
                    "Failed to allocate I2C device on bus %d", busNumber);

    return newI2CDevice;
}