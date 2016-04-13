#include "profibus_slave.h"

#ifdef USE_PROFIBUS_SLAVE_PFC200
#include "profibus_slave_PFC200.h"
#elif defined WIN_OS && defined PAC_PC
#include "profibus_slave_PC.h"
#endif // USE_PROFIBUS_SLAVE_PFC200

//-----------------------------------------------------------------------------
profibus_slave* profibus_slave::get_instance()
    {
#ifdef USE_PROFIBUS_SLAVE_PFC200
    return profibus_slave_PFC200::get_instance();
#elif defined WIN_OS && defined PAC_PC
    return profibus_slave_PC::get_instance();
#else
    return 0;
#endif // USE_PROFIBUS_SLAVE_PFC200
    }
//-----------------------------------------------------------------------------
profibus_slave* G_PROFIBUS_SLAVE_LUA()
    {
    return profibus_slave::get_instance();
    }
//-----------------------------------------------------------------------------
