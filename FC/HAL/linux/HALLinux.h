#pragma once

#include "HAL/base/HALInterface.h"
#include "HAL/base/I2CManagerInterface.h"

namespace FC
{
namespace HAL
{
namespace Linux
{

class HALLinux : public Base::HALInterface
{
public:
    HALLinux(const HALLinux&) = delete;
    HALLinux& operator=(const HALLinux&) = delete;

    static HALLinux &getInstance();

    Base::I2CManagerInterface &getI2CManagerInstance() override;
    Base::UtilsInterface &getUtilsInstance() override;
private:
    HALLinux() = default;
    ~HALLinux() = default;
};

}
}
}