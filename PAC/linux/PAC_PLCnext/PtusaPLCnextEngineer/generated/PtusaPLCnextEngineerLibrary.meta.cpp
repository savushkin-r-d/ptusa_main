#include "Arp/System/Core/Arp.h"
#include "Arp/Plc/Commons/Meta/TypeSystem/TypeSystem.h"
#include "PtusaMainPrg.hpp"
#include "PtusaPLCnextEngineerLibrary.hpp"

namespace PtusaPLCnextEngineer
{

using namespace Arp::Plc::Commons::Meta;

    void PtusaPLCnextEngineerLibrary::InitializeTypeDomain()
    {
        this->typeDomain.AddTypeDefinitions
        (
            // Begin TypeDefinitions
            {
                {   // ProgramDefinition: PtusaPLCnextEngineer::PtusaMainPrg
                    DataType::Program, CTN<PtusaPLCnextEngineer::PtusaMainPrg>(), sizeof(::PtusaPLCnextEngineer::PtusaMainPrg), alignof(::PtusaPLCnextEngineer::PtusaMainPrg), StandardAttribute::None,
                    {
                        // FieldDefinitions:
                        { "NVRAM", offsetof(::PtusaPLCnextEngineer::PtusaMainPrg, NVRAM), DataType::UInt8 | DataType::Array, "", sizeof(uint8), alignof(uint8), { 49000 }, StandardAttribute::Output | StandardAttribute::Retain },
                    }
                },
            }
            // End TypeDefinitions
        );
    }

} // end of namespace PtusaPLCnextEngineer

