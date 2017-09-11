#pragma once
#include "Arp/System/Core/Arp.h"
#include "Arp/Plc/Esm/ProgramBase.hpp"
#include "Arp/System/Commons/Logging.h"

namespace PtusaLib { namespace MainCmp {

using namespace Arp;
using namespace Arp::Plc::Esm;

class MainPrg : public ProgramBase, private Loggable<MainPrg>
{
public: // typedefs

public: // construction/destruction
    MainPrg(const String& name);
    MainPrg(const MainPrg& arg) = delete;
    virtual ~MainPrg(void) = default;

public: // operators
    MainPrg&  operator=(const MainPrg& arg) = delete;

public: // properties

public: // operations
    void    Execute(void)override;

private: // fields

    uint32  cnt_out;
    int16   cnt_in;


};

///////////////////////////////////////////////////////////////////////////////
// inline methods of class ProgramBase

}} // end of namespace PtusaLib::MainCmp
