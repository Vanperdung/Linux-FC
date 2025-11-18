#include "I2CBus.h"

#include <string>
#include <cstring>

#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>

#define LOG_LEVEL LOG_INFO_LEVEL
#include "utils/Logger.h"
#include "utils/Guard.h"

using namespace FC;
using namespace FC::HAL;
using namespace FC::HAL::Linux;

I2CBus::I2CBus(int bus_number)
    : fd_(-1),
      bus_number_(bus_number)
{
}

I2CBus::~I2CBus()
{
    close();
}

FCReturnCode I2CBus::open()
{
    CHECK_PRINT_RET(bus_number_ < 0, FAILED, "Invalid I2C bus number: %d", bus_number_);

    std::string device_file = "/dev/i2c-" + std::to_string(bus_number_);
    fd_ = ::open(device_file.c_str(), O_RDWR);
    CHECK_PRINT_RET(fd_ < 0, FAILED, "Failed to open %s: %s", device_file.c_str(),
                    strerror(errno));

    LOG_DEBUG("Opened I2C bus %d (%s)", bus_number_, device_file.c_str());

    return poller_.start();
}

FCReturnCode I2CBus::close()
{
    CHECK_RET(poller_.stop() != SUCCESS, FAILED);

    if (fd_ >= 0)
        CHECK_PRINT_RET(::close(fd_) < 0, FAILED,
                        "Failed to close I2C bus %d: %s", bus_number_, strerror(errno));

    return SUCCESS;
}

FCReturnCode I2CBus::transfer(uint8_t target_address,
                              const uint8_t *tx_buffer, uint32_t tx_size,
                              uint8_t *rx_buffer, uint32_t rx_size)
{
    CHECK_PRINT_RET(fd_ < 0, FAILED, "I2C bus %d is not opened", bus_number_);
    CHECK_PRINT_RET(target_address >= 128, FAILED,
                    "Invalid I2C target address: 0x%02X", target_address);

    struct i2c_msg messages[2];
    int count = 0;

    if (tx_buffer != nullptr && tx_size > 0)
    {
        messages[count].addr = target_address;
        messages[count].flags = 0; // Write
        messages[count].len = static_cast<__u16>(tx_size);
        messages[count].buf = const_cast<__u8 *>(tx_buffer);
        count++;
    }

    if (rx_buffer != nullptr && rx_size > 0)
    {
        messages[count].addr = target_address;
        messages[count].flags = I2C_M_RD; // Read
        messages[count].len = static_cast<__u16>(rx_size);
        messages[count].buf = const_cast<__u8 *>(rx_buffer);
        count++;
    }

    CHECK_PRINT_RET(count == 0, FAILED,
                    "No data to transfer for I2C address 0x%02X", target_address);

    struct i2c_rdwr_ioctl_data rdwr_data;

    rdwr_data.msgs = messages;
    rdwr_data.nmsgs = count;

    CHECK_PRINT_RET(::ioctl(fd_, I2C_RDWR, &rdwr_data) < 0, FAILED,
                    "I2C transfer failed: %s", strerror(errno));

    return SUCCESS;
}

FCReturnCode I2CBus::registerPeriodicCallback(int slot_fd,
                                              Functor callback,
                                              void *context)
{
    EpollSlot slot(std::bind(callback, context));

    return poller_.registerSlot(slot_fd, slot);
}