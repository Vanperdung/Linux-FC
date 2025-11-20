#pragma once

#include "core/sensor/ImuDef.h"

namespace FC
{

struct BoardDescriptor
{
    static const BoardDescriptor &getInstance();
    ImuDef imu;
};

}
