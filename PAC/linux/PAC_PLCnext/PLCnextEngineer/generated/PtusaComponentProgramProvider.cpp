#include "PtusaComponentProgramProvider.hpp"
#include "PtusaProgram.hpp"

namespace Ptusa
{

IProgram::Ptr PtusaComponentProgramProvider::CreateProgramInternal(const String& programName, const String& programType)
{
    if (programType == "PtusaProgram")
    { 
        return std::make_shared<::Ptusa::PtusaProgram>(this->ptusaComponent, programName);
    }

    // else unknown program
    return nullptr;
}

} // end of namespace Ptusa
