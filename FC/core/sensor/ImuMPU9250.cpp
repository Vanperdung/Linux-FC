#include "ImuMPU9250.h"

#include "ImuBackendFactory.h"
#include "ImuMPUXXXXRegs.h"
#include "HAL/base/HALInterface.h"
#include "HAL/base/I2CManagerInterface.h"
#include "HAL/base/UtilsInterface.h"

#include "utils/Logger.h"
#include "utils/Guard.h"

using namespace FC;

ImuMPU9250::ImuMPU9250(ImuDef::PhyInterface phy_interface,
                       ImuDef::BusNumber bus_number)
    : dev_(nullptr),
      phy_interface_(phy_interface),
      bus_number_(bus_number),
      current_accel_fsr_(0.0f),
      current_gyro_fsr_(0.0f)
{
}

ImuMPU9250::~ImuMPU9250()
{
}

FCReturnCode ImuMPU9250::init()
{
    // Create interface device
    CHECK_RET(createDevice() != SUCCESS, FAILED);

    // Initialize the hardware sensor
    CHECK_RET(initHardware() != SUCCESS, FAILED);

    return SUCCESS;
}

void ImuMPU9250::cleanup()
{
}

FCReturnCode ImuMPU9250::createDevice()
{
    HAL::Base::HALInterface &hal = HAL::Base::HALInterface::getInstance();
    unsigned int bus_number = static_cast<unsigned int>(bus_number_);

    // Ensure that we're creating supported physical interface
    if (phy_interface_ == ImuDef::PhyInterface::I2C)
    {
        dev_ = hal.getI2CManagerInstance().createDevice(bus_number);
    }
    else if (phy_interface_ == ImuDef::PhyInterface::SPI)
    {
        // TODO
        LOG_WARNING("Not support SPI interface yet");
    }
    else
    {
        LOG_ERROR("Unsupported physical interface");
        return FAILED;
    }

    CHECK_PRINT_RET(dev_ == nullptr, FAILED,
                    "Failed to create interface device on bus %d", bus_number);

    LOG_INFO("Created interface device successfully on bus %d", bus_number);

    return SUCCESS;
}

FCReturnCode ImuMPU9250::initHardware()
{
    CHECK_RET(dev_ == nullptr, FAILED);

    // Set I2C address to 0x68 (default for MPU9250 with AD0 low)
    CHECK_RET(setAddress(MPUXXXX_ADDRESS_AD0_LOW) != SUCCESS, FAILED);

    // Reset the hardware
    CHECK_RET(reset() != SUCCESS, FAILED);

    // Set clock source to PLL with X axis gyroscope reference
    CHECK_RET(setClockSource(MPUXXXX_CLOCK_PLL_X) != SUCCESS, FAILED);

    uint8_t id = whoAmI();
    if (id != MPUXXXX_WHO_AM_I_MPU9250)
    {
        LOG_ERROR("Unexpected WHO_AM_I value: 0x%02X", id);
        return FAILED;
    }

    LOG_INFO("Detected MPU9250 with WHO_AM_I = 0x%02X", id);

    // Set gyro full scale range to ±250 °/s
    CHECK_RET(setGyroFullScaleRange(MPUXXXX_GYRO_RANGE_250) != SUCCESS, FAILED);
    current_gyro_fsr_ = 250.0f;

    // Configure gyro sample rate to 1 kHz
    CHECK_RET(configureGyroSampleRate() != SUCCESS, FAILED);

    // Set accelerometer full scale range to ±2 g
    CHECK_RET(setAccelFullScaleRange(MPUXXXX_ACCEL_RANGE_2G) != SUCCESS, FAILED);
    current_accel_fsr_ = 2.0f;

    // Configure accelerometer sample rate to 1 kHz
    CHECK_RET(configureAccelSampleRate() != SUCCESS, FAILED);

    // Configure dividers for 1 kHz sample rate
    CHECK_RET(setSampleRate(DEFAULT_SAMPLE_RATE_HZ) != SUCCESS, FAILED);

    // Disable sleep mode
    CHECK_RET(enableSleepMode(false) != SUCCESS, FAILED);

    // Enable magnetometer
    // TODO: Implement magnetometer enabling

    return SUCCESS;
}

