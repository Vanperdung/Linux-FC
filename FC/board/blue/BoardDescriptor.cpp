#include "../BoardDescriptor.h"

using namespace FC;

const BoardDescriptor &BoardDescriptor::getInstance()
{
    static const BoardDescriptor instance = []() -> BoardDescriptor {
        BoardDescriptor instance;

        instance.imu.type = ImuDef::Type::MPU9250;
        instance.imu.dev = ImuDef::Device::I2C;
        instance.imu.bus_number = ImuDef::BusNumber::BUS0;

        return instance;
    }();

    return instance;
}

