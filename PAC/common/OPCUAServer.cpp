#include <OPCUAServer.h>
#include "log.h"
#include <stdio.h>

#include "prj_mngr.h"
#include "PAC_dev.h"
#include "PAC_info.h"
#include "tech_def.h"
#include "lua_manager.h"
#include "PAC_err.h"

void OPCUAServer::Init( short int port )
    {
    server = UA_Server_new();
    UA_Int16 portNumber = port;
    UA_ServerConfig_setMinimal( UA_Server_getConfig( server ), portNumber, nullptr );
    }

void OPCUAServer::CreateDevObjects()
    {
    u_int deviceCount = G_DEVICE_MANAGER()->get_device_count();

    for ( u_int i = 0; i < deviceCount; i++ )
        {
        UA_NodeId deviceId;

        auto dev = G_DEVICE_MANAGER()->get_device( i );
        auto l = device::get_max_name_length();
        auto deviceName = new char[ l ];
        auto r = fmt::format_to_n( deviceName, l - 1, "{}", dev->get_name() );
        deviceName[ r.size ] = '\0';

        l = device::get_max_description_length();
        auto deviceDescription = new char[ l ];
        r = fmt::format_to_n( deviceDescription, l, "{}", dev->get_description() );
        deviceDescription[ r.size ] = '\0';

        //creating object node
        UA_ObjectAttributes oAttr = UA_ObjectAttributes_default;
        oAttr.displayName = UA_LOCALIZEDTEXT( "en-US", deviceName );
        oAttr.description = UA_LOCALIZEDTEXT( "ru-ru", deviceDescription );
        UA_Server_addObjectNode( server, UA_NODEID_NULL,
            UA_NODEID_NUMERIC( 0, UA_NS0ID_OBJECTSFOLDER ),
            UA_NODEID_NUMERIC( 0, UA_NS0ID_ORGANIZES ),
            UA_QUALIFIEDNAME( 1, deviceName ),
            UA_NODEID_NUMERIC( 0, UA_NS0ID_BASEOBJECTTYPE ),
            oAttr, nullptr, &deviceId );

        //creating value variable node
        UA_VariableAttributes valueAttr = UA_VariableAttributes_default;
        valueAttr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
        UA_Int32 value = 0;
        UA_Variant_setScalar( &valueAttr.value, &value, &UA_TYPES[ UA_TYPES_INT32 ] );

        l = device::get_max_name_length() + 6; // Extra ".Value" or ".State";
        auto valueName = new char[ l ];
        r = fmt::format_to_n( valueName, l - 1, "{}.Value", deviceName );
        valueName[ r.size ] = '\0';

        valueAttr.displayName = UA_LOCALIZEDTEXT( "en-US", valueName );
        UA_NodeId valueNodeId = UA_NODEID_STRING( 0, valueName );

        UA_Server_addVariableNode( server, valueNodeId, deviceId,
            UA_NODEID_NUMERIC( 0, UA_NS0ID_HASCOMPONENT ),
            UA_QUALIFIEDNAME( 1, "Value" ),
            UA_NODEID_NUMERIC( 0, UA_NS0ID_BASEDATAVARIABLETYPE ),
            valueAttr, G_DEVICE_MANAGER()->get_device( i ), nullptr );


        //creating value variable read/write callback
        UA_DataSource valueDataSource;
        valueDataSource.read = readValue;
        valueDataSource.write = writeValue;
        UA_Server_setVariableNode_dataSource( server, valueNodeId, valueDataSource );

        //creating state variable node
        UA_VariableAttributes stateAttr = UA_VariableAttributes_default;
        stateAttr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
        UA_Int32 state = 0;
        UA_Variant_setScalar( &stateAttr.value, &state, &UA_TYPES[ UA_TYPES_INT32 ] );

        auto stateName = new char[ l ];
        r = fmt::format_to_n( stateName, l - 1, "{}.State", deviceName );
        stateName[ r.size ] = '\0';

        stateAttr.displayName = UA_LOCALIZEDTEXT( "en-US", stateName );
        UA_NodeId stateNodeId = UA_NODEID_STRING( 0, stateName );

        UA_Server_addVariableNode( server, stateNodeId, deviceId,
            UA_NODEID_NUMERIC( 0, UA_NS0ID_HASCOMPONENT ),
            UA_QUALIFIEDNAME( 1, "State" ),
            UA_NODEID_NUMERIC( 0, UA_NS0ID_BASEDATAVARIABLETYPE ),
            stateAttr, G_DEVICE_MANAGER()->get_device( i ), nullptr );

        //creating state variable read/write callback
        UA_DataSource stateDataSource;
        stateDataSource.read = readState;
        stateDataSource.write = writeState;
        UA_Server_setVariableNode_dataSource( server, stateNodeId, stateDataSource );
        }
    }

UA_StatusCode OPCUAServer::Start()
    {
    return UA_Server_run_startup( server );
    }

void OPCUAServer::Shutdown()
    {
    UA_Server_run_shutdown( server );
    UA_Server_delete( server );
    server = 0;
    }

void OPCUAServer::Evaluate()
    {
    UA_Server_run_iterate( server, true );
    }


UA_StatusCode OPCUAServer::readState( UA_Server*, const UA_NodeId*, void*,
    const UA_NodeId*, void* nodeContext, UA_Boolean, const UA_NumericRange*,
    UA_DataValue* dataValue )
    {
    if ( nodeContext != nullptr )
        {
        auto dev = (device*)nodeContext;
        UA_Int32 state = dev->get_state();
        UA_Variant_setScalarCopy( &dataValue->value, &state, &UA_TYPES[ UA_TYPES_INT32 ] );
        }
    return UA_STATUSCODE_GOOD;
    }

UA_StatusCode OPCUAServer::writeState( UA_Server*,
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
            }
        }
    return UA_STATUSCODE_GOOD;
    }

UA_StatusCode OPCUAServer::readValue( UA_Server*, const UA_NodeId*, void*,
    const UA_NodeId*, void* nodeContext, UA_Boolean, const UA_NumericRange*,
    UA_DataValue* dataValue )
    {
    if ( nodeContext != nullptr )
        {
        auto dev = (device*)nodeContext;
        UA_Float newvalue = dev->get_value();
        UA_Variant_setScalarCopy( &dataValue->value, &newvalue, &UA_TYPES[ UA_TYPES_FLOAT ] );
        }
    return UA_STATUSCODE_GOOD;
    }

UA_StatusCode OPCUAServer::writeValue( UA_Server*,
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
            }
        }
    return UA_STATUSCODE_GOOD;
    }

OPCUAServer& OPC_UA_SERV()
    {
    return OPCUAServer::getInstance();
    }
