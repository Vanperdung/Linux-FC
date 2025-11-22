#include "Thread.h"

#include <cstring>

#include "utils/Logger.h"
#include "utils/Guard.h"

using namespace FC;
using namespace FC::HAL;
using namespace FC::HAL::Linux;

Thread::Thread(Functor actual_task)
    : thread_(0),
      running_(false),
      actual_task_(std::move(actual_task))
{
}

Thread::~Thread()
{
    join();
}

void *Thread::routine(void *arg)
{
    Thread *thread = reinterpret_cast<Thread *>(arg);
    if (thread && thread->actual_task_)
        thread->actual_task_();
    
    return nullptr;
}

FCReturnCode Thread::start()
{
    CHECK_PRINT_RET(!actual_task_, FAILED, "No task assigned to thread");
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