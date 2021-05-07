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
void i_log::write_log( PRIORITIES priority, const char* debug_message )
    {
    strncpy( msg, debug_message, C_BUFF_SIZE - 1 );
    msg[ C_BUFF_SIZE - 1 ] = 0;
    write_log( priority );
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

void i_log::notice(const char* info_message, ...)
    {
    va_list params;
    va_start(params, info_message);
    vsprintf(msg, info_message, params);
    va_end(params);
    write_log(P_NOTICE);
    }

void i_log::warning(const char* info_message, ...)
    {
    va_list params;
    va_start(params, info_message);
    vsprintf(msg, info_message, params);
    va_end(params);
    write_log(P_WARNING);
    }

void i_log::error(const char* info_message, ...)
    {
    va_list params;
    va_start(params, info_message);
    vsprintf(msg, info_message, params);
    va_end(params);
    write_log(P_ERR);
    }

void i_log::critical(const char* info_message, ...)
    {
    va_list params;
    va_start(params, info_message);
    vsprintf(msg, info_message, params);
    va_end(params);
    write_log(P_CRIT);
    }

void i_log::alert(const char* info_message, ...)
    {
    va_list params;
    va_start(params, info_message);
    vsprintf(msg, info_message, params);
    va_end(params);
    write_log(P_ALERT);
    }

void i_log::emergency(const char* info_message, ...)
    {
    va_list params;
    va_start(params, info_message);
    vsprintf(msg, info_message, params);
    va_end(params);
    write_log(P_EMERG);
    }
//-----------------------------------------------------------------------------
i_log* G_SYS_LOG()
    {
    return log_mngr::get_log();
    }
//-----------------------------------------------------------------------------
