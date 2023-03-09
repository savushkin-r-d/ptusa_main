#include <OPCUAServer.h>
#include "log.h"
#include <stdio.h>

#include "prj_mngr.h"
#include "PAC_dev.h"
#include "PAC_info.h"
#include "tech_def.h"
#include "lua_manager.h"
#include "PAC_err.h"

void Transliterate(const char* str, char* newStr)
    {
    for (; *str != 0; str++)
        {
        switch (str[0])
            {
            case 'а': strcat(&newStr[0], "a"); break;
            case 'б': strcat(&newStr[0], "b"); break;
            case 'в': strcat(&newStr[0], "v"); break;
            case 'г': strcat(&newStr[0], "g"); break;
            case 'д': strcat(&newStr[0], "d"); break;
            case 'е': strcat(&newStr[0], "e"); break;
            case 'ё': strcat(&newStr[0], "ye"); break;
            case 'ж': strcat(&newStr[0], "zh"); break;
            case 'з': strcat(&newStr[0], "z"); break;
            case 'и': strcat(&newStr[0], "i"); break;
            case 'й': strcat(&newStr[0], "y"); break;
            case 'к': strcat(&newStr[0], "k"); break;
            case 'л': strcat(&newStr[0], "l"); break;
            case 'м': strcat(&newStr[0], "m"); break;
            case 'н': strcat(&newStr[0], "n"); break;
            case 'о': strcat(&newStr[0], "o"); break;
            case 'п': strcat(&newStr[0], "p"); break;
            case 'р': strcat(&newStr[0], "r"); break;
            case 'с': strcat(&newStr[0], "s"); break;
            case 'т': strcat(&newStr[0], "t"); break;
            case 'у': strcat(&newStr[0], "u"); break;
            case 'ф': strcat(&newStr[0], "f"); break;
            case 'х': strcat(&newStr[0], "ch"); break;
            case 'ц': strcat(&newStr[0], "z"); break;
            case 'ч': strcat(&newStr[0], "ch"); break;
            case 'ш': strcat(&newStr[0], "sh"); break;
            case 'щ': strcat(&newStr[0], "ch"); break;
            case 'ъ': strcat(&newStr[0], "''"); break;
            case 'ы': strcat(&newStr[0], "y"); break;
            case 'ь': strcat(&newStr[0], "''"); break;
            case 'э': strcat(&newStr[0], "e"); break;
            case 'ю': strcat(&newStr[0], "yu"); break;
            case 'я': strcat(&newStr[0], "ya"); break;
            case 'А': strcat(&newStr[0], "A"); break;
            case 'Б': strcat(&newStr[0], "B"); break;
            case 'В': strcat(&newStr[0], "V"); break;
            case 'Г': strcat(&newStr[0], "G"); break;
            case 'Д': strcat(&newStr[0], "D"); break;
            case 'Е': strcat(&newStr[0], "E"); break;
            case 'Ё': strcat(&newStr[0], "Ye"); break;
            case 'Ж': strcat(&newStr[0], "Zh"); break;
            case 'З': strcat(&newStr[0], "Z"); break;
            case 'И': strcat(&newStr[0], "I"); break;
            case 'Й': strcat(&newStr[0], "Y"); break;
            case 'К': strcat(&newStr[0], "K"); break;
            case 'Л': strcat(&newStr[0], "L"); break;
            case 'М': strcat(&newStr[0], "M"); break;
            case 'Н': strcat(&newStr[0], "N"); break;
            case 'О': strcat(&newStr[0], "O"); break;
            case 'П': strcat(&newStr[0], "P"); break;
            case 'Р': strcat(&newStr[0], "R"); break;
            case 'С': strcat(&newStr[0], "S"); break;
            case 'Т': strcat(&newStr[0], "T"); break;
            case 'У': strcat(&newStr[0], "U"); break;
            case 'Ф': strcat(&newStr[0], "F"); break;
            case 'Х': strcat(&newStr[0], "Ch"); break;
            case 'Ц': strcat(&newStr[0], "Z"); break;
            case 'Ч': strcat(&newStr[0], "Ch"); break;
            case 'Ш': strcat(&newStr[0], "Sh"); break;
            case 'Щ': strcat(&newStr[0], "Ch"); break;
            case 'Ъ': strcat(&newStr[0], "''"); break;
            case 'Ы': strcat(&newStr[0], "Y"); break;
            case 'Ь': strcat(&newStr[0], "''"); break;
            case 'Э': strcat(&newStr[0], "E"); break;
            case 'Ю': strcat(&newStr[0], "Yu"); break;
            case 'Я': strcat(&newStr[0], "Ya"); break;
            default: { char Temp[2] = { str[0], 0 }; strcat(&newStr[0], &Temp[0]); }
            }
        }
    }

/* predefined identifier for later use */
UA_NodeId folderDevicesTypeId = { 1, UA_NODEIDTYPE_NUMERIC,{ 1001 } };
UA_NodeId folderPumpsTypeId = { 1, UA_NODEIDTYPE_NUMERIC,{ 1002 } };
UA_NodeId folderValvesTypeId = { 1, UA_NODEIDTYPE_NUMERIC,{ 1003 } };
UA_NodeId pumpTypeId = { 1, UA_NODEIDTYPE_NUMERIC,{ 1101 } };
UA_NodeId valveTypeId = { 1, UA_NODEIDTYPE_NUMERIC,{ 1102 } };
UA_NodeId techObjectTypeId = { 1, UA_NODEIDTYPE_NUMERIC,{ 1200 } };



