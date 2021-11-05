#include "Arp/System/Core/Arp.h"
#include "Arp/Plc/Commons/Meta/TypeSystem/TypeSystem.h"
#include "PtusaProgram.hpp"
#include "PLCnextEngineerLibrary.hpp"

namespace Ptusa
{

using namespace Arp::Plc::Commons::Meta;

    void PLCnextEngineerLibrary::InitializeTypeDomain()
    {
        this->typeDomain.AddTypeDefinitions
        (
            // Begin TypeDefinitions
            {
                {   // ProgramDefinition: Ptusa::PtusaProgram
                    DataType::Program, CTN<Ptusa::PtusaProgram>(), sizeof(::Ptusa::PtusaProgram), alignof(::Ptusa::PtusaProgram), StandardAttribute::None,
                    {
                        // FieldDefinitions:
                        { "NVRAM", offsetof(::Ptusa::PtusaProgram, NVRAM), DataType::UInt8 | DataType::Array, String::Empty, sizeof(uint8), alignof(uint8), { 49000 }, StandardAttribute::Retain },
                    }
                },
            }
            // End TypeDefinitions
        );
    }

} // end of namespace Ptusa

