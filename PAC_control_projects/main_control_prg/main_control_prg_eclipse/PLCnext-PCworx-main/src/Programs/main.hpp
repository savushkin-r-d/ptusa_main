#pragma once
#include "Arp/System/Core/Arp.h"
#include "Arp/Plc/Esm/ProgramBase.hpp"
#include "Arp/System/Commons/Logging.h"

namespace ptusa { namespace main {

using namespace Arp;
using namespace Arp::Plc::Esm;

class main : public ProgramBase, private Loggable<main>
{
public: // typedefs

public: // construction/destruction
    main(const String& name);
    main(const main& arg) = delete;
    virtual ~main(void) = default;

public: // operators
    main&  operator=(const main& arg) = delete;

public: // properties

public: // operations
    void    Execute(void)override;

private: // fields

};

///////////////////////////////////////////////////////////////////////////////
// inline methods of class ProgramBase

}} // end of namespace ptusa::main