void OPCUAServer::Init(int port)
{
    server = UA_Server_new();
    UA_Int16 portNumber = port;
    UA_ServerConfig_setMinimal(UA_Server_getConfig(server), portNumber, 0);
    //UA_ServerConfig_setDefault(UA_Server_getConfig(server));
}


void OPCUAServer::UserInit()
    {
    //G_LUA_MANAGER->void_exec_lua_method(0, "OPCUA_userinit", 0);
    lua_State* L = G_LUA_MANAGER->get_Lua();
    lua_getfield(L, LUA_GLOBALSINDEX, "OPCUA_userinit");
    if (lua_isfunction(L, -1))
        {
        lua_pcall(L, 0, 0, 0);
        }
    else
        {
        lua_remove(L, -1);
        }
    }

void OPCUAServer::CreateDevObjects()
    {
    u_int deviceCount = G_DEVICE_MANAGER()->get_device_count();
    char deviceName[20];
    char deviceDescription[50];

    for( u_int i = 0; i < deviceCount; i++)
        {
        UA_NodeId deviceId;
        strcpy( deviceName, G_DEVICE_MANAGER()->get_device( i )->get_name() );
        strcpy( deviceDescription, G_DEVICE_MANAGER()->get_device( i )->get_description() );

        UA_ObjectAttributes oAttr = UA_ObjectAttributes_default;
        oAttr.displayName = UA_LOCALIZEDTEXT( "en-US", deviceName );
        oAttr.description = UA_LOCALIZEDTEXT( "ru-ru", deviceDescription);
        UA_Server_addObjectNode( server, UA_NODEID_NULL,
            UA_NODEID_NUMERIC( 0, UA_NS0ID_OBJECTSFOLDER ),
            UA_NODEID_NUMERIC( 0, UA_NS0ID_ORGANIZES ),
            UA_QUALIFIEDNAME( 1, deviceName ),
            UA_NODEID_NUMERIC( 0, UA_NS0ID_BASEOBJECTTYPE ),
            oAttr, NULL, &deviceId );
        }
    }

void OPCUAServer::CreateIOModules()
    {
    u_int ioModulsCount = G_IO_MANAGER()->get_nodes_count();
    char ioModuleName[20];

    for (u_int i = 1; i < ioModulsCount; i++)
        {
        //Create Module Object
        UA_NodeId ioModuleId;
        strcpy(ioModuleName, G_IO_MANAGER()->get_node(i)->name);

        UA_ObjectAttributes oAttr = UA_ObjectAttributes_default;
        oAttr.displayName = UA_LOCALIZEDTEXT("en-US", ioModuleName);
        UA_Server_addObjectNode(server, UA_NODEID_NULL,
            UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
            UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES),
            UA_QUALIFIEDNAME(1, ioModuleName),
            UA_NODEID_NUMERIC(0, UA_NS0ID_BASEOBJECTTYPE),
            oAttr, NULL, &ioModuleId);

        //Create Module DI
        u_int DICount = G_IO_MANAGER()->get_node(i)->DI_cnt;

        for (u_int j = 0; j < DICount / 8; j++)
            {
            UA_VariableAttributes statusAttr = UA_VariableAttributes_default;
            UA_Boolean status = false;
            UA_Variant_setScalar(&statusAttr.value, &status, &UA_TYPES[UA_TYPES_BOOLEAN]);
            std::string str = "DI " + std::to_string(j);
            statusAttr.displayName = UA_LOCALIZEDTEXT("en-US", (char*)str.c_str());

            UA_Server_addVariableNode(server, UA_NODEID_NULL, ioModuleId,
                UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
                UA_QUALIFIEDNAME(1, "DI"),
                UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE),
                statusAttr, NULL, NULL);

            }

        //Create Module DO
        u_int DOCount = G_IO_MANAGER()->get_node(i)->DO_cnt;

        for (u_int j = 0; j < DOCount / 8; j++)
            {
            UA_VariableAttributes statusAttr = UA_VariableAttributes_default;
            UA_Boolean status = false;
            UA_Variant_setScalar(&statusAttr.value, &status, &UA_TYPES[UA_TYPES_BOOLEAN]);
            std::string str = "DO " + std::to_string(j);
            statusAttr.displayName = UA_LOCALIZEDTEXT("en-US", (char*)str.c_str());

            UA_Server_addVariableNode(server, UA_NODEID_NULL, ioModuleId,
                UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
                UA_QUALIFIEDNAME(1, "DO"),
                UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE),
                statusAttr, NULL, NULL);
            }

        //Create Module AI
        u_int AICount = G_IO_MANAGER()->get_node(i)->AI_cnt;

        for (u_int j = 0; j < AICount; j++)
            {
            UA_VariableAttributes statusAttr = UA_VariableAttributes_default;
            UA_Float value = 0.0f;
            UA_Variant_setScalar(&statusAttr.value, &value, &UA_TYPES[UA_TYPES_FLOAT]);
            std::string str = "AI " + std::to_string(j);
            statusAttr.displayName = UA_LOCALIZEDTEXT("en-US", (char*)str.c_str());

            UA_Server_addVariableNode(server, UA_NODEID_NULL, ioModuleId,
                UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
                UA_QUALIFIEDNAME(1, "AI"),
                UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE),
                statusAttr, NULL, NULL);
            }

        //Create Module AO
        u_int AOCount = G_IO_MANAGER()->get_node(i)->AO_cnt;

        for (u_int j = 0; j < AOCount; j++)
            {
            UA_VariableAttributes statusAttr = UA_VariableAttributes_default;
            UA_Float value = 0.0f;
            UA_Variant_setScalar(&statusAttr.value, &value, &UA_TYPES[UA_TYPES_FLOAT]);
            std::string str = "AO " + std::to_string(j);
            statusAttr.displayName = UA_LOCALIZEDTEXT("en-US", (char*)str.c_str());

            UA_Server_addVariableNode(server, UA_NODEID_NULL, ioModuleId,
                UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
                UA_QUALIFIEDNAME(1, "AO"),
                UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE),
                statusAttr, NULL, NULL);
            }
        }
    }

