#pragma once

namespace FC
{
namespace HAL
{
namespace Base
{

class I2CManagerInterface;
class UtilsInterface;

class HALInterface
{
public:
    HALInterface() = default;
    virtual ~HALInterface() = default;

    virtual I2CManagerInterface &getI2CManagerInstance() = 0;
    virtual UtilsInterface &getUtilsInstance() = 0;

    // Must be implemented in derived HAL classes
    static HALInterface &getInstance();
};

}
}
}