FCReturnCode ImuMPU9250::reset()
{
    CHECK_RET(dev_ == nullptr, FAILED);

    HAL::Base::HALInterface &hal = HAL::Base::HALInterface::getInstance();
    uint8_t tx_buf[2] = {MPUXXXX_REGISTER_PWR_MGMT_1, 0};
    uint8_t rx_buf = 0;

    dev_->lock();

    // Set DEVICE_RESET bit in PWR_MGMT_1 register to reset the device
    CHECK_PRINT_GOTO(dev_->transfer(&tx_buf[0], 1, &rx_buf, 1) != SUCCESS, error,
                     "Failed to read PWR_MGMT_1 register");

    tx_buf[1] = rx_buf;
    tx_buf[1] |= (1 << MPUXXXX_PWR1_DEVICE_RESET_BIT);
    CHECK_PRINT_GOTO(dev_->transfer(tx_buf, 2, nullptr, 0) != SUCCESS, error,
                     "Failed to write PWR_MGMT_1 register");

    dev_->unlock();

    // Wait for device to reset
    hal.getUtilsInstance().delayMs(100);

    dev_->lock();

    // Reset signal paths
    tx_buf[0] = MPUXXXX_REGISTER_SIGNAL_PATH_RESET;
    rx_buf = 0;
    CHECK_PRINT_GOTO(dev_->transfer(&tx_buf[0], 1, &rx_buf, 1) != SUCCESS, error,
                     "Failed to read SIGNAL_PATH_RESET register");

    tx_buf[1] = rx_buf;
    tx_buf[1] |= (1 << MPUXXXX_PATHRESET_GYRO_RESET_BIT) |
                 (1 << MPUXXXX_PATHRESET_ACCEL_RESET_BIT) |
                 (1 << MPUXXXX_PATHRESET_TEMP_RESET_BIT);
    CHECK_PRINT_GOTO(dev_->transfer(tx_buf, 2, nullptr, 0) != SUCCESS, error,
                     "Failed to reset signal paths");

    dev_->unlock();

    // Wait for signal paths to reset
    hal.getUtilsInstance().delayMs(100);
    return SUCCESS;

error:
    dev_->unlock();
    return FAILED;
}

uint8_t ImuMPU9250::whoAmI()
{
    CHECK_RET(dev_ == nullptr, 0);

    uint8_t id = 0;
    uint8_t tx_buf = MPUXXXX_REGISTER_WHO_AM_I;

    dev_->lock();

    CHECK_PRINT_GOTO(dev_->transfer(&tx_buf, 1, &id, 1) != SUCCESS, error,
                     "Failed to read WHO_AM_I register");

    dev_->unlock();
    return id;

error:
    dev_->unlock();
    return id;
}

FCReturnCode ImuMPU9250::setAddress(uint8_t address)
{
    CHECK_RET(dev_ == nullptr, FAILED);

    return dev_->setAddress(address);
}

