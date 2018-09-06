$#include "OPCUAServer.h"

$#include "PAC_dev.h"
$#include "tech_def.h"

$#ifdef WIN_OS
$#pragma warning(disable: 4800) //Warning C4800: 'int' : forcing value to bool 'true' or 'false' (performance warning)
$#endif // WIN_OS

class OPCUAServer
    {
    public:
        void BaseConfig(bool readonly);
        void addPumpObject(device* dev);
        void addTechObject(tech_object* tobj);
    };

OPCUAServer& OPC_UA_SERV();