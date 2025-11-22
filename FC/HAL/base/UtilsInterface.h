#pragma once

#include <cstdint>

namespace FC
{
namespace HAL
{
namespace Base
{

class UtilsInterface
{
public:
    UtilsInterface() = default;
    virtual ~UtilsInterface() = default;

    virtual void delayMs(uint16_t ms) = 0;
};

}
}
}
