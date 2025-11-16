#include "I2CManager.h"

#define LOG_LEVEL LOG_INFO_LEVEL
#include "utils/Logger.h"
#include "utils/Guard.h"

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
    cleanup();
}

std::unique_ptr<Base::Device> I2CManager::createDevice(int busNumber)
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

    /**
     * By default, std::make_unique will throw a std::bad_alloc exception on 
     * allocation failure. Using std::nothrow to ignore it.
     */
    std::unique_ptr<Base::Device> device = 
            std::unique_ptr<Base::Device>(new(std::nothrow) I2CDevice(bus));
    CHECK_PRINT_RET(!device, nullptr, "Failed to allocate I2C device on bus %d", busNumber);

    return device;
}

FCReturnCode I2CManager::cleanup()
{
    for (auto bus : buses_)
        CHECK_RET(bus->close() != SUCCESS, FAILED);

    buses_.clear();

    return SUCCESS;
}