FCReturnCode ImuMPU9250::getSample(ImuSample<float> &sample)
{
    CHECK_RET(dev_ == nullptr, FAILED);

    uint8_t tx_buf = MPUXXXX_REGISTER_ACCEL_XOUT_H;
    uint8_t rx_buf[6] = {0};
    int16_t temp_raw = 0;

    // Scales to convert raw data to physical units
    const float accel_scale = current_accel_fsr_ / 32768.0f;
    const float gyro_scale = current_gyro_fsr_ / 32768.0f;

    ImuSample<int16_t> raw_sample;

    dev_->lock();

    CHECK_PRINT_GOTO(dev_->transfer(&tx_buf, 1, rx_buf, 6) != SUCCESS, error,
                     "Failed to read accelerometer data");

    // Parse accelerometer raw data
    raw_sample.accel.x = (static_cast<int16_t>(rx_buf[0]) << 8) | rx_buf[1];
    raw_sample.accel.y = (static_cast<int16_t>(rx_buf[2]) << 8) | rx_buf[3];
    raw_sample.accel.z = (static_cast<int16_t>(rx_buf[4]) << 8) | rx_buf[5];

    // Calculate accelerometer raw data to g units
    sample.accel.x = raw_sample.accel.x * accel_scale;
    sample.accel.y = raw_sample.accel.y * accel_scale;
    sample.accel.z = raw_sample.accel.z * accel_scale;

    // Read gyroscope data
    tx_buf = MPUXXXX_REGISTER_GYRO_XOUT_H;
    CHECK_PRINT_GOTO(dev_->transfer(&tx_buf, 1, rx_buf, 6) != SUCCESS, error,
                     "Failed to read gyroscope data");

    // Parse gyroscope data
    raw_sample.gyro.x = (static_cast<int16_t>(rx_buf[0]) << 8) | rx_buf[1];
    raw_sample.gyro.y = (static_cast<int16_t>(rx_buf[2]) << 8) | rx_buf[3];
    raw_sample.gyro.z = (static_cast<int16_t>(rx_buf[4]) << 8) | rx_buf[5];

    // Calculate gyroscope raw data to °/s units
    sample.gyro.x = raw_sample.gyro.x * gyro_scale;
    sample.gyro.y = raw_sample.gyro.y * gyro_scale;
    sample.gyro.z = raw_sample.gyro.z * gyro_scale;

    // Read magnetometer data
    // TODO: Implement proper magnetometer reading via I2C master
    // Parse magnetometer data
    // TODO: Implement proper magnetometer reading via I2C master

    // Read temperature data
    tx_buf = MPUXXXX_REGISTER_TEMP_OUT_H;
    CHECK_PRINT_GOTO(dev_->transfer(&tx_buf, 1, rx_buf, 2) != SUCCESS, error,
                     "Failed to read temperature data");

    // Parse temperature data
    temp_raw = (static_cast<int16_t>(rx_buf[0]) << 8) | rx_buf[1];
    sample.temperature = temp_raw * 1.0f / 340.0f + 36.53f;

    dev_->unlock();
    return SUCCESS;

error:
    dev_->unlock();
    return FAILED;
}

FCReturnCode ImuMPU9250::resetFIFO()
{
    CHECK_RET(dev_ == nullptr, FAILED);

    uint8_t tx_user_ctrl[2] = {MPUXXXX_REGISTER_USER_CTRL, 0};
    uint8_t tx_fifo_en[2] = {MPUXXXX_REGISTER_FIFO_EN, 0};
    uint8_t rx_buf = 0;

    dev_->lock();

    // Read current USER_CTRL register value
    tx_user_ctrl[0] = MPUXXXX_REGISTER_USER_CTRL;
    CHECK_PRINT_GOTO(dev_->transfer(&tx_user_ctrl[0], 1, &rx_buf, 1) != SUCCESS, error,
                     "Failed to read USER_CTRL register");

    // Disable FIFO
    CHECK_PRINT_GOTO(dev_->transfer(tx_fifo_en, 2, nullptr, 0) != SUCCESS, error,
                     "Failed to read FIFO_EN register");

    // Clear FIFO_RESET and FIFO_EN bits to disable FIFO operation
    tx_user_ctrl[1] = rx_buf;
    tx_user_ctrl[1] &= ~((1 << MPUXXXX_USERCTRL_FIFO_RESET_BIT) | (1 << MPUXXXX_USERCTRL_FIFO_EN_BIT));
    CHECK_PRINT_GOTO(dev_->transfer(tx_user_ctrl, 2, nullptr, 0) != SUCCESS, error,
                     "Failed to disable FIFO");

    // Set FIFO_RESET bit to reset FIFO module
    tx_user_ctrl[1] |= (1 << MPUXXXX_USERCTRL_FIFO_RESET_BIT);
    CHECK_PRINT_GOTO(dev_->transfer(tx_user_ctrl, 2, nullptr, 0) != SUCCESS, error,
                     "Failed to reset FIFO");

    // Set FIFO_EN bit to re-enable FIFO operation mode
    tx_user_ctrl[1] |= (1 << MPUXXXX_USERCTRL_FIFO_EN_BIT);
    CHECK_PRINT_GOTO(dev_->transfer(tx_user_ctrl, 2, nullptr, 0) != SUCCESS, error,
                     "Failed to enable FIFO");

    // Re-enable FIFO for accel, gyro, and temperature
    tx_fifo_en[1] = (1 << MPUXXXX_XG_FIFO_EN_BIT) |
                    (1 << MPUXXXX_YG_FIFO_EN_BIT) |
                    (1 << MPUXXXX_ZG_FIFO_EN_BIT) |
                    (1 << MPUXXXX_ACCEL_FIFO_EN_BIT) |
                    (1 << MPUXXXX_TEMP_FIFO_EN_BIT);
    CHECK_PRINT_GOTO(dev_->transfer(tx_fifo_en, 2, nullptr, 0) != SUCCESS, error,
                     "Failed to enable FIFO for gyro, accel, and temperature");

    dev_->unlock();
    return SUCCESS;

error:
    dev_->unlock();
    return FAILED;
}

