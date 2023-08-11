#include <OPCUAServer.h>
#include "log.h"
#include <stdio.h>

#include "prj_mngr.h"
#include "PAC_dev.h"
#include "PAC_info.h"
#include "tech_def.h"
#include "lua_manager.h"
#include "PAC_err.h"

OPCUA_server& OPCUA_server::get_instance()
    {
    static OPCUA_server instance;
    return instance;
    }


bool OPCUA_server::is_init() const
    {
    return server != nullptr;
    }

void OPCUA_server::init( short int port )
    {
    if ( nullptr == server )
        {
        server = UA_Server_new();
        UA_Int16 portNumber = port;
        UA_ServerConfig_setMinimal( UA_Server_getConfig( server ), portNumber, nullptr );
        }
    }

void OPCUA_server::create_dev_objects()
    {
    if ( !server ) return;
    if ( is_dev_objects_created ) return;

    auto deviceCount = static_cast<u_int>( G_DEVICE_MANAGER()->get_device_count() );
    for ( u_int i = 0; i < deviceCount; i++ )
        {
        UA_NodeId deviceId;
        auto dev = G_DEVICE_MANAGER()->get_device( i );

        //Create object node.
        UA_ObjectAttributes oAttr = UA_ObjectAttributes_default;
        oAttr.displayName = UA_LOCALIZEDTEXT_ALLOC( "en-US", dev->get_name() );
        oAttr.description = UA_LOCALIZEDTEXT_ALLOC( "ru-ru", dev->get_description() );
        UA_Server_addObjectNode( server, UA_NODEID_NULL,
            UA_NODEID_NUMERIC( 0, UA_NS0ID_OBJECTSFOLDER ),
            UA_NODEID_NUMERIC( 0, UA_NS0ID_ORGANIZES ),
            UA_QUALIFIEDNAME_ALLOC( 1, dev->get_name() ),
            UA_NODEID_NUMERIC( 0, UA_NS0ID_BASEOBJECTTYPE ),
            oAttr, nullptr, &deviceId );

        //Creating value variable node.
        UA_VariableAttributes valueAttr = UA_VariableAttributes_default;
        valueAttr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
        UA_Float value = 0;
        UA_Variant_setScalar( &valueAttr.value, &value, &UA_TYPES[ UA_TYPES_FLOAT ] );

        const std::string VALUE = "Value";
        std::string node_name = dev->get_name();
        node_name += "." + VALUE;

        valueAttr.displayName = UA_LOCALIZEDTEXT_ALLOC( "en-US", VALUE.c_str() );
        valueAttr.dataType = UA_TYPES[ UA_TYPES_FLOAT ].typeId;
        UA_NodeId valueNodeId = UA_NODEID_STRING_ALLOC( 0, node_name.c_str() );

        UA_Server_addVariableNode( server, valueNodeId, deviceId,
            UA_NODEID_NUMERIC( 0, UA_NS0ID_HASCOMPONENT ),
            UA_QUALIFIEDNAME_ALLOC( 1, VALUE.c_str() ),
            UA_NODEID_NUMERIC( 0, UA_NS0ID_BASEDATAVARIABLETYPE ),
            valueAttr, dev, nullptr );


        //Creating value variable read/write callbacks.
        UA_DataSource valueDataSource;
        valueDataSource.read = read_value;
        valueDataSource.write = write_value;
        UA_Server_setVariableNode_dataSource( server, valueNodeId, valueDataSource );

        //Creating state variable node.
        UA_VariableAttributes stateAttr = UA_VariableAttributes_default;
        stateAttr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
        UA_Int32 state = 0;
        UA_Variant_setScalar( &stateAttr.value, &state, &UA_TYPES[ UA_TYPES_INT32 ] );

        const std::string STATE = "State";
        node_name = dev->get_name();
        node_name += "." + STATE;

        stateAttr.displayName = UA_LOCALIZEDTEXT_ALLOC( "en-US", STATE.c_str() );
        stateAttr.dataType = UA_TYPES[ UA_TYPES_INT32 ].typeId;
        UA_NodeId stateNodeId = UA_NODEID_STRING_ALLOC( 0, node_name.c_str() );

        UA_Server_addVariableNode( server, stateNodeId, deviceId,
            UA_NODEID_NUMERIC( 0, UA_NS0ID_HASCOMPONENT ),
            UA_QUALIFIEDNAME_ALLOC( 1, STATE.c_str() ),
            UA_NODEID_NUMERIC( 0, UA_NS0ID_BASEDATAVARIABLETYPE ),
            stateAttr, dev, nullptr );

        //Creating state variable read/write callbacks.
        UA_DataSource stateDataSource;
        stateDataSource.read = read_state;
        stateDataSource.write = write_state;
        UA_Server_setVariableNode_dataSource( server, stateNodeId, stateDataSource );
        }

    is_dev_objects_created = true;
    }

