#pragma once

#include <unordered_map>
#include <memory>

#include "ImuDef.h"

namespace FC
{

class ImuBackend;

class ImuBackendFactoryBase
{
public:
    ImuBackendFactoryBase(ImuDef::Type type);
    virtual ~ImuBackendFactoryBase() = default;

    static ImuBackendFactoryBase *getFactoryInstance(ImuDef::Type type);

    virtual std::unique_ptr<ImuBackend> createInstance(ImuDef::PhyInterface phy_interface,
                                                       ImuDef::BusNumber bus_number) = 0;

private:
    void registerNewType(ImuDef::Type type);

    static std::unordered_map<ImuDef::Type, ImuBackendFactoryBase*> registry_;
};

template<class ImuBackendClass>
class ImuBackendFactory : public ImuBackendFactoryBase
{
public:
    ImuBackendFactory(ImuDef::Type type)
        : ImuBackendFactoryBase(type)
    {
    }
    ~ImuBackendFactory() = default;

    std::unique_ptr<ImuBackend> createInstance(ImuDef::PhyInterface phy_interface,
                                               ImuDef::BusNumber bus_number)
    {
        return std::make_unique<ImuBackendClass>(phy_interface, bus_number);
    }
};

#define REGISTER_IMU_BACKEND(class, type) \
    static ImuBackendFactory<class> private_##class##_factory(type);

}
