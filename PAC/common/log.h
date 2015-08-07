/// @file log.h
/// @brief Работа с протоколированием событий, ошибок и т.д.
///
/// @author  Иванюк Дмитрий Сергеевич.
///
/// @par Текущая версия:
/// @$Rev: 332 $.\n
/// @$Author: id $.\n
/// @$Date:: 2011-08-24 09:59:40#$.

#ifndef LOG_H
#define LOG_H

#include "string.h"

//-----------------------------------------------------------------------------
/// @brief Работа с журналом.
///
/// Базовый интерфейсный класс для журнала.
/// Содержит основные методы работы - запиь в лог, т.д.
///
class i_log
    {
    enum CONSTANT
        {
        C_BUFF_SIZE = 1024,
        };

    public:

    virtual ~i_log()
        {
        }

    char msg[ C_BUFF_SIZE ];

    enum PRIORITIES
        {
        P_EMERG, 	// System is unusable
        P_ALERT,	// Action must be taken immediately
        P_CRIT,		// Critical conditions
        P_ERR,		// Error conditions
        P_WARNING,	// Warning conditions
        P_NOTICE,	// Normal but significant condition
        P_INFO,		// Informational
        P_DEBUG,	// Debug-level messages
        };

    /// @brief Запись строки в журнал. Строка должна быть в @msg.
    ///
    /// @param priority - приоритет.
    void virtual write_log( PRIORITIES priority ) = 0;

    protected:

    i_log()
        {
        memset( msg, 0, C_BUFF_SIZE );
        }
    };
//-----------------------------------------------------------------------------
class log_mngr
    {
    public:

    static i_log* get_log();

    private:

    log_mngr()
        {
        }


    ~log_mngr()
        {
        delete lg;
        lg = 0;
        }

    static i_log *lg;
    };
//-----------------------------------------------------------------------------
#define G_LOG log_mngr::get_log()

#endif // LOG_H
