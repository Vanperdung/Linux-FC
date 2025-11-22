#include "ImuBackendFactory.h"

using namespace FC;

std::unordered_map<ImuDef::Type, ImuBackendFactoryBase*> ImuBackendFactoryBase::registry_;

ImuBackendFactoryBase::ImuBackendFactoryBase(ImuDef::Type type)
{
    registerNewType(type);
}

ImuBackendFactoryBase *ImuBackendFactoryBase::getFactoryInstance(ImuDef::Type type)
{
    auto it = registry_.find(type);

    return (it == registry_.end()) ? nullptr : it->second; 
}

void ImuBackendFactoryBase::registerNewType(ImuDef::Type type)
{
    registry_[type] = this;
}
