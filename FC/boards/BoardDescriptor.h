#pragma once

namespace FC
{

enum class ImuSensorBackend
{
    NONE = 0,
    MPU9250,
    MPU6050,
    MPU6000
};

struct BoardDescriptor
{
    static const BoardDescriptor &getInstance();
    ImuSensorBackend imu;
};

}