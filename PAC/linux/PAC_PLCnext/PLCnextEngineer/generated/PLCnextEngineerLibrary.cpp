#include "PLCnextEngineerLibrary.hpp"
#include "Arp/System/Core/CommonTypeName.hxx"
#include "Arp/Plc/Commons/Meta/TypeSystem/TypeSystem.h"
#include "PtusaComponent.hpp"

namespace Ptusa
{

PLCnextEngineerLibrary::PLCnextEngineerLibrary(AppDomain& appDomain)
    : MetaLibraryBase(appDomain, ARP_VERSION_CURRENT, typeDomain)
    , typeDomain(CommonTypeName<PLCnextEngineerLibrary>().GetNamespace())
{
    this->componentFactory.AddFactoryMethod(CommonTypeName<::Ptusa::PtusaComponent>(), &::Ptusa::PtusaComponent::Create);
    this->InitializeTypeDomain();
}

void PLCnextEngineerLibrary::Main(AppDomain& appDomain)
{
    SingletonBase::CreateInstance(appDomain);
}

extern "C" ARP_CXX_SYMBOL_EXPORT ILibrary& ArpDynamicLibraryMain(AppDomain& appDomain)
{
    PLCnextEngineerLibrary::Main(appDomain);
    return  PLCnextEngineerLibrary::GetInstance();
}

} // end of namespace Ptusa
