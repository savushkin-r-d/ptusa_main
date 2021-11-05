#pragma once
#include "Arp/System/Core/Arp.h"
#include "Arp/System/Core/AppDomain.hpp"
#include "Arp/System/Core/Singleton.hxx"
#include "Arp/System/Core/Library.h"
#include "Arp/Plc/Commons/Meta/MetaLibraryBase.hpp"
#include "Arp/Plc/Commons/Meta/TypeSystem/TypeDomain.hpp"

namespace Ptusa
{

using namespace Arp::System::Acf;
using namespace Arp::Plc::Commons::Meta;
using namespace Arp::Plc::Commons::Meta::TypeSystem;

class PtusaLibrary : public MetaLibraryBase, public Singleton<PtusaLibrary>
{
public: // typedefs
    typedef Singleton<PtusaLibrary> SingletonBase;

public: // construction/destruction
    PtusaLibrary(AppDomain& appDomain);
    virtual ~PtusaLibrary() = default;

public: // static operations (called through reflection)
    static void Main(AppDomain& appDomain);

private: // methods
    void InitializeTypeDomain();

private: // deleted methods
    PtusaLibrary(const PtusaLibrary& arg) = delete;
    PtusaLibrary& operator= (const PtusaLibrary& arg) = delete;

private:  // fields
    TypeDomain typeDomain;
};

extern "C" ARP_CXX_SYMBOL_EXPORT ILibrary& ArpDynamicLibraryMain(AppDomain& appDomain);

///////////////////////////////////////////////////////////////////////////////
// inline methods of class PtusaLibrary

} // end of namespace Ptusa
