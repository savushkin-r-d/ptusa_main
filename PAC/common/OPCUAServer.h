#ifndef PAC_COMMON_OPCUA_OPCUASERVER_H_
#define PAC_COMMON_OPCUA_OPCUASERVER_H_

#include <open62541/plugin/log_stdout.h>
#include <open62541/server.h>
#include <open62541/server_config_default.h>
#include <open62541/types_generated.h>

#include "PAC_dev.h"
#include "tech_def.h"

class OPCUAServer
    {
    public:
        static OPCUAServer& getInstance()
            {
            static OPCUAServer instance;
            return instance;
            }

        void Init( short int port );

        void CreateDevObjects();
        UA_StatusCode  Start();
        void Shutdown();
        void Evaluate();

        static UA_StatusCode readState( UA_Server* server,
            const UA_NodeId* sessionId, void* sessionContext,
            const UA_NodeId* nodeId, void* nodeContext,
            UA_Boolean sourceTimeStamp, const UA_NumericRange* range,
            UA_DataValue* dataValue );

        static UA_StatusCode writeState( UA_Server* server,
            const UA_NodeId* sessionId, void* sessionContext,
            const UA_NodeId* nodeId, void* nodeContext,
            const UA_NumericRange* range,
            const UA_DataValue* value );

        static UA_StatusCode readValue( UA_Server* server,
            const UA_NodeId* sessionId, void* sessionContext,
            const UA_NodeId* nodeId, void* nodeContext,
            UA_Boolean sourceTimeStamp, const UA_NumericRange* range,
            UA_DataValue* dataValue );

        static UA_StatusCode writeValue( UA_Server* server,
            const UA_NodeId* sessionId, void* sessionContext,
            const UA_NodeId* nodeId, void* nodeContext,
            const UA_NumericRange* range,
            const UA_DataValue* value );

    private:
        OPCUAServer() {}
        ~OPCUAServer() {}
        OPCUAServer( OPCUAServer const& );
        OPCUAServer& operator=( OPCUAServer const& );


        UA_ServerConfig* config;
        UA_Server* server;
        UA_Node* devicefolder;
    };

OPCUAServer& OPC_UA_SERV();

#endif /* PAC_COMMON_OPCUA_OPCUASERVER_H_ */
