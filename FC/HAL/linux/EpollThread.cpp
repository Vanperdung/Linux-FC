#include "EpollThread.h"

#include <cstring>
#include <algorithm>

#include <unistd.h>
#include <errno.h>
#include <sys/epoll.h>

#define LOG_LEVEL LOG_INFO_LEVEL
#include "utils/Logger.h"
#include "utils/Guard.h"

#define SLOT_CAPACITY 32

using namespace FC;
using namespace FC::HAL;
using namespace FC::HAL::Linux;

EpollThread::EpollThread()
    : Thread(std::bind(&EpollThread::poll, this)),
      fd_(-1),
      pollShouldStop_(false),
      numRegisteredSlots_(0)
{
}

EpollThread::~EpollThread()
{
    stop();
}

FCReturnCode EpollThread::start()
{
    fd_ = ::epoll_create1(0);
    CHECK_PRINT_RET(fd_ < 0, FAILED, "Failed to create epoll instance");

    // Create a new thread to run the demultiplexer loop
    pollShouldStop_ = false;
    CHECK_PRINT_RET(Thread::start() != SUCCESS, FAILED, "Failed to start thread");

    return SUCCESS;
}

void EpollThread::poll()
{
    while (pollShouldStop_ == false)
    {
        struct epoll_event revents[SLOT_CAPACITY] = {};

        int numEvents_ = -1;
        do
        {
            /**
             * FIXME: Need to implement a way to unblock the epoll_wait().
             * When the program does cleanup, it has to wait for the epoll_wait
             * to stop the thread. This issue will be fixed in the future.
             */
            numEvents_ = ::epoll_wait(fd_, revents, SLOT_CAPACITY, -1);
        } while (numEvents_ < 0 && errno == EINTR);

        if (numEvents_ < 0)
        {
            LOG_WARNING("Unexpected behavior: ", strerror(errno));
            return;
        }

        for (int i = 0; i < numEvents_; i++)
        {
            auto it = slotMap_.find(revents[i].data.fd);
            if (it == slotMap_.end())
                continue;

            EpollSlot &slot = it->second;
            if (revents[i].events & EPOLLIN)
                slot.onCanRead();
            else if (revents[i].events & EPOLLOUT)
                slot.onCanWrite();
            else if (revents[i].events & EPOLLERR)
                slot.onError();
            else if (revents[i].events & EPOLLHUP)
                slot.onHangUp();
        }
    }
}

FCReturnCode EpollThread::stop()
{
    // Stop the demultiplexer loop before cleanup the epoll
    pollShouldStop_ = true;
    CHECK_PRINT_RET(Thread::join() != SUCCESS, FAILED, "Failed to join thread");

    if (fd_ >= 0)
        ::close(fd_);

    fd_ = -1;

    return SUCCESS;
}

FCReturnCode EpollThread::registerSlot(int slotFd, EpollSlot slot)
{
    CHECK_PRINT_RET(numRegisteredSlots_ >= SLOT_CAPACITY - 1, FAILED,
                    "Reached the maximum of slot");
    CHECK_PRINT_RET(slotFd < 0, FAILED, "Invalid fd: %d", slotFd);

    if (slotMap_.find(slotFd) == slotMap_.end())
    {
        CHECK_RET(addEvent(slotFd, slot) != SUCCESS, FAILED);

        slotMap_.emplace(slotFd, std::move(slot));
        numRegisteredSlots_++;
        return SUCCESS;
    }

    LOG_WARNING("The slot instance (fd: %d) exists", slotFd);

    return FAILED;
}

FCReturnCode EpollThread::unregisterSlot(int slotFd)
{
    CHECK_PRINT_RET(slotFd < 0, FAILED, "Invalid fd: %d", slotFd);

    if (slotMap_.find(slotFd) != slotMap_.end())
    {
        CHECK_RET(removeEvent(slotFd) != SUCCESS, FAILED);

        slotMap_.erase(slotFd);
        numRegisteredSlots_++;
        return SUCCESS;
    }

    LOG_WARNING("The slot instance (fd: %d) does not exist", slotFd);

    return SUCCESS;
}

FCReturnCode EpollThread::addEvent(int slotFd, const EpollSlot &slot)
{
    uint32_t events = 0;

    if (slot.readCb_ != nullptr)
        events |= EPOLLIN;

    if (slot.writeCb_ != nullptr)
        events |= EPOLLOUT;

    /*
     * EPOLLWAKEUP prevents the system from hibernating or suspending when
     * inside epoll_wait() for this particular event.  It is silently
     * ignored if the process does not have the CAP_BLOCK_SUSPEND
     * capability.
     */
    events |= EPOLLWAKEUP;

    struct epoll_event epev = {};
    epev.data.fd = slotFd;
    epev.events = events;

    CHECK_PRINT_RET(::epoll_ctl(fd_, EPOLL_CTL_ADD, slotFd, &epev) != 0, FAILED,
                    "Failed to add event to epoll: %s", strerror(errno));

    return SUCCESS;
} 

FCReturnCode EpollThread::removeEvent(int slotFd)
{
    CHECK_PRINT_RET(::epoll_ctl(fd_, EPOLL_CTL_DEL, slotFd, nullptr) != 0, FAILED,
                    "Failed to remove event to epoll: %s", strerror(errno));

    return SUCCESS;
}

EpollSlot::EpollSlot(Functor readCb, Functor writeCb,
                     Functor errorCb, Functor hangUpCb)
    : readCb_(readCb),
      writeCb_(writeCb),
      errorCb_(errorCb),
      hangUpCb_(hangUpCb)
{
}

EpollSlot::~EpollSlot()
{
}

FCReturnCode EpollSlot::onCanRead()
{
    if (readCb_ != nullptr)
        readCb_();

    return SUCCESS;
}

FCReturnCode EpollSlot::onCanWrite()
{
    if (writeCb_ != nullptr)
        writeCb_();

    return SUCCESS;
}

FCReturnCode EpollSlot::onError()
{
    if (errorCb_ != nullptr)
        errorCb_();

    return SUCCESS;
}

FCReturnCode EpollSlot::onHangUp()
{
    if (hangUpCb_ != nullptr)
        hangUpCb_();

    return SUCCESS;
}
