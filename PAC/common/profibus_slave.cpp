#include "profibus_slave.h"

#if defined LINUX_OS && defined PAC_WAGO_PFC200
#include "profibus_slave_PFC200.h"
#elif defined WIN_OS && defined PAC_PC
#include "profibus_slave_PC.h"
#elif defined LINUX_OS && defined PAC_WAGO_750_860
#include "profibus_slave_w750.h"
#elif defined LINUX_OS && defined PAC_PLCNEXT
#include "profibus_slave_PC.h"
#endif

//-----------------------------------------------------------------------------
profibus_slave* profibus_slave::get_instance()
    {
#if defined LINUX_OS && defined PAC_WAGO_PFC200
    return profibus_slave_PFC200::get_instance();
#elif defined WIN_OS && defined PAC_PC
    return profibus_slave_PC::get_instance();
#elif defined LINUX_OS && defined PAC_WAGO_750_860
    return profibus_slave_w750::get_instance();
#elif defined LINUX_OS && defined PAC_PLCNEXT
    return profibus_slave_PC::get_instance();
#else
    return 0;
#endif // defined LINUX_OS && defined PAC_WAGO_PFC200
    }
//-----------------------------------------------------------------------------
profibus_slave* G_PROFIBUS_SLAVE_LUA()
    {
    return profibus_slave::get_instance();
    }
//-----------------------------------------------------------------------------
profibus_slave* G_PROFIBUS_SLAVE()
    {
    return profibus_slave::get_instance();
    }
//-----------------------------------------------------------------------------
