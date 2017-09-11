#include "PtusaLib.hpp"
#include "Arp/System/Core/TypeName.hxx"
#include "MainCmp.hpp"

namespace PtusaLib
{

PtusaLib::PtusaLib(AppDomain& appDomain)
    : LibraryBase(appDomain)
{
    this->componentFactory.AddFactoryMethod("MainCmp", &MainCmp::Create);
}

void PtusaLib::Main(AppDomain& appDomain)
{
    TSingletonBase::CreateInstance(appDomain);
}

ILibrary* PtusaLib::GetInstance()
{
    return &TSingletonBase::GetInstance();
}

extern "C" ARP_CXX_SYMBOL_EXPORT void DynamicLibrary_Main(AppDomain& appDomain)
{
    PtusaLib::Main(appDomain);
}

extern "C" ARP_CXX_SYMBOL_EXPORT ILibrary* DynamicLibrary_GetInstance()
{
    return PtusaLib::GetInstance();
}

} // end of namespace PtusaLib
