#include "PtusaPLCnextEngineerLibrary.hpp"
#include "Arp/System/Core/CommonTypeName.hxx"
#include "Arp/Plc/Commons/Meta/TypeSystem/TypeSystem.h"
#include "PtusaMainCmpnt.hpp"

namespace PtusaPLCnextEngineer
{

PtusaPLCnextEngineerLibrary::PtusaPLCnextEngineerLibrary(AppDomain& appDomain)
    : MetaLibraryBase(appDomain, ARP_VERSION_CURRENT, typeDomain)
    , typeDomain(CommonTypeName<PtusaPLCnextEngineerLibrary>().GetNamespace())
{
    this->componentFactory.AddFactoryMethod(CommonTypeName<::PtusaPLCnextEngineer::PtusaMainCmpnt>(), &::PtusaPLCnextEngineer::PtusaMainCmpnt::Create);
    this->InitializeTypeDomain();
}

void PtusaPLCnextEngineerLibrary::Main(AppDomain& appDomain)
{
    SingletonBase::CreateInstance(appDomain);
}

extern "C" ARP_CXX_SYMBOL_EXPORT ILibrary& ArpDynamicLibraryMain(AppDomain& appDomain)
{
    PtusaPLCnextEngineerLibrary::Main(appDomain);
    return  PtusaPLCnextEngineerLibrary::GetInstance();
}

} // end of namespace PtusaPLCnextEngineer
