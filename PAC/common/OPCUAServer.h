#ifndef PAC_COMMON_OPCUA_OPCUASERVER_H_
#define PAC_COMMON_OPCUA_OPCUASERVER_H_

#include "open62541.h"
#include "PAC_dev.h"

class OPCUAServer {
public:
    static OPCUAServer& getInstance()
    {
        static OPCUAServer instance;
        return instance;
    }
    void Init(int port);
    void UserInit();
    UA_StatusCode  Start();
    void Shutdown();
    void Evaluate();
    void BaseConfig(bool readonly);
    void CreateFolders();
    void CreateDeviceTypes(bool readonly);
    void addPumpObject(device* dev);

    static UA_StatusCode readState(UA_Server *server,
        const UA_NodeId *sessionId, void *sessionContext,
        const UA_NodeId *nodeId, void *nodeContext,
        UA_Boolean sourceTimeStamp, const UA_NumericRange *range,
        UA_DataValue *dataValue);

    static UA_StatusCode writeState(UA_Server *server, 
        const UA_NodeId *sessionId, void *sessionContext, 
        const UA_NodeId *nodeId, void *nodeContext, 
        const UA_NumericRange *range,
        const UA_DataValue *value);

    static UA_StatusCode readValue(UA_Server *server,
        const UA_NodeId *sessionId, void *sessionContext,
        const UA_NodeId *nodeId, void *nodeContext,
        UA_Boolean sourceTimeStamp, const UA_NumericRange *range,
        UA_DataValue *dataValue);

    static UA_StatusCode writeValue(UA_Server *server,
        const UA_NodeId *sessionId, void *sessionContext,
        const UA_NodeId *nodeId, void *nodeContext,
        const UA_NumericRange *range,
        const UA_DataValue *value);

    static UA_StatusCode
        pumpTypeConstructor(UA_Server *server,
        const UA_NodeId *sessionId, void *sessionContext,
        const UA_NodeId *typeId, void *typeContext,
        const UA_NodeId *nodeId, void **nodeContext);

private:
    OPCUAServer() {}
    ~OPCUAServer() {}
    OPCUAServer(OPCUAServer const&);
    void operator=(OPCUAServer const&);


    UA_ServerConfig* config;
    UA_ServerNetworkLayer nl;
    UA_Server *server;
    UA_Node *devicefolder;
};

OPCUAServer& OPC_UA_SERV();

#endif /* PAC_COMMON_OPCUA_OPCUASERVER_H_ */
