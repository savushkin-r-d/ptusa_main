#pragma once
#include "Arp/System/Core/Arp.h"
#include "Arp/System/Core/AppDomain.hpp"
#include "Arp/System/Core/Singleton.hxx"
#include "Arp/System/Core/Library.h"
#include "Arp/System/Acf/LibraryBase.hpp"

using namespace Arp;
using namespace Arp::System::Acf;

namespace PtusaLib
{

class PtusaLib : public LibraryBase, public Singleton<PtusaLib>
{
public: // typedefs
    typedef Singleton<PtusaLib> TSingletonBase;

public: // construction/destruction
    PtusaLib(AppDomain& appDomain);
    virtual ~PtusaLib(void) = default;

public: // overridden operations

public: // static operations (called through reflection)
    static void Main(AppDomain& appDomain);
    static ILibrary* GetInstance(void);

private: // methods
    PtusaLib(const PtusaLib& arg) = delete;
    PtusaLib& operator= (const PtusaLib& arg) = delete;
};

extern "C" ARP_CXX_SYMBOL_EXPORT void DynamicLibrary_Main(AppDomain& appDomain);
extern "C" ARP_CXX_SYMBOL_EXPORT ILibrary* DynamicLibrary_GetInstance(void);

///////////////////////////////////////////////////////////////////////////////
// inline methods of class SimulinkMode1Library

} // end of namespace PtusaLib
