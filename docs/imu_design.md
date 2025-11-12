# IMU Design

ImuSensor <-> ImuBackend
                    ^
                    |
                ImuMPU9250

ImuSensor -probe-> ImuMPU9250

ImuInvensense -> SPIDevice/I2CDevice

struct ImuSample {
    Vec3 accel;
    Vec3 gyro;
    Vec3 mag;
    float temperature;
    uint64_t timestamp;       // accel+gyro
    uint64_t timestamp_mag;   // mag
    bool mag_valid;
};

- ImuMPU9250: hardware driver (talks to I2C/SPI, reads raw accel/gyro/mag, runs self-test, calibration, etc.)
- ImuBackend: backend abstraction — provides unified data access from any IMU type (MPU9250, ICM20948, BMI160, etc.)
- ImuSensor: frontend — exposes processed/filtered/estimated data to higher layers (attitude, stabilization, controller, logging, etc.)

ImuSensor:
    + init()
    + getGyro()
    + getAccel()
    + getInstance()
    - backend_

ImuBackend:
    + init()
    + reset()
    + getChipID()
    + calibrate()
    + readData(ImuSample &)
    + setSampleRate()
    + setFullScaleRange()
    + checkHealth()
    + isMagSupported()
    + getTimestampMs()
    - enableFifo(bool)
    - readFifo()
    - numberOfSampleFifo()
    - resetFifo()
    - hasNewData() // if using interrupt or FIFO
    - setLowPassFilter(bool, ...)
    - dev_ // I2CDevice/SPIDevice
    - fifo_enabled_
    - mag_supported_

ImuMPU9250:
    + override ImuBackend's functions
    + initMag()
    + setMagMode()
    + resetMag()
    + magDataReady()
    + calibrateMag()
    - selfTest()
    - readReg()
    - writeReg()

## FIFO 

Direct register reads:
- Pros:
- Cons:
  - Not suitable for synchronized multi sensor fusion because of reading the gyro and accel at slightly different time.

FIFO:
- Pros:
  - Accurate sample alignment: The gyro + accel + temp can be read at the same time.
  - Suitable for high sample rate.
- Cons:
  - Max FIFO size: only 512 bytes => can store only ~42 sample.
  - FIFO latency: FIFO has many samples if it is not read frequently => take a lot of time to pop.

FIFO stores gyro + accel + temp per sample, mag is seperate. Samples are pushed sequentially at the internal ODR (output data rate). 

FIFO layout:

```
| Accel_X_H | Accel_X_L | Accel_Y_H | Accel_Y_L | Accel_Z_H | Accel_Z_L |
| Gyro_X_H  | Gyro_X_L  | Gyro_Y_H  | Gyro_Y_L  | Gyro_Z_H  | Gyro_Z_L  |
| Temp_H    | Temp_L    |
```

Configure FIFO:
- Registers:
```
| Register                      | Address     | Description                                            |
| ----------------------------- | ----------- | ------------------------------------------------------ |
| `USER_CTRL`                   | 0x6A        | Enable FIFO, reset FIFO                                |
| `FIFO_EN`                     | 0x23        | Select which sensors write to FIFO (Accel, Gyro, Temp) |
| `FIFO_COUNTH` / `FIFO_COUNTL` | 0x72 / 0x73 | Current byte count in FIFO                             |
| `FIFO_R_W`                    | 0x74        | FIFO read/write data register                          |
```
- Enable FIFO:
```
writeReg(USER_CTRL, 0x40);   // Enable FIFO
writeReg(FIFO_EN, 0x78);     // Accel + Gyro to FIFO
```

How to read FIFO:
- Check how many bytes are available
```
uint8_t buf[2];
readRegs(FIFO_COUNTH, buf, 2);
size_t count = (buf[0] << 8) | buf[1];  // FIFO byte count
size_t bytes_per_sample = 14; // accel + gyro + temp
size_t num_samples = count / bytes_per_sample;
```
- Read samples (burst read)
```
for (size_t i = 0; i < num_samples; ++i) {
    uint8_t data[14];
    readRegs(FIFO_R_W, data, 14);

    // Convert raw bytes to physical units
    ImuSample sample;
    sample.accel = convertAccel(data[0], data[1], data[2], data[3], data[4], data[5]);
    sample.gyro  = convertGyro(data[6], data[7], data[8], data[9], data[10], data[11]);
    sample.temp  = convertTemp(data[12], data[13]);
    sample.timestamp_us = timestampNow();

    // store / process sample
    processSample(sample);
```

- Must read the FIFO frequently, can enable DRDY interrupt to read the sample when available in FIFO

## Mag (AK8963)

One thing need to be known: Frequency mismatch between gyro + accel and mag.
- Gyro + Accel: up to 1 kHz (or even 8 kHz internal sampling, 1 kHz output).
- Mag (AK8963): maximum 100 Hz in continuous mode.

Registers:
```
| Device  | Register              | Function                       |
| ------- | --------------------- | ------------------------------ |
| MPU9250 | `USER_CTRL` (0x6A)    | Enable I²C master              |
| MPU9250 | `I2C_MST_CTRL` (0x24) | I²C clock setting              |
| MPU9250 | `I2C_SLV0_*`          | Setup AK8963 read transactions |
| AK8963  | `CNTL1` (0x0A)        | Mode control                   |
| AK8963  | `CNTL2` (0x0B)        | Reset                          |
| AK8963  | `ASAX`–`ASAZ`         | Factory sensitivity adjustment |
| AK8963  | `ST1`                 | Data ready                     |
| AK8963  | `HXL`–`HZH`           | Raw mag data          |
```