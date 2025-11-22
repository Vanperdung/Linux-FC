#include "../BoardDescriptor.h"

#include <unistd.h>

#include "utils/Logger.h"

using namespace FC;

const BoardDescriptor &BoardDescriptor::getInstance()
{
    static const BoardDescriptor instance = []() -> BoardDescriptor
    {
        BoardDescriptor instance;

        instance.imu.type = ImuDef::Type::MPU9250;
        instance.imu.phy_interface = ImuDef::PhyInterface::I2C;
        instance.imu.bus_number = ImuDef::BusNumber::BUS0;

        Logger::registerLogFunc([](uint8_t *buffer, uint16_t n) {
            write(STDOUT_FILENO, buffer, n);
        });

        return instance;
    }();

    return instance;
}