FCReturnCode ImuMPU9250::setSampleRate(uint16_t rate_hz)
{
    CHECK_RET(dev_ == nullptr, FAILED);

    if (rate_hz == 0 || rate_hz > DEFAULT_SAMPLE_RATE_HZ)
    {
        LOG_ERROR("Invalid sample rate: %d Hz", rate_hz);
        return FAILED;
    }

    uint8_t tx_buf[2] = {MPUXXXX_REGISTER_SMPLRT_DIV, 0};
    uint8_t rx_buf = 0;

    dev_->lock();
    // Read current SMPLRT_DIV register value
    CHECK_PRINT_GOTO(dev_->transfer(&tx_buf[0], 1, &rx_buf, 1) != SUCCESS, error,
                     "Failed to read SMPLRT_DIV register");

    // Set sample rate divider
    tx_buf[1] = (DEFAULT_SAMPLE_RATE_HZ / rate_hz) - 1;
    CHECK_PRINT_GOTO(dev_->transfer(tx_buf, 2, nullptr, 0) != SUCCESS, error,
                     "Failed to write SMPLRT_DIV register");

    dev_->unlock();
    return SUCCESS;

error:
    dev_->unlock();
    return FAILED;
}

FCReturnCode ImuMPU9250::enableInterrupt(bool enable)
{
    CHECK_RET(dev_ == nullptr, FAILED);

    uint8_t tx_buf[2] = {MPUXXXX_REGISTER_INT_ENABLE, 0};
    uint8_t rx_buf = 0;

    dev_->lock();

    // Read current INT_ENABLE register value
    CHECK_PRINT_GOTO(dev_->transfer(&tx_buf[0], 1, &rx_buf, 1) != SUCCESS, error,
                     "Failed to read INT_ENABLE register");

    if (enable)
        tx_buf[1] = rx_buf | (1 << MPUXXXX_INT_DATA_RDY_BIT);
    else
        tx_buf[1] = rx_buf & ~(1 << MPUXXXX_INT_DATA_RDY_BIT);

    // Enable interrupt to fire on data ready
    CHECK_PRINT_GOTO(dev_->transfer(tx_buf, 2, nullptr, 0) != SUCCESS, error,
                     "Failed to enable interrupt on data ready");

    dev_->unlock();
    return SUCCESS;

error:
    dev_->unlock();
    return FAILED;
}

FCReturnCode ImuMPU9250::registerPeriodicCallback(Functor callback,
                                                  void *context,
                                                  uint32_t interval_ns)
{
    CHECK_RET(dev_ == nullptr, FAILED);

    return dev_->registerPeriodicCallback(callback, context, interval_ns);
}

FCReturnCode ImuMPU9250::setClockSource(uint8_t clk_source)
{
    CHECK_RET(dev_ == nullptr, FAILED);

    uint8_t tx_buf[2] = {MPUXXXX_REGISTER_PWR_MGMT_1, 0};
    uint8_t rx_buf = 0;

    dev_->lock();

    // Read current PWR_MGMT_1 register value
    CHECK_PRINT_GOTO(dev_->transfer(&tx_buf[0], 1, &rx_buf, 1) != SUCCESS, error,
                     "Failed to read PWR_MGMT_1 register");

    // Modify clock source bits
    tx_buf[1] = rx_buf & ~MPUXXXX_PWR1_CLKSEL_MASK;
    tx_buf[1] |= (clk_source << MPUXXXX_PWR1_CLKSEL_BIT) & MPUXXXX_PWR1_CLKSEL_MASK;

    // Write back modified PWR_MGMT_1 register value
    CHECK_PRINT_GOTO(dev_->transfer(tx_buf, 2, nullptr, 0) != SUCCESS, error,
                     "Failed to write PWR_MGMT_1 register");

    dev_->unlock();
    return SUCCESS;

error:
    dev_->unlock();
    return FAILED;
}

