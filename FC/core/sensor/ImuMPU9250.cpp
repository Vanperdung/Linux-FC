#include "ImuMPU9250.h"

#include "ImuBackendFactory.h"
#include "ImuMPUXXXXRegs.h"
#include "HAL/base/I2CManager.h"

#include "utils/Logger.h"
#include "utils/Guard.h"

using namespace FC;

ImuMPU9250::ImuMPU9250(ImuDef::PhyInterface phy_interface,
                       ImuDef::BusNumber bus_number)
    : dev_(nullptr),
      phy_interface_(phy_interface),
      bus_number_(bus_number)
{
}

ImuMPU9250::~ImuMPU9250()
{
}

FCReturnCode ImuMPU9250::init()
{
    CHECK_RET(createDevice() != SUCCESS, FAILED);

    return SUCCESS;
}

FCReturnCode ImuMPU9250::createDevice()
{
    unsigned int bus_number = static_cast<unsigned int>(bus_number_);

    // Ensure that we're creating supported physical interface
    if (phy_interface_ == ImuDef::PhyInterface::I2C)
    {
        dev_ = HAL::Base::I2CManager::getInstance()->createDevice(bus_number);
    }
    else if (phy_interface_ == ImuDef::PhyInterface::SPI)
    {
        // TODO
        LOG_WARNING("Not support SPI interface yet");
    }
    else
    {
        LOG_ERROR("Unsupported physical interface");
        return FAILED;
    }

    CHECK_PRINT_RET(dev_ == nullptr, FAILED,
                    "Failed to create interface device on bus %d", bus_number);

    LOG_INFO("Created interface device successfully on bus %d", bus_number);

    return SUCCESS;
}

FCReturnCode ImuMPU9250::reset()
{
    return SUCCESS;
}

uint8_t ImuMPU9250::whoAmI()
{
    return 0;
}

REGISTER_IMU_BACKEND(ImuMPU9250, ImuDef::Type::MPU9250)
