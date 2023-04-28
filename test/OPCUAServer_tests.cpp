#include "OPCUAServer_tests.h"

using namespace ::testing;

TEST( OPCUAServer, Start )
{
    OPCUAServer::getInstance().Init( 4841 );
    EXPECT_EQ( UA_STATUSCODE_GOOD, OPCUAServer::getInstance().Start() );
    OPCUAServer::getInstance().Shutdown();
}

TEST( OPCUAServer, CreateDevObjects )
{
    OPCUAServer::getInstance().Init(4841);
    G_DEVICE_MANAGER()->add_io_device(device::DT_V, device::DST_V_DO1, "Valve1", "Test Valve", "");
    OPCUAServer::getInstance().CreateDevObjects();
    EXPECT_EQ(UA_STATUSCODE_GOOD, OPCUAServer::getInstance().Start());
    OPCUAServer::getInstance().Shutdown();
}
