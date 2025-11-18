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
      poll_should_stop_(false),
      num_registered_slots_(0)
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
    poll_should_stop_ = false;
    CHECK_PRINT_RET(Thread::start() != SUCCESS, FAILED, "Failed to start thread");

    return SUCCESS;
}

void EpollThread::poll()
{
    while (poll_should_stop_ == false)
    {
        struct epoll_event revents[SLOT_CAPACITY] = {};

        int num_events_ = -1;
        do
        {
            /**
             * FIXME: Need to implement a way to unblock the epoll_wait().
             * When the program does cleanup, it has to wait for the epoll_wait
             * to stop the thread. This issue will be fixed in the future.
             */
            num_events_ = ::epoll_wait(fd_, revents, SLOT_CAPACITY, -1);
        } while (num_events_ < 0 && errno == EINTR);

        if (num_events_ < 0)
        {
            LOG_WARNING("Unexpected behavior: %s", strerror(errno));
            return;
        }

        for (int i = 0; i < num_events_; i++)
        {
            auto it = slot_map_.find(revents[i].data.fd);
            if (it == slot_map_.end())
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
    poll_should_stop_ = true;
    CHECK_PRINT_RET(Thread::join() != SUCCESS, FAILED, "Failed to join thread");

    if (fd_ >= 0)
        ::close(fd_);

    fd_ = -1;

    return SUCCESS;
}

FCReturnCode EpollThread::registerSlot(int slot_fd, EpollSlot slot)
{
    CHECK_PRINT_RET(num_registered_slots_ >= SLOT_CAPACITY - 1, FAILED,
                    "Reached the maximum of slot");
    CHECK_PRINT_RET(slot_fd < 0, FAILED, "Invalid fd: %d", slot_fd);

    if (slot_map_.find(slot_fd) == slot_map_.end())
    {
        CHECK_RET(addEvent(slot_fd, slot) != SUCCESS, FAILED);

        slot_map_.emplace(slot_fd, std::move(slot));
        num_registered_slots_++;
        return SUCCESS;
    }

    LOG_WARNING("The slot instance (fd: %d) exists", slot_fd);

    return FAILED;
}

FCReturnCode EpollThread::unregisterSlot(int slot_fd)
{
    CHECK_PRINT_RET(slot_fd < 0, FAILED, "Invalid fd: %d", slot_fd);

    if (slot_map_.find(slot_fd) != slot_map_.end())
    {
        CHECK_RET(removeEvent(slot_fd) != SUCCESS, FAILED);

        slot_map_.erase(slot_fd);
        num_registered_slots_--;
        return SUCCESS;
    }

    LOG_WARNING("The slot instance (fd: %d) does not exist", slot_fd);

    return SUCCESS;
}

FCReturnCode EpollThread::addEvent(int slot_fd, const EpollSlot &slot)
{
    uint32_t events = 0;

    if (slot.read_cb_ != nullptr)
        events |= EPOLLIN;

    if (slot.write_cb_ != nullptr)
        events |= EPOLLOUT;

    /*
     * EPOLLWAKEUP prevents the system from hibernating or suspending when
     * inside epoll_wait() for this particular event.  It is silently
     * ignored if the process does not have the CAP_BLOCK_SUSPEND
     * capability.
     */
    events |= EPOLLWAKEUP;

    struct epoll_event epev = {};
    epev.data.fd = slot_fd;
    epev.events = events;

    CHECK_PRINT_RET(::epoll_ctl(fd_, EPOLL_CTL_ADD, slot_fd, &epev) != 0, FAILED,
                    "Failed to add event to epoll: %s", strerror(errno));

    return SUCCESS;
} 

FCReturnCode EpollThread::removeEvent(int slot_fd)
{
    CHECK_PRINT_RET(::epoll_ctl(fd_, EPOLL_CTL_DEL, slot_fd, nullptr) != 0, FAILED,
                    "Failed to remove event to epoll: %s", strerror(errno));

    return SUCCESS;
}

EpollSlot::EpollSlot(Functor read_cb, Functor write_cb,
                     Functor error_cb, Functor hangup_cb)
    : read_cb_(read_cb),
      write_cb_(write_cb),
      error_cb_(error_cb),
      hangup_cb_(hangup_cb)
{
}

EpollSlot::~EpollSlot()
{
}

FCReturnCode EpollSlot::onCanRead()
{
    if (read_cb_ != nullptr)
        read_cb_();

    return SUCCESS;
}

FCReturnCode EpollSlot::onCanWrite()
{
    if (write_cb_ != nullptr)
        write_cb_();

    return SUCCESS;
}

FCReturnCode EpollSlot::onError()
{
    if (error_cb_ != nullptr)
        error_cb_();

    return SUCCESS;
}

FCReturnCode EpollSlot::onHangUp()
{
    if (hangup_cb_ != nullptr)
        hangup_cb_();

    return SUCCESS;
}
