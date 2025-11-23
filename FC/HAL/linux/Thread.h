#pragma once

#include <functional>

#include <pthread.h>

#include "utils/Types.h"

namespace FC
{
namespace HAL
{
namespace Linux
{

class Thread
{
protected:
    using Functor = std::function<void(void)>;

    Thread(Functor actual_task);
    virtual ~Thread();

    FCReturnCode start();
    FCReturnCode join();

private:
    static void *routine(void *arg);

    pthread_t thread_;
    bool running_;
    Functor actual_task_;
};

}
}
}
