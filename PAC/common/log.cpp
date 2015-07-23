#include "log.h"

#ifdef LINUX_OS
#include "l_log.h"
#endif // LINUX_OS

i_log* log_mngr::lg = new l_log();
//-----------------------------------------------------------------------------
i_log* log_mngr::get_log()
    {
    return lg;
    }
//-----------------------------------------------------------------------------
