#include "mainProgramProvider.hpp"
#include "Programs/main.hpp"

namespace ptusa
{

IProgram* mainProgramProvider::CreateProgram(const String& programName, const String& programType)
{
    if (programType == "main")
    {
        return new main::main(programName);
    }

    // else unknown program
    return nullptr;
}

} // end of namespace ptusa
