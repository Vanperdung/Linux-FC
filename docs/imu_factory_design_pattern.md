# IMU Factory Design Pattern

## Problem

Let's see our problems with the current IMU design. As I explained, the implementation for IMU sensor consists of 3 main components:
- `ImuXXX`: sensor driver, directly access the HW via I2C/SPI device.
- `ImuBackend`: the interface for the `ImuXXX`, provides unified methods for many kinds of sensor.
- `ImuSensor`: or frontend, exposes processed/filtered/estimated data to higher layers (attitude, stabilization, controller, logging, etc.).

The problem is the `ImuXXX`: we may have many sensor types, each one has its own `ImuXXX` class. If we use a simple factory function (following the simple factory design pattern) to create the `ImuXXX` instance, this function will be very huge, difficult to maintain and unreadable because it has to contain many `if-else` or `switch-case` blocks. This design also requires modifying the factory function when we add or remove an `ImuXXX`.

## Solution

We need an efficient way to produce the `ImuXXX` instance and resolve the above problem =>  Use the idea of factory method design pattern.

Here are the key ideas:
- Whenever we declare a class for an `ImuXXX`, we must call a macro (`REGISTER_IMU_BACKEND(class, type)`) to automatically create a corresponding static `ImuBackendFactory` instance.
- The `ImuBackendFactory` class is a template class. The template argument is the type of the `ImuXXX` class, specified when calling the `REGISTER_IMU_BACKEND` macro.
- Each `ImuBackendFactory` instance is responsible for creating the corresponding `ImuXXX` instance based on the `type` specified in `REGISTER_IMU_BACKEND`.
- Since each `ImuBackendFactory` is a static instance (with internal linkage), it cannot be accessed directly from other translation units. To solve this, we store a pointer to each factory instance upon its creation. `ImuBackendFactory` inherits from a common `ImuBackendFactoryBase` class, which maintains a list of pointers to all created factory instances. To get a specific factory, other parts of the code can call the static member function `getFactoryInstance` on `ImuBackendFactoryBase`, passing the desired type.
