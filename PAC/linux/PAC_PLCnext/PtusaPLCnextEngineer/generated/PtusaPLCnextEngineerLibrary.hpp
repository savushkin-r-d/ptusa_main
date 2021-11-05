#pragma once
#include "Arp/System/Core/Arp.h"
#include "Arp/System/Core/AppDomain.hpp"
#include "Arp/System/Core/Singleton.hxx"
#include "Arp/System/Core/Library.h"
#include "Arp/Plc/Commons/Meta/MetaLibraryBase.hpp"
#include "Arp/Plc/Commons/Meta/TypeSystem/TypeDomain.hpp"

namespace PtusaPLCnextEngineer
{

using namespace Arp::System::Acf;
using namespace Arp::Plc::Commons::Meta;
using namespace Arp::Plc::Commons::Meta::TypeSystem;

class PtusaPLCnextEngineerLibrary : public MetaLibraryBase, public Singleton<PtusaPLCnextEngineerLibrary>
{
public: // typedefs
    typedef Singleton<PtusaPLCnextEngineerLibrary> SingletonBase;

public: // construction/destruction
    PtusaPLCnextEngineerLibrary(AppDomain& appDomain);
    virtual ~PtusaPLCnextEngineerLibrary() = default;

public: // static operations (called through reflection)
    static void Main(AppDomain& appDomain);

private: // methods
    void InitializeTypeDomain();

private: // deleted methods
    PtusaPLCnextEngineerLibrary(const PtusaPLCnextEngineerLibrary& arg) = delete;
    PtusaPLCnextEngineerLibrary& operator= (const PtusaPLCnextEngineerLibrary& arg) = delete;

private:  // fields
    TypeDomain typeDomain;
};

extern "C" ARP_CXX_SYMBOL_EXPORT ILibrary& ArpDynamicLibraryMain(AppDomain& appDomain);

///////////////////////////////////////////////////////////////////////////////
// inline methods of class PtusaPLCnextEngineerLibrary

} // end of namespace PtusaPLCnextEngineer
