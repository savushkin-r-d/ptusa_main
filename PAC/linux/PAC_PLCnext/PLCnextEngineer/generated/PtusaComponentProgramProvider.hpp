#pragma once
#include "Arp/System/Core/Arp.h"
#include "Arp/Plc/Commons/Esm/ProgramProviderBase.hpp"

namespace Ptusa
{

using namespace Arp;
using namespace Arp::Plc::Commons::Esm;

//forwards
class PtusaComponent;

class PtusaComponentProgramProvider : public ProgramProviderBase
{

public:   // construction/destruction
    PtusaComponentProgramProvider(PtusaComponent& ptusaComponentArg);
    virtual ~PtusaComponentProgramProvider() = default;

public:   // IProgramProvider operations
    IProgram::Ptr CreateProgramInternal(const String& programName, const String& programType) override;

private:   // deleted methods
    PtusaComponentProgramProvider(const PtusaComponentProgramProvider& arg) = delete;
    PtusaComponentProgramProvider& operator=(const PtusaComponentProgramProvider& arg) = delete;

private: // fields
    PtusaComponent& ptusaComponent;
};

///////////////////////////////////////////////////////////////////////////////
// inline methods of class PtusaComponentProgramProvider

inline PtusaComponentProgramProvider::PtusaComponentProgramProvider(PtusaComponent& ptusaComponentArg)
    : ptusaComponent(ptusaComponentArg)
{
}

} // end of namespace Ptusa
