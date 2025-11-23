#include "ImuSensor.h"

#include <string>

#include "ImuBackend.h"
#include "ImuBackendFactory.h"
#include "board/BoardDescriptor.h"
#include "HAL/base/UtilsInterface.h"
#include "HAL/base/HALInterface.h"

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

    // Get IMU backend factory instance from its type
    ImuBackendFactoryBase *backend_factory = ImuBackendFactoryBase::getFactoryInstance(backend_type);
    CHECK_PRINT_RET(backend_factory == nullptr, FAILED, "Failed to find IMU backend factory");

    ImuDef::PhyInterface phy_interface = BoardDescriptor::getInstance().imu.phy_interface;
    ImuDef::BusNumber bus_number = BoardDescriptor::getInstance().imu.bus_number;

    // Create IMU backend instance from its factory instance
    backend_ = backend_factory->createInstance(phy_interface, bus_number);
    CHECK_PRINT_RET(backend_ == nullptr, FAILED, "Failed to create IMU backend");

    CHECK_PRINT_RET(backend_->init() != SUCCESS, FAILED, 
                    "Failed to initialize IMU backend");

    // Register periodic callback to get IMU sample every 1 ms
    CHECK_PRINT_RET(backend_->registerPeriodicCallback(periodicGetSampleCallback, this, 1000000) != SUCCESS, FAILED,
                    "Failed to register periodic callback");

    return SUCCESS;
}

void ImuSensor::periodicGetSampleCallback(void *context)
{
    ImuSensor *imu_sensor = static_cast<ImuSensor*>(context);
    if (imu_sensor == nullptr || imu_sensor->backend_ == nullptr)
        return;

    ImuSample<float> sample;
    FCReturnCode ret = imu_sensor->backend_->getSample(sample);
    if (ret != SUCCESS)
    {
        LOG_WARNING("Failed to get IMU sample in periodic callback");
        return;
    }

    std::string timestamp;
    HAL::Base::HALInterface &hal = HAL::Base::HALInterface::getInstance();    
    hal.getUtilsInstance().getTimestamp(timestamp);

    LOG_INFO("[%s]: accel[%.2f, %.2f, %.2f], gyro[%.2f, %.2f, %.2f]",
             timestamp.c_str(),
             sample.accel.x, sample.accel.y, sample.accel.z,
             sample.gyro.x, sample.gyro.y, sample.gyro.z);
}