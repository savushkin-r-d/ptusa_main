#include <OPCUAServer.h>
#include "log.h"
#include <stdio.h>

#include "prj_mngr.h"
#include "device/device.h"
#include "device/manager.h"
#include "PAC_info.h"
#include "tech_def.h"
#include "lua_manager.h"
#include "PAC_err.h"

OPCUA_server& OPCUA_server::get_instance()
    {
    static OPCUA_server instance;
    return instance;
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

    UA_NodeId dev_root;
    //Create root object node.
    UA_ObjectAttributes oAttr = UA_ObjectAttributes_default;
    oAttr.displayName = UA_LOCALIZEDTEXT_ALLOC( "en-US", "devices" );
    oAttr.description = UA_LOCALIZEDTEXT_ALLOC( "ru-ru", "devices" );
    UA_QualifiedName qn = UA_QUALIFIEDNAME_ALLOC( 1, "devices" );
    UA_Server_addObjectNode( server, UA_NODEID_NULL,
        UA_NODEID_NUMERIC( 0, UA_NS0ID_OBJECTSFOLDER ),
        UA_NODEID_NUMERIC( 0, UA_NS0ID_ORGANIZES ),
        qn,
        UA_NODEID_NUMERIC( 0, UA_NS0ID_BASEOBJECTTYPE ),
        oAttr, nullptr, &dev_root );
    UA_ObjectAttributes_clear( &oAttr );
    UA_QualifiedName_clear( &qn );

    auto deviceCount = static_cast<u_int>( G_DEVICE_MANAGER()->get_device_count() );
    for ( u_int i = 0; i < deviceCount; i++ )
        {
        UA_NodeId deviceId;
        auto dev = G_DEVICE_MANAGER()->get_device( i );

        //Create object node.
        oAttr.displayName = UA_LOCALIZEDTEXT_ALLOC( "en-US", dev->get_name() );
        oAttr.description = UA_LOCALIZEDTEXT_ALLOC( "ru-ru", dev->get_description() );
        qn = UA_QUALIFIEDNAME_ALLOC( 1, dev->get_name() );
        UA_Server_addObjectNode( server, UA_NODEID_NULL,
            dev_root,
            UA_NODEID_NUMERIC( 0, UA_NS0ID_ORGANIZES ),
            qn,
            UA_NODEID_NUMERIC( 0, UA_NS0ID_BASEOBJECTTYPE ),
            oAttr, nullptr, &deviceId );
        UA_ObjectAttributes_clear( &oAttr );
        UA_QualifiedName_clear( &qn );

        //Creating value variable node.
        UA_VariableAttributes valueAttr = UA_VariableAttributes_default;
        valueAttr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
        UA_Float value = 0;
        UA_Variant_setScalarCopy( &valueAttr.value, &value, &UA_TYPES[ UA_TYPES_FLOAT ] );

        const std::string VALUE = "value";
        std::string node_name = dev->get_name();
        node_name += "." + VALUE;

        valueAttr.displayName = UA_LOCALIZEDTEXT_ALLOC( "en-US", VALUE.c_str() );
        valueAttr.dataType = UA_TYPES[ UA_TYPES_FLOAT ].typeId;
        UA_NodeId valueNodeId = UA_NODEID_STRING_ALLOC( 0, node_name.c_str() );

        qn = UA_QUALIFIEDNAME_ALLOC( 1, VALUE.c_str() );
        UA_Server_addVariableNode( server, valueNodeId, deviceId,
            UA_NODEID_NUMERIC( 0, UA_NS0ID_HASCOMPONENT ),
            qn,
            UA_NODEID_NUMERIC( 0, UA_NS0ID_BASEDATAVARIABLETYPE ),
            valueAttr, dev, nullptr );
        UA_VariableAttributes_clear( &valueAttr );
        UA_QualifiedName_clear( &qn );
        

        //Creating value variable read/write callbacks.
        UA_DataSource valueDataSource;
        valueDataSource.read = read_value;
        valueDataSource.write = write_value;
        UA_Server_setVariableNode_dataSource( server, valueNodeId, valueDataSource );
        UA_NodeId_clear( &valueNodeId );

        //Creating state variable node.
        UA_VariableAttributes stateAttr = UA_VariableAttributes_default;
        stateAttr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
        UA_Int32 state = 0;
        UA_Variant_setScalarCopy( &stateAttr.value, &state, &UA_TYPES[ UA_TYPES_INT32 ] );

        const std::string STATE = "state";
        node_name = dev->get_name();
        node_name += "." + STATE;

        stateAttr.displayName = UA_LOCALIZEDTEXT_ALLOC( "en-US", STATE.c_str() );
        stateAttr.dataType = UA_TYPES[ UA_TYPES_INT32 ].typeId;
        UA_NodeId stateNodeId = UA_NODEID_STRING_ALLOC( 0, node_name.c_str() );

        qn = UA_QUALIFIEDNAME_ALLOC( 1, STATE.c_str() );
        UA_Server_addVariableNode( server, stateNodeId, deviceId,
            UA_NODEID_NUMERIC( 0, UA_NS0ID_HASCOMPONENT ),
            qn,
            UA_NODEID_NUMERIC( 0, UA_NS0ID_BASEDATAVARIABLETYPE ),
            stateAttr, dev, nullptr );
        UA_VariableAttributes_clear( &stateAttr );
        UA_QualifiedName_clear( &qn );

        //Creating state variable read/write callbacks.
        UA_DataSource stateDataSource;
        stateDataSource.read = read_state;
        stateDataSource.write = write_state;
        UA_Server_setVariableNode_dataSource( server, stateNodeId, stateDataSource );
        UA_NodeId_clear( &stateNodeId );
        }

    is_dev_objects_created = true;
    }

