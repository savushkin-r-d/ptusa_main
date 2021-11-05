#pragma once
#include "Arp/System/Core/Arp.h"
#include "Arp/Plc/Commons/Esm/ProgramProviderBase.hpp"

namespace PtusaPLCnextEngineer
{

using namespace Arp;
using namespace Arp::Plc::Commons::Esm;

//forwards
class PtusaMainCmpnt;

class PtusaMainCmpntProgramProvider : public ProgramProviderBase
{

public:   // construction/destruction
    PtusaMainCmpntProgramProvider(PtusaMainCmpnt& ptusaMainCmpntArg);
    virtual ~PtusaMainCmpntProgramProvider() = default;

public:   // IProgramProvider operations
    IProgram::Ptr CreateProgramInternal(const String& programName, const String& programType) override;

private:   // deleted methods
    PtusaMainCmpntProgramProvider(const PtusaMainCmpntProgramProvider& arg) = delete;
    PtusaMainCmpntProgramProvider& operator=(const PtusaMainCmpntProgramProvider& arg) = delete;

private: // fields
    PtusaMainCmpnt& ptusaMainCmpnt;
};

///////////////////////////////////////////////////////////////////////////////
// inline methods of class PtusaMainCmpntProgramProvider

inline PtusaMainCmpntProgramProvider::PtusaMainCmpntProgramProvider(PtusaMainCmpnt& ptusaMainCmpntArg)
    : ptusaMainCmpnt(ptusaMainCmpntArg)
{
}

} // end of namespace PtusaPLCnextEngineer
