#include "PtusaMainCmpntProgramProvider.hpp"
#include "PtusaMainPrg.hpp"

namespace PtusaPLCnextEngineer
{

IProgram::Ptr PtusaMainCmpntProgramProvider::CreateProgramInternal(const String& programName, const String& programType)
{
    if (programType == "PtusaMainPrg")
    { 
        return std::make_shared<::PtusaPLCnextEngineer::PtusaMainPrg>(this->ptusaMainCmpnt, programName);
    }

    // else unknown program
    return nullptr;
}

} // end of namespace PtusaPLCnextEngineer
