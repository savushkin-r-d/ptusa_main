#include "OPCUAServer_tests.h"

using namespace ::testing;

TEST( OPCUAServer, Evaluate )
    {
    OPCUAServer::getInstance().Init();
    G_DEVICE_MANAGER()->add_io_device( device::DT_V, device::DST_V_DO1,
        "Valve1", "Test Valve", "" );
    OPCUAServer::getInstance().CreateDevObjects();

    auto res = OPCUAServer::getInstance().Start();
    EXPECT_EQ( UA_STATUSCODE_GOOD, res  );

    OPCUAServer::getInstance().Evaluate();
    
    res = OPCUAServer::getInstance().readState( nullptr, nullptr, nullptr,
        nullptr, nullptr, false, nullptr, nullptr );
    EXPECT_EQ( UA_STATUSCODE_GOOD, res );

    res = OPCUAServer::getInstance().writeState( nullptr, nullptr, nullptr,
        nullptr, nullptr, nullptr, nullptr );
    EXPECT_EQ( UA_STATUSCODE_GOOD, res );

    res = OPCUAServer::getInstance().readValue( nullptr, nullptr, nullptr,
        nullptr, nullptr, false, nullptr, nullptr );
    EXPECT_EQ( UA_STATUSCODE_GOOD, res );

    res = OPCUAServer::getInstance().writeValue( nullptr, nullptr, nullptr,
        nullptr, nullptr, nullptr, nullptr );
    EXPECT_EQ( UA_STATUSCODE_GOOD, res );

    OPCUAServer::getInstance().Shutdown();
    }
