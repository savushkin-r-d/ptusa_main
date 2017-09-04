#pragma once
#include "Arp/System/Core/Arp.h"
#include "Arp/System/Core/AppDomain.hpp"
#include "Arp/System/Core/Singleton.hxx"
#include "Arp/System/Core/Library.h"
#include "Arp/System/Acf/LibraryBase.hpp"

using namespace Arp;
using namespace Arp::System::Acf;

namespace ptusa
{

class ptusa : public LibraryBase, public Singleton<ptusa>
{
public: // typedefs
    typedef Singleton<ptusa> TSingletonBase;

public: // construction/destruction
    ptusa(AppDomain& appDomain);
    virtual ~ptusa(void) = default;

public: // overridden operations

public: // static operations (called through reflection)
    static void Main(AppDomain& appDomain);
    static ILibrary* GetInstance(void);

private: // methods
    ptusa(const ptusa& arg) = delete;
    ptusa& operator= (const ptusa& arg) = delete;
};

extern "C" ARP_CXX_SYMBOL_EXPORT void DynamicLibrary_Main(AppDomain& appDomain);
extern "C" ARP_CXX_SYMBOL_EXPORT ILibrary* DynamicLibrary_GetInstance(void);

///////////////////////////////////////////////////////////////////////////////
// inline methods of class SimulinkMode1Library

} // end of namespace ptusa
