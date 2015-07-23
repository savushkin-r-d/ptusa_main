/// @file l_log.h
/// @brief Работа с протоколированием событий, ошибок и т.д. для ОС Linux.
///
/// @author  Иванюк Дмитрий Сергеевич.
///
/// @par Текущая версия:
/// @$Rev: 332 $.\n
/// @$Author: id $.\n
/// @$Date:: 2011-08-24 09:59:40#$.

#ifndef L_LOG_H
#define L_LOG_H

#include "log.h"

#include <syslog.h>

#include <stdio.h>
//-----------------------------------------------------------------------------
/// @brief Работа с журналом.
///
/// Базовый интерфейсный класс для журнала.
/// Содержит основные методы работы - запиь в лог, т.д.
///
class l_log: public i_log
    {
    public:

    virtual ~l_log()
        {
        }

    void virtual write_log(PRIORITIES priority)
        {
        //Using openlog() because kbus functions also use it
        //(openlog( "kbus", ...)).
        openlog( "ptusa", LOG_PID | LOG_PERROR, LOG_USER );

        syslog( (int) priority, "%s", msg );
        closelog();
        }

    l_log()
        {
        }
};

#endif // L_LOG_H
