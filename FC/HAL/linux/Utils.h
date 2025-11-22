#pragma once

#include "HAL/base/UtilsInterface.h"

namespace FC
{
namespace HAL
{
namespace Linux
{

class Utils : public Base::UtilsInterface
{
public:
    Utils(const Utils&) = delete;
    Utils& operator=(const Utils&) = delete;

    static Utils &getInstance();

    void delayMs(uint16_t ms) override;

private:
    Utils() = default;
    ~Utils() = default;
};

}
}
}
