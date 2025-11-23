#pragma once

#include <memory>

#include "utils/Types.h"

namespace FC
{

class ImuBackend;

class ImuSensor
{
public:
    ImuSensor();
    ~ImuSensor();

    FCReturnCode init();
    FCReturnCode cleanup();

private:
    static void periodicGetSampleCallback(void *context);
    FCReturnCode initBackend();

    std::unique_ptr<ImuBackend> backend_;
};

}
