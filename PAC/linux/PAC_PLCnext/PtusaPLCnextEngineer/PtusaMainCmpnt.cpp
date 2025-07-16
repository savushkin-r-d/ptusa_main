#include "PtusaMainCmpnt.hpp"
#include "Arp/Plc/Commons/Esm/ProgramComponentBase.hpp"

namespace PtusaPLCnextEngineer
    {
    void PtusaMainCmpnt::Initialize()
        {
        // never remove next line
        ProgramComponentBase::Initialize();

        // subscribe events from the event system (Nm) here
        }

    void PtusaMainCmpnt::LoadConfig()
        {
        // load project config here
        }

    void PtusaMainCmpnt::SetupConfig()
        {
        // never remove next line
        ProgramComponentBase::SetupConfig();

        // setup project config here
        }

    void PtusaMainCmpnt::ResetConfig()
        {
        // never remove next line
        ProgramComponentBase::ResetConfig();

        // implement this inverse to SetupConfig() and LoadConfig()
        }
    } // end of namespace PtusaPLCnextEngineer
