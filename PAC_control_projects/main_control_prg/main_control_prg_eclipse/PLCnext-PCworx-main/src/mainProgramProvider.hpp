#pragma once
#include "Arp/System/Core/Arp.h"
#include "Arp/Plc/Esm/IProgramProvider.hpp"

using namespace Arp;
using namespace Arp::Plc::Esm;

namespace ptusa
{

class mainProgramProvider : public IProgramProvider
{
public:                             // typedefs

public:                             // construction/destruction
	mainProgramProvider(void) = default;
	mainProgramProvider(const mainProgramProvider& arg) = delete;
    ~mainProgramProvider(void) = default;

public:                             // static operations

public:                             // operators
    mainProgramProvider& operator= (const mainProgramProvider& arg) = delete;

public:                             // properties

public:                             // operations
    IProgram* CreateProgram(const String& programName, const String& programType)override;

private:                            // methods

private:                            // fields

private:                            // static fields
};

} // end of namespace ptusa
