#pragma once

#include <cstdint>
#include <functional>

#include "utils/Types.h"
#include "utils/Vector3.h"

namespace FC
{

template <typename T>
struct ImuSample
{
    Vector3<T> accel;
    Vector3<T> gyro;
    Vector3<T> mag;
    float temperature;
};

class ImuBackend
{
public:
    using Functor = std::function<void(void*)>;

    ImuBackend() = default;
    virtual ~ImuBackend() = default;

    virtual FCReturnCode init() = 0;
    virtual void cleanup() = 0;
    virtual FCReturnCode reset() = 0;
    virtual uint8_t whoAmI() = 0;
    virtual FCReturnCode getSample(ImuSample<float> &sample) = 0;
    virtual FCReturnCode setSampleRate(uint16_t rate_hz) = 0;
    virtual FCReturnCode enableInterrupt(bool enable) = 0;
    virtual FCReturnCode enableSleepMode(bool enable) = 0;
    virtual FCReturnCode registerPeriodicCallback(Functor callback,
                                                  void *context,
                                                  uint32_t interval_ns) = 0;

protected:
    virtual FCReturnCode calibrate() = 0;
    virtual FCReturnCode setAddress(uint8_t address) = 0;
    virtual FCReturnCode resetFIFO() = 0;
};

}