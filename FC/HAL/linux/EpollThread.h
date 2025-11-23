#pragma once

#include <unordered_map>
#include <functional>
#include <cstdint>

#include "Thread.h"

namespace FC
{
namespace HAL
{
namespace Linux
{
class EpollSlot
{
public:
    using Functor = std::function<void(void)>;

    friend class EpollThread;

    EpollSlot(Functor read_cb = nullptr, Functor write_cb = nullptr,
              Functor error_cb = nullptr, Functor hangup_cb = nullptr);
    ~EpollSlot();

    /* 
    * Called when the I/O device file becomes readable.
    * Triggered by EPOLLIN, meaning the driver has made new data available
    * or a hardware event has occurred (e.g., sensor data ready, IRQ fired).
    * The handler should read all available data to clear the device buffer.
    */
    FCReturnCode onCanRead();

    /* 
    * Called when the I/O device file becomes writable.
    * Triggered by EPOLLOUT, meaning the device driver is ready to accept
    * more data (e.g., I2C/SPI transfer completed, TX buffer is empty).
    * The handler should write pending data until write() returns EAGAIN.
    */
    FCReturnCode onCanWrite();

    /* 
    * Called when an error condition occurs on the I/O device file.
    * Triggered by EPOLLERR. This indicates a device or driver-level error,
    * such as a failed transfer, bus error, or invalid device state.
    * The handler should inspect errno or device-specific status registers.
    */
    FCReturnCode onError();

    /* 
    * Called when the I/O device file is hung up.
    * Triggered by EPOLLHUP. This usually means the device was removed,
    * reset, powered down, or the underlying driver can no longer access it.
    * No further I/O is expected; handler should clean up and deregister the FD.
    */
    FCReturnCode onHangUp();

private:
    Functor read_cb_;
    Functor write_cb_;
    Functor error_cb_;
    Functor hangup_cb_;
};

class EpollThread : public Thread
{
public:
    EpollThread();
    ~EpollThread();

    FCReturnCode start();
    FCReturnCode stop();
    void wakeup();

    FCReturnCode registerSlot(int slot_fd, EpollSlot slot);
    FCReturnCode unregisterSlot(int slot_fd);

private:
    void poll();
    FCReturnCode addEvent(int slot_fd, const EpollSlot &slot);
    FCReturnCode removeEvent(int slot_fd);
    
    int fd_;
    int wakeup_fd_;
    bool poll_should_stop_;
    uint8_t num_registered_slots_;
    std::unordered_map<int, EpollSlot> slot_map_;
};

}
}
}
