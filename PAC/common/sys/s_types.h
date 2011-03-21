/// @file s_types.h
/// @brief Некоторые переименования типов простых чисел для обеспечения 
/// совместимости при сериализации вне зависимости от разрядности процессора, 
/// также для обеспечения более удобной короткой записи.
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

#ifndef TYPES_H
#define TYPES_H

#if !defined WIN_OS && !defined LINUX_OS && !defined MINIOS7
#error You must define OS!
#endif 

#ifdef WIN_OS
#include "w_types.h"
#endif // WIN_OS

#ifdef LINUX_OS
#include "l_types.h"
#endif // LINUX_OS

#ifdef MINIOS7
#include "mos7_types.h"
#endif // LINUX_OS
//-----------------------------------------------------------------------------
typedef unsigned char     u_char;
typedef unsigned int      u_int;
typedef unsigned long int u_long;

#endif // TYPES_H
