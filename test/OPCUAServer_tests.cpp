#include "OPCUAServer_tests.h"

using namespace ::testing;

TEST( OPCUAServer, Evaluate )
    {
    OPCUAServer::getInstance().Init();
    G_DEVICE_MANAGER()->clear_io_devices();
    G_DEVICE_MANAGER()->add_io_device( device::DT_V, device::DST_V_DO1,
        "Valve1", "Test Valve", "" );
    auto valve1 = G_DEVICE_MANAGER()->get_V( "Valve1" );  
    valve1->on();

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

    UA_BrowseDescription bd;
    UA_BrowseDescription_init( &bd );
    bd.resultMask = UA_BROWSERESULTMASK_ALL;
    bd.nodeClassMask = UA_BROWSERESULTMASK_ALL;
    bd.includeSubtypes = true;
    bd.nodeId = UA_NODEID_NUMERIC( 0, UA_NS0ID_OBJECTSFOLDER );
    bd.referenceTypeId = UA_NODEID_NUMERIC( 0, UA_NS0ID_ORGANIZES );
    
    bd.browseDirection = UA_BROWSEDIRECTION_FORWARD;
    bd.includeSubtypes = true;

    auto UA_server = OPCUAServer::getInstance().get_server();
    UA_BrowseResult br = UA_Server_browse( UA_server, 0, &bd );
    EXPECT_EQ( UA_STATUSCODE_GOOD, br.statusCode );

    bool is_exist_node = false;
    size_t total = br.referencesSize;   
    for ( size_t i = 0; i < br.referencesSize; ++i )
        {
        
        auto name = br.references[ i ].browseName.name;
        char str_name[ 100 ] = { 0 };
        memcpy( str_name, name.data, name.length );
        if ( strcmp( str_name, "Valve1" ) == 0 )
            {
            //auto node = br.references[ i ].nodeId.nodeId;
            is_exist_node = true;
            }
        }   
    EXPECT_TRUE( is_exist_node );
    UA_BrowseResult_deleteMembers( &br );

    UA_Variant out;
    UA_Variant_init( &out );
    UA_NodeId valve1_state_NodeId = UA_NODEID_STRING( 0, "Valve1.State" );
    res = UA_Server_readValue( UA_server, valve1_state_NodeId, &out );
    EXPECT_EQ( UA_STATUSCODE_GOOD, res );
    EXPECT_TRUE( out.type == &UA_TYPES[ UA_TYPES_INT32 ] );
    auto state = (UA_Int32*)out.data;
    EXPECT_EQ( 1, *state );

    UA_NodeId valve1_value_NodeId = UA_NODEID_STRING( 0, "Valve1.Value" );
    res = UA_Server_readValue( UA_server, valve1_value_NodeId, &out );

    EXPECT_EQ( UA_STATUSCODE_GOOD, res );
    EXPECT_TRUE( out.type == &UA_TYPES[ UA_TYPES_FLOAT ] );
    auto value = (UA_Float*)out.data;
    EXPECT_EQ( 1.0f, *value );

    OPCUAServer::getInstance().Shutdown();
    }
