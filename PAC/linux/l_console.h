/// @file l_console.h
/// @brief Работа с консолью в ОС Linux. Также другие безопасные функции
/// работы со строками.
///
/// @author  Иванюк Дмитрий Сергеевич.
///
/// @par Описание директив препроцессора:
/// @c DEBUG    - компиляция c выводом отладочной информации в консоль.@n@n
///
/// @par Текущая версия:
/// @$Rev: 220 $.\n
/// @$Author: id $.\n
/// @$Date:: 2011-02-15 16:58:56#$.

#ifndef L_CONSOLE_H
#define L_CONSOLE_H

#include <time.h>
//-----------------------------------------------------------------------------
#ifdef DEBUG
#undef Print
#define Print    printf
#else
#define Print(...)
#endif // DEBUG

#define RED				"\e[31m"
#define RED_I		    "\e[1;31m"
#define GREEN			"\e[32m"
#define YELLOW			"\e[33m"
#define GRAY			"\e[37m"
#define WHITE			"\e[1;37m"
#define RESET			"\e[0m"

#define CLEARSCREEN		printf("\e[2J\e[H")
#define SetColor(x)		printf(x)
//-----------------------------------------------------------------------------
extern time_t t_;
extern struct tm *timeInfo_;

#define print_time t_ = time( 0 ); timeInfo_ = localtime( &t_ );\
     printf( "%02d.%02d.%02d %02d:%02d:%02d ",\
     timeInfo_->tm_mday, timeInfo_->tm_mon + 1, timeInfo_->tm_year,\
     timeInfo_->tm_hour, timeInfo_->tm_min, timeInfo_->tm_sec ); printf
//-----------------------------------------------------------------------------
#endif // L_CONSOLE_H
