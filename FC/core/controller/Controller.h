#pragma once

#include <memory>

#include "core/sensor/ImuSensor.h"

namespace FC
{

class Controller
{
public:
    Controller();
    ~Controller();

private:
    std::unique_ptr<ImuSensor> imu_; 
};

}
