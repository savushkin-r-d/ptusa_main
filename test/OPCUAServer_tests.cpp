#include "OPCUAServer_tests.h"

using namespace ::testing;

TEST( OPCUAServer, Evaluate )
    {
    OPCUAServer::getInstance().Init();
    G_DEVICE_MANAGER()->add_io_device( device::DT_V, device::DST_V_DO1,
        "Valve1", "Test Valve", "" );
    OPCUAServer::getInstance().CreateDevObjects();
    EXPECT_EQ( UA_STATUSCODE_GOOD, OPCUAServer::getInstance().Start() );

    OPCUAServer::getInstance().Evaluate();

    OPCUAServer::getInstance().Shutdown();
    }
