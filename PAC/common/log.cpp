#include "log.h"
#include <stdarg.h>

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

void i_log::debug(const char* debug_message, ...)
	{
	if (G_DEBUG)
		{
		va_list params;
		va_start(params, debug_message);
		vsprintf(msg, debug_message, params);
		va_end(params);
		write_log(P_DEBUG);
		}
	}

void i_log::info(const char* info_message, ...)
	{
	va_list params;
	va_start(params, info_message);
	vsprintf(msg, info_message, params);
	va_end(params);
	write_log(P_INFO);
	}
