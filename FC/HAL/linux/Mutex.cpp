#include "Mutex.h"

using namespace FC;
using namespace FC::HAL;
using namespace FC::HAL::Linux;

Mutex::Mutex()
{
    pthread_mutex_init(&lock_, nullptr);
}

Mutex::~Mutex()
{
    pthread_mutex_destroy(&lock_);
}

FCReturnCode Mutex::lock()
{
    return pthread_mutex_lock(&lock_) != 0 ? FAILED : SUCCESS;
}

FCReturnCode Mutex::unlock()
{
    return pthread_mutex_unlock(&lock_) != 0 ? FAILED : SUCCESS;
}

FCReturnCode Mutex::tryLock()
{
    return pthread_mutex_trylock(&lock_) != 0 ? FAILED : SUCCESS;
}