UA_StatusCode OPCUAServer::Start()
{
    return UA_Server_run_startup(server);
}

void OPCUAServer::Shutdown()
{
    UA_Server_run_shutdown(server);
    UA_Server_delete(server);
}

void OPCUAServer::Evaluate()
{
    UA_Server_run_iterate(server, true);
}


void OPCUAServer::BaseConfig(bool readonly)
{
    CreateFolders();
    CreateDeviceTypes(readonly);
    CreateTechObjectType(readonly);


    UA_VariableAttributes attr = UA_VariableAttributes_default;
    UA_Int32 myInteger = 42;
    UA_Variant_setScalar(&attr.value, &myInteger, &UA_TYPES[UA_TYPES_INT32]);
    attr.description = UA_LOCALIZEDTEXT("en-US", "the answer");
    attr.displayName = UA_LOCALIZEDTEXT("en-US", "the answer");
    attr.dataType = UA_TYPES[UA_TYPES_INT32].typeId;
    attr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;

    /* Add the variable node to the information model */
    UA_NodeId myIntegerNodeId = UA_NODEID_STRING(1, "the.answer");
    UA_QualifiedName myIntegerName = UA_QUALIFIEDNAME(1, "the answer");
    UA_NodeId parentNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER);
    UA_NodeId parentReferenceNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES);
    UA_Server_addVariableNode(server, myIntegerNodeId, parentNodeId,
        parentReferenceNodeId, myIntegerName,
        UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), attr, NULL, NULL);
    
}

///Создаем структуру папок.
void OPCUAServer::CreateFolders()
    {
    //папка Objects\Devices
    UA_ObjectAttributes devicesFolderAttr = UA_ObjectAttributes_default;
    devicesFolderAttr.displayName = UA_LOCALIZEDTEXT("en-US", "Devices");
    UA_Server_addObjectNode(server, UA_NODEID_NULL,
        UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
        UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES),
        UA_QUALIFIEDNAME(1, "Devices"),
        UA_NODEID_NUMERIC(0, UA_NS0ID_FOLDERTYPE),
        devicesFolderAttr, NULL, &folderDevicesTypeId);
    //папка Objects\Devices\Pumps
    UA_ObjectAttributes pumpsFolderAttr = UA_ObjectAttributes_default;
    pumpsFolderAttr.displayName = UA_LOCALIZEDTEXT("en-US", "Pumps");
    UA_Server_addObjectNode(server, UA_NODEID_NULL,
        folderDevicesTypeId,
        UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES),
        UA_QUALIFIEDNAME(1, "Pumps"),
        UA_NODEID_NUMERIC(0, UA_NS0ID_FOLDERTYPE),
        pumpsFolderAttr, NULL, &folderPumpsTypeId);
    //папка Objects\Devices\Valves
    UA_ObjectAttributes valvesFolderAttr = UA_ObjectAttributes_default;
    valvesFolderAttr.displayName = UA_LOCALIZEDTEXT("en-US", "Valves");
    UA_Server_addObjectNode(server, UA_NODEID_NULL,
        folderDevicesTypeId,
        UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES),
        UA_QUALIFIEDNAME(1, "Valves"),
        UA_NODEID_NUMERIC(0, UA_NS0ID_FOLDERTYPE),
        valvesFolderAttr, NULL, &folderValvesTypeId);
    }

