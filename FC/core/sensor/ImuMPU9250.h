#pragma once

#include <memory>

#include "ImuBackend.h"
#include "ImuDef.h"
#include "HAL/base/DeviceInterface.h"

namespace FC
{

class ImuMPU9250 : public ImuBackend
{
public:
    using Functor = std::function<void(void*)>;

    ImuMPU9250(ImuDef::PhyInterface phy_interface, ImuDef::BusNumber bus_number);
    ~ImuMPU9250();

    FCReturnCode init() override;
    void cleanup() override;
    FCReturnCode reset() override;
    uint8_t whoAmI() override;
    FCReturnCode getSample(ImuSample<float> &sample) override;
    FCReturnCode setSampleRate(uint16_t rate_hz) override;
    FCReturnCode enableInterrupt(bool enable) override;
    FCReturnCode enableSleepMode(bool enable) override;
    FCReturnCode registerPeriodicCallback(Functor callback, 
                                          void *context, 
                                          uint32_t interval_ns) override;

private:
    FCReturnCode calibrate() override;
    FCReturnCode setAddress(uint8_t address) override;
    FCReturnCode resetFIFO() override;

    FCReturnCode createDevice();
    FCReturnCode initHardware();
    FCReturnCode setClockSource(uint8_t clk_source);
    FCReturnCode setGyroFullScaleRange(uint8_t range);
    FCReturnCode setAccelFullScaleRange(uint8_t range);
    FCReturnCode enableMagnetometer(bool enable);

    /**
     * @brief Configure the gyroscope sample rate to 1 kHz
     */
    FCReturnCode configureGyroSampleRate();

    /**
     * @brief Configure the accelerometer sample rate to 1 kHz
     */
    FCReturnCode configureAccelSampleRate();

    std::unique_ptr<HAL::Base::DeviceInterface> dev_;
    ImuDef::PhyInterface phy_interface_;
    ImuDef::BusNumber bus_number_;

    // Current full scale ranges for accel and gyro
    float current_accel_fsr_;
    float current_gyro_fsr_;

    static constexpr uint32_t DEFAULT_SAMPLE_RATE_HZ = 1000;
};

}
