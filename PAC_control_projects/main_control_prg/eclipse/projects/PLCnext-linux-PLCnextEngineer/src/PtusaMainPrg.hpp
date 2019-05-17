#pragma once
#include "Arp/System/Core/Arp.h"
#include "Arp/Plc/Commons/Esm/ProgramBase.hpp"
#include "Arp/System/Commons/Logging.h"
#include "PtusaMainCmpnt.hpp"

namespace PtusaPLCnextEngineer
    {
    using namespace Arp;
    using namespace Arp::System::Commons::Diagnostics::Logging;
    using namespace Arp::Plc::Commons::Esm;

    //#program
    //#component(PtusaPLCnextEngineer::PtusaMainCmpnt)
    class PtusaMainPrg: public ProgramBase, private Loggable<PtusaMainPrg>
        {
    public:
        // typedefs

    public:
        // construction/destruction
        PtusaMainPrg(PtusaPLCnextEngineer::PtusaMainCmpnt& ptusaMainCmpntArg,
                const String& name);
        PtusaMainPrg(const PtusaMainPrg& arg) = delete;
        virtual ~PtusaMainPrg() = default;

    public:
        // operators
        PtusaMainPrg& operator=(const PtusaMainPrg& arg) = delete;

    public:
        // properties

    public:
        // operations
        void Execute() override;

    public:
        /* Ports
         =====
         Ports are defined in the following way:
         //#port
         //#attributes(Input|Retain)
         //#name(NameOfPort)
         boolean portField;

         The attributes comment define the port attributes and is optional.
         The name comment defines the name of the port and is optional. Default is the name of the field.
         */

    private:
        // fields
        PtusaPLCnextEngineer::PtusaMainCmpnt& ptusaMainCmpnt;

        };

    ///////////////////////////////////////////////////////////////////////////////
    // inline methods of class ProgramBase
    inline PtusaMainPrg::PtusaMainPrg(
            PtusaPLCnextEngineer::PtusaMainCmpnt& ptusaMainCmpntArg,
            const String& name) :
            ProgramBase(name), ptusaMainCmpnt(ptusaMainCmpntArg)
        {
        }
    } // end of namespace PtusaPLCnextEngineer
