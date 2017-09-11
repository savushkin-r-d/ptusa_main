#pragma once
#include "Arp/System/Core/Arp.h"
#include "Arp/Plc/Esm/IProgramProvider.hpp"

using namespace Arp;
using namespace Arp::Plc::Esm;

namespace PtusaLib
{

class MainCmpProgramProvider : public IProgramProvider
{
public:                             // typedefs

public:                             // construction/destruction
	MainCmpProgramProvider(void) = default;
	MainCmpProgramProvider(const MainCmpProgramProvider& arg) = delete;
    ~MainCmpProgramProvider(void) = default;

public:                             // static operations

public:                             // operators
    MainCmpProgramProvider& operator= (const MainCmpProgramProvider& arg) = delete;

public:                             // properties

public:                             // operations
    IProgram* CreateProgram(const String& programName, const String& programType)override;

private:                            // methods

private:                            // fields

private:                            // static fields
};

} // end of namespace PtusaLib