FCReturnCode ImuMPU9250::setGyroFullScaleRange(uint8_t range)
{
    CHECK_RET(dev_ == nullptr, FAILED);

    uint8_t tx_buf[2] = {MPUXXXX_REGISTER_GYRO_CONFIG, 0};
    uint8_t rx_buf = 0;

    dev_->lock();

    // Read current GYRO_CONFIG register value
    CHECK_PRINT_GOTO(dev_->transfer(&tx_buf[0], 1, &rx_buf, 1) != SUCCESS, error,
                     "Failed to read GYRO_CONFIG register");

    // Modify full scale range bits
    tx_buf[1] = rx_buf & ~MPUXXXX_GCONFIG_FS_SEL_MASK;
    tx_buf[1] |= (range << MPUXXXX_GCONFIG_FS_SEL_BIT) & MPUXXXX_GCONFIG_FS_SEL_MASK;

    // Write back modified GYRO_CONFIG register value
    CHECK_PRINT_GOTO(dev_->transfer(tx_buf, 2, nullptr, 0) != SUCCESS, error,
                     "Failed to write GYRO_CONFIG register");

    dev_->unlock();
    return SUCCESS;

error:
    dev_->unlock();
    return FAILED;
}

FCReturnCode ImuMPU9250::setAccelFullScaleRange(uint8_t range)
{
    CHECK_RET(dev_ == nullptr, FAILED);

    uint8_t tx_buf[2] = {MPUXXXX_REGISTER_ACCEL_CONFIG, 0};
    uint8_t rx_buf = 0;

    dev_->lock();

    // Read current ACCEL_CONFIG register value
    CHECK_PRINT_GOTO(dev_->transfer(&tx_buf[0], 1, &rx_buf, 1) != SUCCESS, error,
                     "Failed to read ACCEL_CONFIG register");

    // Modify full scale range bits
    tx_buf[1] = rx_buf & ~MPUXXXX_ACONFIG_AFS_SEL_MASK;
    tx_buf[1] |= (range << MPUXXXX_ACONFIG_AFS_SEL_BIT) & MPUXXXX_ACONFIG_AFS_SEL_MASK;

    // Write back modified ACCEL_CONFIG register value
    CHECK_PRINT_GOTO(dev_->transfer(tx_buf, 2, nullptr, 0) != SUCCESS, error,
                     "Failed to write ACCEL_CONFIG register");

    dev_->unlock();
    return SUCCESS;

error:
    dev_->unlock();
    return FAILED;
}

FCReturnCode ImuMPU9250::enableSleepMode(bool enable)
{
    CHECK_RET(dev_ == nullptr, FAILED);

    uint8_t tx_buf[2] = {MPUXXXX_REGISTER_PWR_MGMT_1, 0};
    uint8_t rx_buf = 0;

    dev_->lock();

    // Read current PWR_MGMT_1 register value
    CHECK_PRINT_GOTO(dev_->transfer(&tx_buf[0], 1, &rx_buf, 1) != SUCCESS, error,
                     "Failed to read PWR_MGMT_1 register");

    // Modify SLEEP bit
    if (enable)
        tx_buf[1] = rx_buf | (1 << MPUXXXX_PWR1_SLEEP_BIT);
    else
        tx_buf[1] = rx_buf & ~(1 << MPUXXXX_PWR1_SLEEP_BIT);

    // Write back modified PWR_MGMT_1 register value
    CHECK_PRINT_GOTO(dev_->transfer(tx_buf, 2, nullptr, 0) != SUCCESS, error,
                     "Failed to write PWR_MGMT_1 register");

    dev_->unlock();
    return SUCCESS;

error:
    dev_->unlock();
    return FAILED;
}

FCReturnCode ImuMPU9250::enableMagnetometer(bool enable)
{
    // TODO: Implement magnetometer enabling/disabling if needed
    return SUCCESS;
}

