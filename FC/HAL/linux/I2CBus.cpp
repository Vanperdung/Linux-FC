#include "I2CBus.h"

#include <string>
#include <string.h>

#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>

#define LOG_LEVEL LOG_INFO_LEVEL
#include "utils/logger.h"
#include "utils/guard.h"

using namespace FC;
using namespace FC::HAL;
using namespace FC::HAL::Linux;

I2CBus::I2CBus(int busNumber)
    : fd_(-1),
      busNumber_(busNumber)
{
}

I2CBus::~I2CBus()
{
    close();
}

FCReturnCode I2CBus::open()
{
    CHECK_PRINT_RET(busNumber_ < 0, FAILED, "Invalid I2C bus number: %d", busNumber_);

    std::string deviceFile = "/dev/i2c-" + std::to_string(busNumber_);
    fd_ = ::open(deviceFile.c_str(), O_RDWR);
    CHECK_PRINT_RET(fd_ < 0, FAILED, "Failed to open %s: %s", deviceFile.c_str(),
                    strerror(errno));

    LOG_DEBUG("Opened I2C bus %d (%s)", busNumber_, deviceFile.c_str());

    return SUCCESS;
}

FCReturnCode I2CBus::close()
{
    if (fd_ >= 0)
        CHECK_PRINT_RET(::close(fd_) < 0, FAILED,
                        "Failed to close I2C bus %d: %s", busNumber_, strerror(errno));

    return SUCCESS;
}

FCReturnCode I2CBus::transfer(uint8_t targetAddress,
                              const uint8_t *txBuffer, uint32_t txSize,
                              uint8_t *rxBuffer, uint32_t rxSize)
{
    CHECK_PRINT_RET(fd_ < 0, FAILED, "I2C bus %d is not opened", busNumber_);
    CHECK_PRINT_RET(targetAddress >= 128, FAILED,
                    "Invalid I2C target address: 0x%02X", targetAddress);

    struct i2c_msg messages[2];
    int count = 0;

    if (txBuffer != nullptr && txSize > 0)
    {
        messages[count].addr = targetAddress;
        messages[count].flags = 0; // Write
        messages[count].len = static_cast<__u16>(txSize);
        messages[count].buf = const_cast<__u8 *>(txBuffer);
        count++;
    }

    if (rxBuffer != nullptr && rxSize > 0)
    {
        messages[count].addr = targetAddress;
        messages[count].flags = I2C_M_RD; // Read
        messages[count].len = static_cast<__u16>(rxSize);
        messages[count].buf = const_cast<__u8 *>(rxBuffer);
        count++;
    }

    CHECK_PRINT_RET(count == 0, FAILED,
                    "No data to transfer for I2C address 0x%02X", targetAddress);

    struct i2c_rdwr_ioctl_data rdwrData;

    rdwrData.msgs = messages;
    rdwrData.nmsgs = count;

    CHECK_PRINT_RET(::ioctl(fd_, I2C_RDWR, &rdwrData) < 0, FAILED,
                    "I2C transfer failed: %s", strerror(errno));

    return SUCCESS;
}