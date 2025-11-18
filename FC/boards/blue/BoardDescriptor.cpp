#include "../BoardDescriptor.h"

using namespace FC;

const BoardDescriptor &BoardDescriptor::getInstance()
{
    static const BoardDescriptor instance = []() -> BoardDescriptor {
        BoardDescriptor instance;

        instance.imu = ImuSensorBackend::MPU9250;

        return instance;
    }();

    return instance;
}

