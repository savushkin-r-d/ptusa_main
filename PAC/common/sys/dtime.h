/// @file dtime.h
/// @brief Функции работы со временем.
///
/// @author  Иванюк Дмитрий Сергеевич.
///
/// @par Описание директив препроцессора:
/// @c WIN_OS - реализация для ОС Windows.@n@n
///
/// @par Текущая версия:
/// @$Rev: 223 $.\n
/// @$Author: id $.\n
/// @$Date:: 2011-02-17 09:39:32#$.

#ifndef DTIME_H
#define DTIME_H

#if !defined WIN_OS && !defined LINUX_OS
#error You must define OS!
#endif

#include "s_types.h"

#include <time.h>

const long MSEC_IN_SEC = 1000;
const int SEC_IN_MIN = 60;
//-----------------------------------------------------------------------------
/// @brief Получение текущего времени в секундах.
///
/// @return Текущее время.
/// @warning Время возвращается в секундах с 01.01.1970, в 2038 произойдет
/// переполнение.
u_long get_sec();
//-----------------------------------------------------------------------------
/// @brief Получение времени в миллисекундах.
///
/// Так как за 4 дня происходит переполнение и отсчет продолжается с 0, то для
/// вычисления разности времени использовать @ref get_delta_millisec.
///
/// @return Время с момента запуска программы в миллисекундах.
u_long get_millisec();
//-----------------------------------------------------------------------------
/// @brief Получение разности времени в миллисекундах.
///
/// @param time1     - начальное время.
/// @return Разность времени в миллисекундах.
u_long get_delta_millisec( u_long time1 );
//-----------------------------------------------------------------------------
/// @brief Ожидание заданное время.
///
/// @param ms - время ожидания, мс.
void sleep_ms( u_int ms );
//-----------------------------------------------------------------------------
/// @brief Получение текущей информации о времени.
///
/// @return Текущая дата и время.
extern struct tm get_time();
//-----------------------------------------------------------------------------
#ifdef WIN_OS
#pragma warning(disable : 4996) // localtime to localtime_s
#endif // WIN

struct stat_time
    {
    u_long all_time;
    u_long cycles_cnt;

    u_long  max_iteration_cycle_time;
    u_long  min_iteration_cycle_time;

    int print_cycle_last_h;

    stat_time() : all_time( 0 ), cycles_cnt( 1 ), max_iteration_cycle_time( 0 ),
	    min_iteration_cycle_time( 10000 )
	{
        time_t t_ = time( 0 );
        struct tm *timeInfo_ = localtime( &t_ );

        print_cycle_last_h = timeInfo_->tm_hour;
	}

    void clear()
	{
	all_time = 0;
	cycles_cnt = 1;
	max_iteration_cycle_time = 0;
	min_iteration_cycle_time = 10000;
	}
    };

#ifdef PTUSA_TEST
tm get_time_next_hour();

tm get_fixed_time();
#endif

#endif // DTIME_H
