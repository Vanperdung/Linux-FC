# Device Design

# I2CDevice

Device <-- I2CDevice <-- Linux::I2CDevice

I2CDeviceManager::buses -> I2C-1
                        -> I2C-2
I2CDeviceManager::get_device_ptr()  -> create I2CBus if id does not exist
                                    -> create I2CDevice

I2CBus is private class from higher layer and is Linux-specific class. It is only known by the I2CDeviceManager.

I2CBus includes a PollerThread or an EventDispatcher responsible for running epoll() to demultiplex event tasks.

Device:
    + lock()
    + unlock()
    + setSpeed()
    + transfer()
    + transferFullDuplex()
    + registerPeriodicCallback()
    + adjustPeriodicCallback()
    + setAddress()
    + getAddress()
    + setChipSelect()

I2CDevice:
    + lock()
    + unlock()
    + transfer()
    + registerPeriodicCallback()
    + adjustPeriodicCallback()
    + setAddress()
    + getAddress()