#pragma once

#include <memory>

namespace FC
{
namespace HAL
{
namespace Base
{

class Device;

class I2CManager
{
public:
    virtual ~I2CManager() = default;
    virtual std::unique_ptr<Device> createDevice(int bus_number) = 0;

    /**
     * Must be implemented by the concrete I2CManager and
     * return the address of its instance.
     *
     * FIXME: I really dislike the current HAL design. Every concrete
     * implementation has to define this method so that other components
     * can access it. Can we centralize or auto-register all instances
     * instead?
     */
    static I2CManager *getInstance();

protected:
    I2CManager() = default;
};

}
}
}
