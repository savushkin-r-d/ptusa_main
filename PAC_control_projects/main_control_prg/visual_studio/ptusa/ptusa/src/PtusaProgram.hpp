#pragma once
#include "Arp/System/Core/Arp.h"
#include "Arp/Plc/Commons/Esm/ProgramBase.hpp"
#include "Arp/System/Commons/Logging.h"
#include "PtusaComponent.hpp"

namespace Ptusa
    {
    using namespace Arp;
    using namespace Arp::System::Commons::Diagnostics::Logging;
    using namespace Arp::Plc::Commons::Esm;

    //#program
    //#component(Ptusa::PtusaComponent)
    class PtusaProgram : public ProgramBase, private Loggable<PtusaProgram>
        {
        public: // typedefs

        public: // construction/destruction
            PtusaProgram( Ptusa::PtusaComponent& ptusaComponentArg, const String& name );
            PtusaProgram( const PtusaProgram& arg ) = delete;
            virtual ~PtusaProgram() = default;

        public: // operators
            PtusaProgram& operator=( const PtusaProgram& arg ) = delete;

        public: // properties

        public: // operations
            void    Execute() override;

        public: /* Ports
                   =====
                   Ports are defined in the following way:
                   //#port
                   //#attributes(Input|Retain)
                   //#name(NameOfPort)
                   boolean portField;

                   The attributes comment define the port attributes and is optional.
                   The name comment defines the name of the port and is optional. Default is the name of the field.
                */

            uint8 NVRAM[ 49000 ] = { 0 };

        private: // fields
            Ptusa::PtusaComponent& ptusaComponent;

        };

    ///////////////////////////////////////////////////////////////////////////////
    // inline methods of class ProgramBase
    inline PtusaProgram::PtusaProgram( Ptusa::PtusaComponent& ptusaComponentArg, const String& name )
        : ProgramBase( name )
        , ptusaComponent( ptusaComponentArg )
        {
        }

    } // end of namespace Ptusa