void OPCUAServer::CreateDeviceTypes(bool readonly)
{
    /* Объявляем базовый класс устройства */
    UA_NodeId deviceTypeId; /* get the nodeid assigned by the server */
    UA_ObjectTypeAttributes dtAttr = UA_ObjectTypeAttributes_default;
    dtAttr.displayName = UA_LOCALIZEDTEXT("en-US", "DeviceType");
    UA_Server_addObjectTypeNode(server, UA_NODEID_NULL,
        UA_NODEID_NUMERIC(0, UA_NS0ID_BASEOBJECTTYPE),
        UA_NODEID_NUMERIC(0, UA_NS0ID_HASSUBTYPE),
        UA_QUALIFIEDNAME(1, "DeviceType"), dtAttr,
        NULL, &deviceTypeId);
    /* Добавляем поле "Description" - описание устройства*/
    UA_VariableAttributes mnAttr = UA_VariableAttributes_default;
    mnAttr.displayName = UA_LOCALIZEDTEXT("en-US", "Description");
    mnAttr.dataType = UA_NODEID_NUMERIC(0, UA_NS0ID_STRING);
    UA_NodeId deviceDescription;
    UA_Server_addVariableNode(server, UA_NODEID_NULL, deviceTypeId,
        UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
        UA_QUALIFIEDNAME(1, "Description"),
        UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), mnAttr, NULL, &deviceDescription);
    /* Делаем поле обязательным */
    UA_Server_addReference(server, deviceDescription,
        UA_NODEID_NUMERIC(0, UA_NS0ID_HASMODELLINGRULE),
        UA_EXPANDEDNODEID_NUMERIC(0, UA_NS0ID_MODELLINGRULE_MANDATORY), true);
    //====================================================================
    /* Объявление подкласса - мотор/насос "PumpType" */
    UA_ObjectTypeAttributes ptAttr = UA_ObjectTypeAttributes_default;
    ptAttr.displayName = UA_LOCALIZEDTEXT("en-US", "PumpType");
    UA_Server_addObjectTypeNode(server, pumpTypeId,
        deviceTypeId, UA_NODEID_NUMERIC(0, UA_NS0ID_HASSUBTYPE),
        UA_QUALIFIEDNAME(1, "PumpType"), ptAttr,
        NULL, NULL);

    UA_DataSource stateDataSource;
    stateDataSource.read = readState;
    stateDataSource.write = writeState;
    UA_VariableAttributes statusAttr = UA_VariableAttributes_default;
    statusAttr.displayName = UA_LOCALIZEDTEXT("en-US", "State");
    statusAttr.valueRank = -1;
    statusAttr.dataType = UA_NODEID_NUMERIC(0, UA_NS0ID_INT32);
    if (!readonly) statusAttr.accessLevel = UA_ACCESSLEVELMASK_WRITE + UA_ACCESSLEVELMASK_READ;
    UA_NodeId statusId;
    UA_Server_addDataSourceVariableNode(server, UA_NODEID_NULL, pumpTypeId,
        UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
        UA_QUALIFIEDNAME(1, "State"),
        UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), statusAttr, stateDataSource, NULL, &statusId);
    /* Make the status variable mandatory */
    UA_Server_addReference(server, statusId,
        UA_NODEID_NUMERIC(0, UA_NS0ID_HASMODELLINGRULE),
        UA_EXPANDEDNODEID_NUMERIC(0, UA_NS0ID_MODELLINGRULE_MANDATORY), true);
    
  
    UA_DataSource frequencyDataSource;
    frequencyDataSource.read = readValue;
    frequencyDataSource.write = writeValue;
    UA_VariableAttributes rpmAttr = UA_VariableAttributes_default;
    rpmAttr.displayName = UA_LOCALIZEDTEXT("en-US", "Frequency");
    rpmAttr.valueRank = -1;
    UA_UInt32 * pOutputDimensions = UA_UInt32_new();
    *pOutputDimensions = NULL;
    rpmAttr.arrayDimensions = pOutputDimensions;
    rpmAttr.dataType = UA_NODEID_NUMERIC(0, UA_NS0ID_FLOAT);
    if (!readonly) rpmAttr.accessLevel = UA_ACCESSLEVELMASK_WRITE + UA_ACCESSLEVELMASK_READ;
    UA_NodeId motorRpm;
    UA_Server_addDataSourceVariableNode(server, UA_NODEID_NULL, pumpTypeId,
        UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
        UA_QUALIFIEDNAME(1, "Frequency"),
        UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), rpmAttr, frequencyDataSource, NULL, &motorRpm);
    ///* Make the status variable mandatory */
    UA_Server_addReference(server, motorRpm,
        UA_NODEID_NUMERIC(0, UA_NS0ID_HASMODELLINGRULE),
        UA_EXPANDEDNODEID_NUMERIC(0, UA_NS0ID_MODELLINGRULE_MANDATORY), true);

    UA_NodeTypeLifecycle lifecycle;
    lifecycle.constructor = pumpTypeConstructor;
    lifecycle.destructor = NULL;
    UA_Server_setNodeTypeLifecycle(server, pumpTypeId, lifecycle);

    }

