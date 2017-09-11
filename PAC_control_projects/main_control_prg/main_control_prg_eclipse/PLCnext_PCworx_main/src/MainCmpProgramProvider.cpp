#include "MainCmpProgramProvider.hpp"
#include "Programs/MainPrg.hpp"

namespace PtusaLib
{

IProgram* MainCmpProgramProvider::CreateProgram(const String& programName, const String& programType)
{
    if (programType == "MainPrg")
    {
        return new MainCmp::MainPrg(programName);
    }

    // else unknown program
    return nullptr;
}

} // end of namespace PtusaLib
