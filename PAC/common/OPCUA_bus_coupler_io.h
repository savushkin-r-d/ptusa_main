#include "bus_coupler_io.h"
#include "OPCUAServer.h"
#include "dtime.h"
#include "PAC_err.h"

//-----------------------------------------------------------------------------
/// @brief Работа с модулями ввода/вывода c помощью OPCUA.
///
///
class io_manager_OPCUA : public io_manager
{

    int read_inputs();
    int write_outputs();

public:
    io_manager_OPCUA();

    virtual ~io_manager_OPCUA();
};

