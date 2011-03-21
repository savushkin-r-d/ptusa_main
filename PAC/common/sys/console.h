/// @file console.h
/// @brief Работа с консолью вне зависимости от аппаратной реализации. Также 
/// некоторые безопасные функции работы со строками.
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

#ifndef CONSOLE_H
#define CONSOLE_H

#if !defined WIN_OS && !defined LINUX_OS && !defined MINIOS7
#error You must define OS!
#endif 

#include <string.h>
#include <stdio.h>

#include "s_types.h"
//-----------------------------------------------------------------------------
/// @brief Печать в консоль.
#ifndef MINIOS7 
#define Print( ... )
#endif // MINIOS7

#ifdef WIN_OS
#include "w_console.h"
#endif // WIN_OS

#ifdef LINUX_OS
#include "l_console.h"
#endif // LINUX_OS

#ifdef MINIOS7
#include "mos7_console.h"
#endif // LINUX_OS
//-----------------------------------------------------------------------------
/// @brief Проверка нажатия клавиши.
///
/// @return 0 - нет нажатий клавиш.
/// @return 1 - нажата клавиша.
int  kb_hit();
//-----------------------------------------------------------------------------
/// @brief Считывание нажатого символа с консоли.
///
/// @return - символ нажатой клавиши.
int  get_char();
//-----------------------------------------------------------------------------
/// @brief Вывод числа в консоль в двоичном виде.
///
/// @param с - выводимое число.
void print_binary( unsigned int c );
//-----------------------------------------------------------------------------
#endif // CONSOLE_H
