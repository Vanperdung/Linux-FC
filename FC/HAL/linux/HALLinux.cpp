#include "HALLinux.h"

#include "I2CManager.h"
#include "Utils.h"

using namespace FC;
using namespace FC::HAL;
using namespace FC::HAL::Linux;

HALLinux &HALLinux::getInstance()
{
    static HALLinux instance;
    return instance;
}

Base::I2CManagerInterface &HALLinux::getI2CManagerInstance()
{
    return I2CManager::getInstance();
}

Base::UtilsInterface &HALLinux::getUtilsInstance()
{
    return Utils::getInstance();
}

Base::HALInterface &Base::HALInterface::getInstance()
{
    return HALLinux::getInstance();
}