void OPCUAServer::CreateTechObjectType(bool readonly)
    {
    /* Объявляем базовый класс устройства */
    UA_ObjectTypeAttributes dtAttr = UA_ObjectTypeAttributes_default;
    dtAttr.displayName = UA_LOCALIZEDTEXT("en-US", "TechObjectType");
    UA_Server_addObjectTypeNode(server, techObjectTypeId,
        UA_NODEID_NUMERIC(0, UA_NS0ID_BASEOBJECTTYPE),
        UA_NODEID_NUMERIC(0, UA_NS0ID_HASSUBTYPE),
        UA_QUALIFIEDNAME(1, "TechObjectType"), dtAttr,
        NULL, NULL);
    /* Добавляем поле "Name" - имя объекта*/
    UA_VariableAttributes mnAttr = UA_VariableAttributes_default;
    mnAttr.displayName = UA_LOCALIZEDTEXT("en-US", "Name");
    UA_NodeId deviceDescription;
    UA_Server_addVariableNode(server, UA_NODEID_NULL, techObjectTypeId,
        UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
        UA_QUALIFIEDNAME(1, "Name"),
        UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), mnAttr, NULL, &deviceDescription);
    /* Делаем поле обязательным */
    UA_Server_addReference(server, deviceDescription,
        UA_NODEID_NUMERIC(0, UA_NS0ID_HASMODELLINGRULE),
        UA_EXPANDEDNODEID_NUMERIC(0, UA_NS0ID_MODELLINGRULE_MANDATORY), true);

    /* Добавляем поле "Number" - номер*/
    mnAttr = UA_VariableAttributes_default;
    mnAttr.displayName = UA_LOCALIZEDTEXT("en-US", "Number");
    UA_NodeId deviceNumber;
    UA_Server_addVariableNode(server, UA_NODEID_NULL, techObjectTypeId,
        UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
        UA_QUALIFIEDNAME(1, "Number"),
        UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), mnAttr, NULL, &deviceNumber);
    /* Делаем поле обязательным */
    UA_Server_addReference(server, deviceNumber,
        UA_NODEID_NUMERIC(0, UA_NS0ID_HASMODELLINGRULE),
        UA_EXPANDEDNODEID_NUMERIC(0, UA_NS0ID_MODELLINGRULE_MANDATORY), true);

    /* Добавляем поле "PAR_FLOAT" - параметры*/
    mnAttr = UA_VariableAttributes_default;
    mnAttr.displayName = UA_LOCALIZEDTEXT("en-US", "PAR_FLOAT");
    UA_NodeId parFloat;
    UA_DataSource parfloatDataSource;
    parfloatDataSource.read = readParFloat;
    parfloatDataSource.write =NULL;
    UA_Server_addDataSourceVariableNode(server, UA_NODEID_NULL, techObjectTypeId,
        UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
        UA_QUALIFIEDNAME(1, "PAR_FLOAT"),
        UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), mnAttr, parfloatDataSource, NULL, &parFloat);
    /* Делаем поле обязательным */
    UA_Server_addReference(server, parFloat,
        UA_NODEID_NUMERIC(0, UA_NS0ID_HASMODELLINGRULE),
        UA_EXPANDEDNODEID_NUMERIC(0, UA_NS0ID_MODELLINGRULE_MANDATORY), true);

    /* Добавляем поле "RT_PAR_FLOAT"*/
    mnAttr = UA_VariableAttributes_default;
    mnAttr.displayName = UA_LOCALIZEDTEXT("en-US", "RT_PAR_FLOAT");
    if (!readonly) mnAttr.accessLevel = UA_ACCESSLEVELMASK_WRITE + UA_ACCESSLEVELMASK_READ;
    UA_DataSource rtparfloatDataSource;
    rtparfloatDataSource.read = readRtParFloat;
    rtparfloatDataSource.write = writeRtParFloat;
    UA_NodeId rtparFloat;
    UA_Server_addDataSourceVariableNode(server, UA_NODEID_NULL, techObjectTypeId,
        UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
        UA_QUALIFIEDNAME(1, "RT_PAR_FLOAT"),
        UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), mnAttr, rtparfloatDataSource, NULL, &rtparFloat);
    /* Делаем поле обязательным */
    UA_Server_addReference(server, rtparFloat,
        UA_NODEID_NUMERIC(0, UA_NS0ID_HASMODELLINGRULE),
        UA_EXPANDEDNODEID_NUMERIC(0, UA_NS0ID_MODELLINGRULE_MANDATORY), true);

    /* Добавляем поле "Operations"*/
    mnAttr = UA_VariableAttributes_default;
    mnAttr.displayName = UA_LOCALIZEDTEXT("en-US", "Operations");
    if (!readonly) mnAttr.accessLevel = UA_ACCESSLEVELMASK_WRITE + UA_ACCESSLEVELMASK_READ;
    UA_DataSource operationsDataSource;
    operationsDataSource.read = readOperations;
    operationsDataSource.write = NULL;
    UA_NodeId operationsNode;
    UA_Server_addDataSourceVariableNode(server, UA_NODEID_NULL, techObjectTypeId,
        UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
        UA_QUALIFIEDNAME(1, "Operations"),
        UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), mnAttr, operationsDataSource, NULL, &operationsNode);
    /* Делаем поле обязательным */
    UA_Server_addReference(server, operationsNode,
        UA_NODEID_NUMERIC(0, UA_NS0ID_HASMODELLINGRULE),
        UA_EXPANDEDNODEID_NUMERIC(0, UA_NS0ID_MODELLINGRULE_MANDATORY), true);


    /*Добавляем конструктор типа*/
    UA_NodeTypeLifecycle lifecycle;
    lifecycle.constructor = techObjectTypeConstructor;
    lifecycle.destructor = NULL;
    UA_Server_setNodeTypeLifecycle(server, techObjectTypeId, lifecycle);
    }

