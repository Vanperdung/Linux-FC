#include "Thread.h"

#include <cstring>

#define LOG_LEVEL LOG_INFO_LEVEL
#include "utils/Logger.h"
#include "utils/Guard.h"

using namespace FC;
using namespace FC::HAL;
using namespace FC::HAL::Linux;

Thread::Thread(Functor actualTask)
    : thread_(0),
      running_(false),
      actualTask_(std::move(actualTask))
{
}

Thread::~Thread()
{
    join();
}

void *Thread::routine(void *arg)
{
    Thread *thread = reinterpret_cast<Thread *>(arg);
    if (thread && thread->actualTask_)
        thread->actualTask_();
    
    return nullptr;
}

FCReturnCode Thread::start()
{
    CHECK_PRINT_RET(!actualTask_, FAILED, "No task assigned to thread");
    CHECK_PRINT_RET(running_, FAILED, "Thread is already running");

    int ret = pthread_create(&thread_, nullptr, &Thread::routine, this);
    CHECK_PRINT_RET(ret != 0, FAILED, "Failed to create thread: %s", strerror(ret));
    
    running_ = true;

    return SUCCESS;
}

FCReturnCode Thread::join()
{
    void *ret = nullptr;

    if (running_ == true)
    {
        if (pthread_join(thread_, &ret) != 0)
        {
            LOG_WARNING("Failed to join thread");
            return FAILED;
        }

        running_ = false;
        return SUCCESS;
    }   

    return SUCCESS;
}