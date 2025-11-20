#pragma once

#include <unordered_map>
#include <memory>

#include "ImuDef.h"

namespace FC
{

class ImuBackendFactoryBase
{
public:
    ImuBackendFactoryBase(ImuDef::Type type);

    static ImuBackendFactoryBase *getFactoryInstance(ImuDef::Type type);

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

    std::unique_ptr<ImuBackendClass> createInstance()
    {
        return std::make_unique<ImuBackendClass>();
    }
};

#define REGISTER_IMU_BACKEND(class, type) \
    static ImuBackendFactory<class> private_##class##_factory(type);

}