void OPCUAServer::addPumpObject(device * dev)
    {
    char* name = strdup(dev->get_name());
    UA_ObjectAttributes oAttr = UA_ObjectAttributes_default;
    oAttr.displayName = UA_LOCALIZEDTEXT("en-US", name);
    UA_Server_addObjectNode(server, UA_NODEID_NULL,
        folderPumpsTypeId,
        UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES),
        UA_QUALIFIEDNAME(1, name),
        pumpTypeId, 
        oAttr, dev, NULL);
    free(name);
    }

void OPCUAServer::addTechObject(tech_object* tobj)
    {
    char* name = strdup(tobj->get_name_in_Lua());
    UA_ObjectAttributes oAttr = UA_ObjectAttributes_default;
    oAttr.displayName = UA_LOCALIZEDTEXT("en-US", name);
    UA_Server_addObjectNode(server, UA_NODEID_NULL,
        UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
        UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES),
        UA_QUALIFIEDNAME(1, name),
        techObjectTypeId, 
        oAttr, tobj, NULL);
    free(name);
    }

UA_StatusCode OPCUAServer::readState(UA_Server * server, const UA_NodeId * sessionId, void * sessionContext, const UA_NodeId * nodeId, void * nodeContext, UA_Boolean sourceTimeStamp, const UA_NumericRange * range, UA_DataValue * dataValue)
    {
    if (nodeContext != nullptr)
        {
        device* dev = (device*)nodeContext;
        UA_Int32 state = dev->get_state();
        UA_Variant_setScalarCopy(&dataValue->value, &state, &UA_TYPES[UA_TYPES_INT32]);
        }
    return UA_STATUSCODE_GOOD;
    }

UA_StatusCode OPCUAServer::writeState(UA_Server *server,
    const UA_NodeId *sessionId, void *sessionContext,
    const UA_NodeId *nodeId, void *nodeContext,
    const UA_NumericRange *range,
    const UA_DataValue *value)
    {
    if (nodeContext != nullptr)
        {
        device* dev = (device*)nodeContext;
        if (value->hasValue)
            {
            UA_Int32 state = ((UA_Int32*)(value->value.data))[0];
            dev->set_state(state);
            }
        }
    return UA_STATUSCODE_GOOD;
    }

UA_StatusCode OPCUAServer::readValue(UA_Server * server, const UA_NodeId * sessionId, void * sessionContext, const UA_NodeId * nodeId, void * nodeContext, UA_Boolean sourceTimeStamp, const UA_NumericRange * range, UA_DataValue * dataValue)
    {
    if (nodeContext != nullptr)
        {
        device* dev = (device*)nodeContext;
        UA_Float newvalue = dev->get_value();
        UA_Variant_setScalarCopy(&dataValue->value, &newvalue, &UA_TYPES[UA_TYPES_FLOAT]);
        }
    return UA_STATUSCODE_GOOD;
    }

UA_StatusCode OPCUAServer::writeValue(UA_Server *server,
    const UA_NodeId *sessionId, void *sessionContext,
    const UA_NodeId *nodeId, void *nodeContext,
    const UA_NumericRange *range,
    const UA_DataValue *value)
    {
    if (nodeContext != nullptr)
        {
        device* dev = (device*)nodeContext;
        if (value->hasValue)
            {
            UA_Float newvalue = ((UA_Float*)(value->value.data))[0];
            dev->set_value(newvalue);
            }
        }
    return UA_STATUSCODE_GOOD;
    }

UA_StatusCode OPCUAServer::readParFloat(UA_Server *server, const UA_NodeId *sessionId, void *sessionContext, const UA_NodeId *nodeId, void *nodeContext, UA_Boolean sourceTimeStamp, const UA_NumericRange *range, UA_DataValue *dataValue)
    {
    if (nodeContext != nullptr)
        {
        tech_object* t_obj = (tech_object*)nodeContext;
        int parsize = t_obj->par_float.get_count();

        void* pararray = UA_Array_new(parsize, &UA_TYPES[UA_TYPES_FLOAT]);
        UA_Variant_setArrayCopy(&dataValue->value, pararray, parsize, &UA_TYPES[UA_TYPES_FLOAT]);
        UA_Array_delete(pararray, parsize, &UA_TYPES[UA_TYPES_FLOAT]);
        for (int i = 0; i < parsize; i++)
            {
            ((UA_Float*)(dataValue->value.data))[i] = t_obj->par_float[i + 1];
            }
        }

    return UA_STATUSCODE_GOOD;
    }

UA_StatusCode OPCUAServer::readRtParFloat(UA_Server * server, const UA_NodeId * sessionId, void * sessionContext, const UA_NodeId * nodeId, void * nodeContext, UA_Boolean sourceTimeStamp, const UA_NumericRange * range, UA_DataValue * dataValue)
    {
    if (nodeContext != nullptr)
        {
        tech_object* t_obj = (tech_object*)nodeContext;
        int parsize = t_obj->rt_par_float.get_count();

        void* pararray = UA_Array_new(parsize, &UA_TYPES[UA_TYPES_FLOAT]);
        UA_Variant_setArrayCopy(&dataValue->value, pararray, parsize, &UA_TYPES[UA_TYPES_FLOAT]);
        UA_Array_delete(pararray, parsize, &UA_TYPES[UA_TYPES_FLOAT]);
        for (int i = 0; i < parsize; i++)
            {
            ((UA_Float*)(dataValue->value.data))[i] = t_obj->rt_par_float[i + 1];
            }
        }
    
    return UA_STATUSCODE_GOOD;
    }