FCReturnCode ImuMPU9250::configureGyroSampleRate()
{
    CHECK_RET(dev_ == nullptr, FAILED);

    uint8_t tx_buf[2] = {};
    uint8_t rx_buf = 0;

    /**
     * To configure the gyroscope sample rate to 1 kHz, we need to set the
     * DLPF (Digital Low Pass Filter) and FCHOICE_B bits appropriately.
     * 1. Set FCHOICE_B to 0 in GYRO_CONFIG register to enable DLPF.
     * 2. Set DLPF_CFG to 1 in CONFIG register for 1 kHz sample rate and 2.9 ms delay.
     */
    dev_->lock();

    tx_buf[0] = MPUXXXX_REGISTER_GYRO_CONFIG;
    CHECK_PRINT_GOTO(dev_->transfer(&tx_buf[0], 1, &rx_buf, 1) != SUCCESS, error,
                     "Failed to read GYRO_CONFIG register");

    // Set FCHOICE_B to 0 to enable DLPF
    tx_buf[1] = (rx_buf & ~MPUXXXX_GCONFIG_FCHOICE_B_MASK);
    CHECK_PRINT_GOTO(dev_->transfer(tx_buf, 2, nullptr, 0) != SUCCESS, error,
                     "Failed to write GYRO_CONFIG register");

    tx_buf[0] = MPUXXXX_REGISTER_CONFIG;
    rx_buf = 0;
    CHECK_PRINT_GOTO(dev_->transfer(&tx_buf[0], 1, &rx_buf, 1) != SUCCESS, error,
                     "Failed to read CONFIG register");

    // Set DLPF_CFG to 1 for Gyro Fs = 1 kHz
    tx_buf[1] = (rx_buf & ~MPUXXXX_CFG_DLPF_CFG_MASK) | (1 << MPUXXXX_CFG_DLPF_CFG_BIT);
    CHECK_PRINT_GOTO(dev_->transfer(tx_buf, 2, nullptr, 0) != SUCCESS, error,
                     "Failed to write CONFIG register");

    dev_->unlock();
    return SUCCESS;

error:
    dev_->unlock();
    return FAILED;
}

FCReturnCode ImuMPU9250::configureAccelSampleRate()
{
    CHECK_RET(dev_ == nullptr, FAILED);

    uint8_t tx_buf[2] = {MPUXXXX_REGISTER_ACCEL_CONFIG, 0};
    uint8_t rx_buf = 0;

    /**
     * To configure the accelerometer sample rate to 1 kHz, we need to set the
     * A_DLPF (Accelerometer Digital Low Pass Filter) and ACCEL_FCHOICE_B
     * bits appropriately.
     * 1. Set ACCEL_FCHOICE_B to 0 in ACCEL_CONFIG register to enable DLPF.
     * 2. Set A_DLPF to 2 in CONFIG register for 1 kHz sample rate and 2.88 ms delay.
     */

    dev_->lock();

    // Read current ACCEL_CONFIG register value
    CHECK_PRINT_GOTO(dev_->transfer(&tx_buf[0], 1, &rx_buf, 1) != SUCCESS, error,
                     "Failed to read ACCEL_CONFIG register");

    // Set ACCEL_FCHOICE_B bit to 0 to enable A_DLPF
    tx_buf[1] = (rx_buf & ~MPUXXXX_ACONFIG_ACCEL_FCHOICE_B_MASK);

    // Set A_DLPF to 2 for 1 kHz sample rate and 2.88 ms delay
    tx_buf[1] = (rx_buf & ~MPUXXXX_ACONFIG_A_DLPF_CFG_MASK) | (2 << MPUXXXX_ACONFIG_A_DLPF_CFG_BIT);
    CHECK_PRINT_GOTO(dev_->transfer(tx_buf, 2, nullptr, 0) != SUCCESS, error,
                     "Failed to write CONFIG register");

    dev_->unlock();
    return SUCCESS;

error:
    dev_->unlock();
    return FAILED;
}

FCReturnCode ImuMPU9250::calibrate()
{
    return SUCCESS;
}


REGISTER_IMU_BACKEND(ImuMPU9250, ImuDef::Type::MPU9250)
