#ifndef PAC_COMMON_OPCUA_OPCUASERVER_H_
#define PAC_COMMON_OPCUA_OPCUASERVER_H_

#include "open62541.h"

class OPCUAServer {
public:
    static OPCUAServer& getInstance()
    {
        static OPCUAServer instance;
        return instance;
    }
    void Init(int port);
    UA_StatusCode  Start();
    void Shutdown();
    void Evaluate();
    void TestConfig();
private:
    OPCUAServer() {}
    ~OPCUAServer() {}
    OPCUAServer(OPCUAServer const&);
    void operator=(OPCUAServer const&);


    UA_ServerConfig config;
    UA_ServerNetworkLayer nl;
    UA_Server *server;
};

#endif /* PAC_COMMON_OPCUA_OPCUASERVER_H_ */
