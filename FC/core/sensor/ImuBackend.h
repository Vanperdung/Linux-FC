#pragma once

#include <cstdint>

#include "utils/Types.h"
#include "utils/Vector3.h"

namespace FC
{

struct ImuSample
{
    Vector3f accel;
    Vector3f gyro;
    Vector3f mag;
};

class ImuBackend
{
public:
    ImuBackend() = default;
    virtual ~ImuBackend() = default;

    virtual FCReturnCode init() = 0;
    virtual FCReturnCode reset() = 0;
    virtual uint8_t whoAmI() = 0;

protected:
    virtual FCReturnCode calibrate() { return UNSUPPORTED; }

    
};

}