void OPCUA_server::create_PAC_info()
    {
    if ( !server ) return;
    if ( is_PAC_info_created ) return;

    UA_NodeId PAC_NodeId;

    //Create object node.
    UA_ObjectAttributes PAC_InfoObjAttr = UA_ObjectAttributes_default;
    PAC_InfoObjAttr.displayName = UA_LOCALIZEDTEXT_ALLOC( "en-US", "PAC_info" );
    PAC_InfoObjAttr.description = UA_LOCALIZEDTEXT_ALLOC( "ru-ru", "PAC_info" );
    UA_QualifiedName qn = UA_QUALIFIEDNAME_ALLOC( 1, "PAC_info" );
    UA_Server_addObjectNode( server, UA_NODEID_NULL,
        UA_NODEID_NUMERIC( 0, UA_NS0ID_OBJECTSFOLDER ),
        UA_NODEID_NUMERIC( 0, UA_NS0ID_ORGANIZES ),
        qn,
        UA_NODEID_NUMERIC( 0, UA_NS0ID_BASEOBJECTTYPE ),
        PAC_InfoObjAttr, nullptr, &PAC_NodeId );
    UA_ObjectAttributes_clear( &PAC_InfoObjAttr );
    UA_QualifiedName_clear( &qn );

    //Uptime variable.
    UA_VariableAttributes uptimeVarAttr = UA_VariableAttributes_default;
    UA_String value = UA_String_fromChars( "0 дн. " );
    UA_Variant_setScalarCopy( &uptimeVarAttr.value, &value, &UA_TYPES[ UA_TYPES_STRING ] );
    std::string node_name = "PAC_info.uptime";
    uptimeVarAttr.displayName = UA_LOCALIZEDTEXT_ALLOC( "en-US", "uptime" );
    uptimeVarAttr.dataType = UA_TYPES[ UA_TYPES_STRING ].typeId;
    UA_NodeId uptimeNodeId = UA_NODEID_STRING_ALLOC( 0, node_name.c_str() );
    qn = UA_QUALIFIEDNAME_ALLOC( 1, node_name.c_str() );
    UA_Server_addVariableNode( server, uptimeNodeId, PAC_NodeId,
        UA_NODEID_NUMERIC( 0, UA_NS0ID_HASCOMPONENT ),
        qn,
        UA_NODEID_NUMERIC( 0, UA_NS0ID_BASEDATAVARIABLETYPE ),
        uptimeVarAttr, nullptr, nullptr );
    UA_VariableAttributes_clear( &uptimeVarAttr );
    UA_QualifiedName_clear( &qn );
    UA_String_clear( &value );    

    //PRODUCT_VERSION_FULL_STR variable.
    UA_VariableAttributes versionVarAttr = UA_VariableAttributes_default;
    UA_String version = UA_String_fromChars( PRODUCT_VERSION_FULL_STR );
    UA_Variant_setScalarCopy( &versionVarAttr.value, &version, &UA_TYPES[ UA_TYPES_STRING ] );
    node_name = "PAC_info.version";
    versionVarAttr.displayName = UA_LOCALIZEDTEXT_ALLOC( "en-US", "version" );
    versionVarAttr.dataType = UA_TYPES[ UA_TYPES_STRING ].typeId;
    UA_NodeId versionNodeId = UA_NODEID_STRING_ALLOC( 0, node_name.c_str() );
    qn = UA_QUALIFIEDNAME_ALLOC( 1, node_name.c_str() );
    UA_Server_addVariableNode( server, versionNodeId, PAC_NodeId,
        UA_NODEID_NUMERIC( 0, UA_NS0ID_HASCOMPONENT ),
        qn,
        UA_NODEID_NUMERIC( 0, UA_NS0ID_BASEDATAVARIABLETYPE ),
        versionVarAttr, nullptr, nullptr );
    UA_VariableAttributes_clear( &versionVarAttr );
    UA_QualifiedName_clear( &qn );
    UA_String_clear( &version );
    UA_NodeId_clear( &PAC_NodeId );
    UA_NodeId_clear( &versionNodeId );

    // Creating Uptime variable read callback.
    UA_DataSource uptimeDataSource{ read_PAC_info_value, nullptr };
    uptimeDataSource.read = read_PAC_info_value;
    UA_Server_setVariableNode_dataSource( server, uptimeNodeId, uptimeDataSource );
    UA_NodeId_clear( &uptimeNodeId );

    is_PAC_info_created = true;
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
        dataValue->hasValue = true;
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
            if ( G_PAC_INFO()->par[ PAC_info::P_IS_OPC_UA_SERVER_CONTROL ] == 1 )
                {
                UA_Int32 state = ( (UA_Int32*)( value->value.data ) )[ 0 ];
                dev->set_state( state );
                return UA_STATUSCODE_GOOD;
                }
            else
                {
                return UA_STATUSCODE_BADUSERACCESSDENIED;
                }
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
            if ( G_PAC_INFO()->par[ PAC_info::P_IS_OPC_UA_SERVER_CONTROL ] == 1 )
                {
                UA_Float newvalue = ( (UA_Float*)( value->value.data ) )[ 0 ];
                dev->set_value( newvalue );
                return UA_STATUSCODE_GOOD;
                }
            else
                {
                return UA_STATUSCODE_BADUSERACCESSDENIED;
                }
        }
    return UA_STATUSCODE_BAD;
    }

UA_StatusCode OPCUA_server::read_PAC_info_value( UA_Server*, const UA_NodeId*, void*,
    const UA_NodeId*, void* nodeContext, UA_Boolean, const UA_NumericRange*,
    UA_DataValue* dataValue )
    {
    static auto value = UA_STRING( const_cast<char*>( G_PAC_INFO()->get_up_time_str() ) );
    UA_Variant_setScalar( &dataValue->value, &value, &UA_TYPES[ UA_TYPES_STRING ] );
    dataValue->value.storageType = UA_VARIANT_DATA_NODELETE;
    dataValue->hasValue = true;

    return UA_STATUSCODE_GOOD;
    }

UA_Server* OPCUA_server::get_server() const
    {
    return server;
    }

OPCUA_server::~OPCUA_server()
    {
    shutdown();
    }
