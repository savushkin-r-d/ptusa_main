#include "PtusaLibrary.hpp"
#include "Arp/System/Core/CommonTypeName.hxx"
#include "Arp/Plc/Commons/Meta/TypeSystem/TypeSystem.h"
#include "PtusaComponent.hpp"

namespace Ptusa
{

PtusaLibrary::PtusaLibrary(AppDomain& appDomain)
    : MetaLibraryBase(appDomain, ARP_VERSION_CURRENT, typeDomain)
    , typeDomain(CommonTypeName<PtusaLibrary>().GetNamespace())
{
    this->componentFactory.AddFactoryMethod(CommonTypeName<::Ptusa::PtusaComponent>(), &::Ptusa::PtusaComponent::Create);
    this->InitializeTypeDomain();
}

void PtusaLibrary::Main(AppDomain& appDomain)
{
    SingletonBase::CreateInstance(appDomain);
}

extern "C" ARP_CXX_SYMBOL_EXPORT ILibrary& ArpDynamicLibraryMain(AppDomain& appDomain)
{
    PtusaLibrary::Main(appDomain);
    return  PtusaLibrary::GetInstance();
}

} // end of namespace Ptusa