UA_StatusCode OPCUAServer::readOperations(UA_Server *server, const UA_NodeId *sessionId, void *sessionContext, const UA_NodeId *nodeId, void *nodeContext, UA_Boolean sourceTimeStamp, const UA_NumericRange *range, UA_DataValue *dataValue)
    {
    if (nodeContext != nullptr)
        {
        tech_object* t_obj = (tech_object*)nodeContext;
        int parsize = t_obj->get_modes_count();

        void* pararray = UA_Array_new(parsize, &UA_TYPES[UA_TYPES_INT32]);
        UA_Variant_setArrayCopy(&dataValue->value, pararray, parsize, &UA_TYPES[UA_TYPES_INT32]);
        UA_Array_delete(pararray, parsize, &UA_TYPES[UA_TYPES_INT32]);
        for (int i = 0; i < parsize; i++)
            {
            ((UA_Int32*)(dataValue->value.data))[i] = t_obj->get_operation_state(i+1);
            }
        }

    return UA_STATUSCODE_GOOD;
    }

UA_StatusCode OPCUAServer::writeRtParFloat(UA_Server *server, const UA_NodeId *sessionId, void *sessionContext, const UA_NodeId *nodeId, void *nodeContext, const UA_NumericRange *range, const UA_DataValue *value)
    {
    if (nodeContext != nullptr)
        {
        tech_object* t_obj = (tech_object*)nodeContext;
        if (value->hasValue)
            {
            }
        }
    return UA_STATUSCODE_GOOD;
    }

UA_StatusCode OPCUAServer::pumpTypeConstructor(UA_Server *server, const UA_NodeId *sessionId, void *sessionContext, const UA_NodeId *typeId, void *typeContext, const UA_NodeId *nodeId, void **nodeContext)
    {
    device* dev = (device*)(nodeContext[0]);
    
    if (G_DEBUG)
        {
        UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "New pump %s created", dev->get_name());
        }

    /* Ищем дочерние ноды */
    UA_RelativePathElement rpe;
    UA_RelativePathElement_init(&rpe);
    rpe.referenceTypeId = UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT);
    rpe.isInverse = false;
    rpe.includeSubtypes = false;
    rpe.targetName = UA_QUALIFIEDNAME(1, "State");

    UA_BrowsePath bp;
    UA_BrowsePath_init(&bp);
    bp.startingNode = *nodeId;
    bp.relativePath.elementsSize = 1;
    bp.relativePath.elements = &rpe;

    UA_BrowsePathResult bpr =
        UA_Server_translateBrowsePathToNodeIds(server, &bp);
    if (bpr.statusCode != UA_STATUSCODE_GOOD ||
        bpr.targetsSize < 1)
        return bpr.statusCode;

    UA_Int32 status = 0;
    UA_Variant value;
    UA_Variant_setScalar(&value, &status, &UA_TYPES[UA_TYPES_INT32]);
    UA_Server_writeValue(server, bpr.targets[0].targetId.nodeId, value);
    UA_Server_setNodeContext(server, bpr.targets[0].targetId.nodeId, nodeContext[0]);
    UA_BrowsePathResult_deleteMembers(&bpr);

    rpe.targetName = UA_QUALIFIEDNAME(1, "Frequency");
    bpr =
        UA_Server_translateBrowsePathToNodeIds(server, &bp);
    if (bpr.statusCode != UA_STATUSCODE_GOOD ||
        bpr.targetsSize < 1)
        return bpr.statusCode;
    UA_Server_setNodeContext(server, bpr.targets[0].targetId.nodeId, nodeContext[0]);
    UA_BrowsePathResult_deleteMembers(&bpr);

    rpe.targetName = UA_QUALIFIEDNAME(1, "Description");
    bpr =
        UA_Server_translateBrowsePathToNodeIds(server, &bp);
    if (bpr.statusCode != UA_STATUSCODE_GOOD ||
        bpr.targetsSize < 1)
        return bpr.statusCode;
    int myddlen = strlen(dev->get_description());
    char mydd[100];
    strcpy(mydd, "");
    Transliterate(dev->get_description(), mydd);
    UA_String devdescr = UA_String_fromChars(mydd);
    UA_Variant_setScalar(&value, &devdescr, &UA_TYPES[UA_TYPES_STRING]);
    UA_Server_writeValue(server, bpr.targets[0].targetId.nodeId, value);
    UA_BrowsePathResult_deleteMembers(&bpr);

    return UA_STATUSCODE_GOOD;
    }