UA_StatusCode OPCUA_server::start()
    {
    return UA_Server_run_startup( server );
    }

void OPCUA_server::shutdown()
    {
    if ( server )
        {
        UA_Server_run_shutdown( server );
        UA_Server_delete( server );
        server = nullptr;

        is_dev_objects_created = false;
        }
    }

void OPCUA_server::evaluate()
    {
    if ( server ) UA_Server_run_iterate( server, true );
    }


UA_StatusCode OPCUA_server::read_state( UA_Server*, const UA_NodeId*, void*,
    const UA_NodeId*, void* nodeContext, UA_Boolean, const UA_NumericRange*,
    UA_DataValue* dataValue )
    {
    if ( nodeContext != nullptr )
        {
        auto dev = (device*)nodeContext;
        UA_Int32 state = dev->get_state();
        UA_Variant_setScalarCopy( &dataValue->value, &state, &UA_TYPES[ UA_TYPES_INT32 ] );
        return UA_STATUSCODE_GOOD;
        }

    return UA_STATUSCODE_BAD;
    }

UA_StatusCode OPCUA_server::write_state( UA_Server*,
    const UA_NodeId*, void*,
    const UA_NodeId*, void* nodeContext,
    const UA_NumericRange*,
    const UA_DataValue* value )
    {
    if ( nodeContext != nullptr )
        {
        auto dev = (device*)nodeContext;
        if ( value->hasValue )
            {
            UA_Int32 state = ( (UA_Int32*)( value->value.data ) )[ 0 ];
            dev->set_state( state );
            return UA_STATUSCODE_GOOD;
            }
        }
    return UA_STATUSCODE_BAD;
    }

UA_StatusCode OPCUA_server::read_value( UA_Server*, const UA_NodeId*, void*,
    const UA_NodeId*, void* nodeContext, UA_Boolean, const UA_NumericRange*,
    UA_DataValue* dataValue )
    {
    if ( nodeContext != nullptr )
        {
        auto dev = (device*)nodeContext;
        UA_Float newvalue = dev->get_value();
        UA_Variant_setScalarCopy( &dataValue->value, &newvalue, &UA_TYPES[ UA_TYPES_FLOAT ] );
        return UA_STATUSCODE_GOOD;
        }
    return UA_STATUSCODE_BAD;
    }

UA_StatusCode OPCUA_server::write_value( UA_Server*,
    const UA_NodeId*, void*,
    const UA_NodeId*, void* nodeContext,
    const UA_NumericRange*,
    const UA_DataValue* value )
    {
    if ( nodeContext != nullptr )
        {
        auto dev = (device*)nodeContext;
        if ( value->hasValue )
            {
            UA_Float newvalue = ( (UA_Float*)( value->value.data ) )[ 0 ];
            dev->set_value( newvalue );
            return UA_STATUSCODE_GOOD;
            }
        }
    return UA_STATUSCODE_BAD;
    }

UA_Server* OPCUA_server::get_server() const
    {
    return server;
    }

OPCUA_server::~OPCUA_server()
    {
    shutdown();
    }
