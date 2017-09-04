#include "ptusa.hpp"
#include "Arp/System/Core/TypeName.hxx"
#include "main.hpp"

namespace ptusa
{

ptusa::ptusa(AppDomain& appDomain)
    : LibraryBase(appDomain)
{
    this->componentFactory.AddFactoryMethod("main", &main::Create);
}

void ptusa::Main(AppDomain& appDomain)
{
    TSingletonBase::CreateInstance(appDomain);
}

ILibrary* ptusa::GetInstance()
{
    return &TSingletonBase::GetInstance();
}

extern "C" ARP_CXX_SYMBOL_EXPORT void DynamicLibrary_Main(AppDomain& appDomain)
{
    ptusa::Main(appDomain);
}

extern "C" ARP_CXX_SYMBOL_EXPORT ILibrary* DynamicLibrary_GetInstance()
{
    return ptusa::GetInstance();
}

} // end of namespace ptusa
