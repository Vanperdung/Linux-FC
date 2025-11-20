#pragma once

namespace FC
{

struct ImuDef
{
    enum class Type
    {
        NONE = 0,
        MPU9250,
        MPU6050,
        MPU6000
    };

    enum class Device
    {
        NONE = 0,
        SPI,
        I2C
    };

    enum class BusNumber
    {
        NONE = 0,
        BUS0,
        BUS1,
        BUS2,
        BUS3
    };

    Type type;
    Device dev;
    BusNumber bus_number;
};

}
