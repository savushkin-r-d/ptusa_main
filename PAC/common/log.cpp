#include "log.h"

#ifdef LINUX_OS
#include "l_log.h"
i_log* log_mngr::lg = new l_log();
#endif // LINUX_OS

#ifdef WIN_OS
#include "w_log.h"
i_log* log_mngr::lg = new w_log();
#endif // WIN_OS

//-----------------------------------------------------------------------------
i_log* log_mngr::get_log()
    {
    return lg;
    }
//-----------------------------------------------------------------------------
