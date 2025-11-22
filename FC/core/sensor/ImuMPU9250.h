#pragma once

#include <memory>

#include "ImuBackend.h"
#include "ImuDef.h"
#include "HAL/base/Device.h"

namespace FC
{

class ImuMPU9250 : public ImuBackend
{
public:
    ImuMPU9250(ImuDef::PhyInterface phy_interface, ImuDef::BusNumber bus_number);
    ~ImuMPU9250();

    FCReturnCode init() override;
    FCReturnCode reset() override;
    uint8_t whoAmI() override;
    
private:
    FCReturnCode createDevice();

    std::unique_ptr<HAL::Base::Device> dev_;
    ImuDef::PhyInterface phy_interface_;
    ImuDef::BusNumber bus_number_;
};

}