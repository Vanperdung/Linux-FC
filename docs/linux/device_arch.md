# Device Arch

## I2C

In Linux, using the I2C device file /dev/i2c-XXX to read or write data from or to hardware is very popular because Linux exposes many IOCTL system calls that make such interactions convenient from userspace. As developers, we can focus on solving our problem without needing to deal with low-level driver complexities.

However, nothing is perfect. The I2C device file interface also comes with limitations. The biggest issue in my opinion is *concurrency*.

The I2C device file allows multiple processes/threads to access it at the same time, which can lead to conflicts on the bus. Since the I2C bus is fundamentally a shared resource, concurrent access without coordination may cause mixed or interleaved transactions, corrupt data transfers, or unexpected communication patterns... The kernel does not enforce exclusive access at the device-file level; therefore, user applications must implement their own locking mechanism if they want the application to run correctly.

The I2C implementation for Linux-based devices consists of 3 key components:
- I2CBus
  - represents an I2C bus (each physical I2C interface corresponds to a device file such as /dev/i2c-X).
  - exposes the functionalities needed by I2CDevice to perform read/write operations on the bus.
  - allow registering the periodic callbacks when devices require scheduled polling.
  - is internal to the FC system and is only visible to I2CDevice and I2CManager.
- I2CDevice
  - represents the I2C interface of a specific hardware module (sensor, actuator, etc.).
  - each I2CDevice is linked to an I2CBus instance. 
- I2CManager
  - acts as the centralized controller that manages the lifecycle of all I2CBus instances.
  - is responsible for creating and providing I2CDevice objects, ensuring that multiple devices correctly share the same bus and preventing unnecessary duplication of bus resources.