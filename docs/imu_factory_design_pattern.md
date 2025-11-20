# IMU Factory Design Pattern

## Problem

Let's see our problems with the current IMU design. As I explained, the implementation for IMU sensor consists of 3 main components:
- `ImuXXX`: sensor driver, directly access the HW via I2C/SPI device.
- `ImuBackend`: the interface for the `ImuXXX`, provides unified methods for many kinds of sensor.
- `ImuSensor`: or frontend, exposes processed/filtered/estimated data to higher layers (attitude, stabilization, controller, logging, etc.).

The problem is the `ImuXXX`: we may have many sensor types, each one has its own `ImuXXX` class. If we use a simple factory function (followed simple factory design pattern) to create the `ImuXXX` instance, this function will be very huge, difficult to maintain and non-readable because it has to contain many `if-else` or `switch-case` blocks. This design also requires to modify the factory function when we add new or remove `ImuXXX`.

## Solution

We need an efficient way to produce the `ImuXXX` instance and resolve the above problem =>  Use the idea of factory method design pattern.

Here are the key ideas:
- Whenever we declare the class of `ImuXXX`, we must call a macro (`REGISTER_IMU_BACKEND(class, type)`) to automatically create a static `ImuBackendFactory` instance to avoid naming conflict.
- The class of `ImuBackendFactory` must be template class. The type of template class is the type of `ImuXXX` and can be specified when calling `REGISTER_IMU_BACKEND` macro.
- `ImuBackendFactory` instance is responsible for creating the corresponding `ImuXXX` instance based on the specifed `type` from `REGISTER_IMU_BACKEND`.
- Due to `ImuBackendFactory` instance is a static instance, so we can't access it directly from other cpp files. We need to store the address to this instace while creating. Therefore, `ImuBackendFactory` class must inherit the `ImuBackendFactoryBase` class which contains a list of pointer pointing to `ImuBackendFactory` instance. In other cpp files, we just call the static member function `getFactoryInstance` of `ImuBackendFactoryBase` with parameter to find the pointer to your desired `ImuBackendFactory` instance.
