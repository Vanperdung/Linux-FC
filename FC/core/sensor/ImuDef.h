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

    enum class PhyInterface
    {
        NONE = 0,
        SPI,
        I2C
    };

    enum class BusNumber
    {
        BUS0 = 0,
        BUS1,
        BUS2,
        BUS3,
    };

    Type type;
    PhyInterface phy_interface;
    BusNumber bus_number;
};

}
