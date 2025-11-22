#include "Test.h"

#include "core/sensor/ImuSensor.h"
#include "utils/Types.h"

using namespace FC;

class ImuSensorTest : public Test
{
public:
    bool init() override 
    { 
        CHECK_RET(imu_.init() != SUCCESS, false);

        return true; 
    }

    bool run() override 
    { 
        return true; 
    }

    bool cleanup() override
    { 
        return true; 
    }

private:
    ImuSensor imu_;
};

REGISTER_TEST(ImuSensorTest)
