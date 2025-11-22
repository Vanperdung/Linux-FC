#include "ImuSensor.h"

#include "ImuBackend.h"
#include "ImuBackendFactory.h"
#include "board/BoardDescriptor.h"

#include "utils/Logger.h"
#include "utils/Guard.h"

using namespace FC;

ImuSensor::ImuSensor()
    : backend_(nullptr)
{
}

ImuSensor::~ImuSensor()
{
}

FCReturnCode ImuSensor::init()
{
    CHECK_RET(initBackend() != SUCCESS, FAILED);

    return SUCCESS;
}

FCReturnCode ImuSensor::cleanup()
{

    return SUCCESS;
}

FCReturnCode ImuSensor::initBackend()
{
    ImuDef::Type backend_type = BoardDescriptor::getInstance().imu.type;
    CHECK_PRINT_RET(backend_type == ImuDef::Type::NONE, FAILED,
                    "Invalid IMU backend type");

    ImuBackendFactoryBase *backend_factory = ImuBackendFactoryBase::getFactoryInstance(backend_type);
    CHECK_PRINT_RET(backend_factory == nullptr, FAILED, "Failed to find IMU backend factory");

    ImuDef::PhyInterface phy_interface = BoardDescriptor::getInstance().imu.phy_interface;
    ImuDef::BusNumber bus_number = BoardDescriptor::getInstance().imu.bus_number;

    backend_ = backend_factory->createInstance(phy_interface, bus_number);
    CHECK_PRINT_RET(backend_ == nullptr, FAILED, "Failed to create IMU backend");

    CHECK_PRINT_RET(backend_->init() != SUCCESS, FAILED, 
                    "Failed to initialize IMU backend");
    
    return SUCCESS;
}