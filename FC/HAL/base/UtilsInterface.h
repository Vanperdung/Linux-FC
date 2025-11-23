#pragma once

#include <cstdint>
#include <string>

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
    virtual void getTimestamp(std::string &timestamp) = 0;
};

}
}
}