UA_StatusCode OPCUAServer::techObjectTypeConstructor(UA_Server *server, const UA_NodeId *sessionId, void *sessionContext, const UA_NodeId *typeId, void *typeContext, const UA_NodeId *nodeId, void **nodeContext)
    {
    tech_object* t_obj = (tech_object*)(nodeContext[0]);
    if (G_DEBUG)
        {
        UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "Tech Object %s created", t_obj->get_name());
        }

    /* Ищем дочерние ноды */
    UA_RelativePathElement rpe;
    UA_RelativePathElement_init(&rpe);
    rpe.referenceTypeId = UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT);
    rpe.isInverse = false;
    rpe.includeSubtypes = false;
    rpe.targetName = UA_QUALIFIEDNAME(1, "Name");
    UA_BrowsePath bp;
    UA_BrowsePath_init(&bp);
    bp.startingNode = *nodeId;
    bp.relativePath.elementsSize = 1;
    bp.relativePath.elements = &rpe;
    UA_BrowsePathResult bpr =
        UA_Server_translateBrowsePathToNodeIds(server, &bp);
    if (bpr.statusCode != UA_STATUSCODE_GOOD ||
        bpr.targetsSize < 1)
        return bpr.statusCode;
    char mydd[100];
    strcpy(mydd, "");
    UA_Variant value;
    Transliterate(t_obj->get_name(), mydd);
    UA_String devdescr = UA_String_fromChars(mydd);
    UA_Variant_setScalar(&value, &devdescr, &UA_TYPES[UA_TYPES_STRING]);
    UA_Server_writeValue(server, bpr.targets[0].targetId.nodeId, value);
    UA_BrowsePathResult_deleteMembers(&bpr);

    rpe.targetName = UA_QUALIFIEDNAME(1, "Number");
    bpr =
        UA_Server_translateBrowsePathToNodeIds(server, &bp);
    if (bpr.statusCode != UA_STATUSCODE_GOOD ||
        bpr.targetsSize < 1)
        return bpr.statusCode;
    UA_Int32 number = t_obj->get_number();
    UA_Variant_setScalar(&value, &number, &UA_TYPES[UA_TYPES_INT32]);
    UA_Server_writeValue(server, bpr.targets[0].targetId.nodeId, value);
    UA_BrowsePathResult_deleteMembers(&bpr);

    rpe.targetName = UA_QUALIFIEDNAME(1, "RT_PAR_FLOAT");
    bpr =
        UA_Server_translateBrowsePathToNodeIds(server, &bp);
    if (bpr.statusCode != UA_STATUSCODE_GOOD ||
        bpr.targetsSize < 1)
        return bpr.statusCode;
    number = t_obj->rt_par_float.get_count();
    void* myarr = UA_Array_new(number, &UA_TYPES[UA_TYPES_FLOAT]);
    UA_Variant_setArrayCopy(&value, &myarr, number, &UA_TYPES[UA_TYPES_FLOAT]);
    UA_Array_delete(myarr, number, &UA_TYPES[UA_TYPES_FLOAT]);
    UA_Server_writeValue(server, bpr.targets[0].targetId.nodeId, value);
    UA_Server_setNodeContext(server, bpr.targets[0].targetId.nodeId, nodeContext[0]);
    UA_BrowsePathResult_deleteMembers(&bpr);

    rpe.targetName = UA_QUALIFIEDNAME(1, "PAR_FLOAT");
    bpr =
        UA_Server_translateBrowsePathToNodeIds(server, &bp);
    if (bpr.statusCode != UA_STATUSCODE_GOOD ||
        bpr.targetsSize < 1)
        return bpr.statusCode;
    number = t_obj->par_float.get_count();
    myarr = UA_Array_new(number, &UA_TYPES[UA_TYPES_FLOAT]);
    UA_Variant_setArrayCopy(&value, &myarr, number, &UA_TYPES[UA_TYPES_FLOAT]);
    UA_Array_delete(myarr, number, &UA_TYPES[UA_TYPES_FLOAT]);
    UA_Server_writeValue(server, bpr.targets[0].targetId.nodeId, value);
    UA_Server_setNodeContext(server, bpr.targets[0].targetId.nodeId, nodeContext[0]);
    UA_BrowsePathResult_deleteMembers(&bpr);

    rpe.targetName = UA_QUALIFIEDNAME(1, "Operations");
    bpr =
        UA_Server_translateBrowsePathToNodeIds(server, &bp);
    if (bpr.statusCode != UA_STATUSCODE_GOOD ||
        bpr.targetsSize < 1)
        return bpr.statusCode;
    number = t_obj->get_modes_count();
    myarr = UA_Array_new(number, &UA_TYPES[UA_TYPES_INT32]);
    UA_Variant_setArrayCopy(&value, &myarr, number, &UA_TYPES[UA_TYPES_INT32]);
    UA_Array_delete(myarr, number, &UA_TYPES[UA_TYPES_INT32]);
    UA_Server_writeValue(server, bpr.targets[0].targetId.nodeId, value);
    UA_Server_setNodeContext(server, bpr.targets[0].targetId.nodeId, nodeContext[0]);
    UA_BrowsePathResult_deleteMembers(&bpr);



    return UA_STATUSCODE_GOOD;
    }

OPCUAServer& OPC_UA_SERV()
    {
    return OPCUAServer::getInstance();
    }


