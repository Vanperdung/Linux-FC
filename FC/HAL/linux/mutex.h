#pragma once

#include <pthread.h>

#include "utils/types.h"

namespace FC
{
namespace HAL
{
namespace Linux
{

class Mutex
{
public:
    Mutex();
    ~Mutex();

    FCReturnCode lock();
    FCReturnCode unlock();
    FCReturnCode tryLock();

private:
    pthread_mutex_t lock_;
};

}
}
}