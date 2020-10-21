/// @file mos7_mem.h
/// @brief Содержит описания классов, которые реализуют аппаратную часть
/// работы с памятью в MINIOS7.
///
/// @author  Иванюк Дмитрий Сергеевич.
///
/// @par Описание директив препроцессора:
///
/// @par Текущая версия:
/// @$Rev: 223 $.\n
/// @$Author: id $.\n
/// @$Date:: 2011-02-17 09:39:32#$.

#ifndef MINIOS7_H
#define MINIOS7_H

#include "base_mem.h"
//-----------------------------------------------------------------------------
/// @brief Работа с энергонезависимой ОЗУ (Electrically Erasable
/// Programmable Read-Only Memory).
///
/// Имеет ограничения на количество циклов записи/чтения - 1 миллион.
class EEPROM_7186: public NV_memory
    {
    public:
        EEPROM_7186();

        /// @brief Метод интерфейса @ref i_memory.
        int read( void *buff, u_int count, u_int start_pos );

        /// @brief Метод интерфейса @ref i_memory.
        int write( void *buff, u_int count, u_int start_pos );
    };
//-----------------------------------------------------------------------------
/// @brief Работа с энергонезависимой ОЗУ (non-volatile random access 
/// memory).
///
/// Данная память питается от батарейки, не имеет ограничений на  
/// количество циклов записи/чтения.
class NVRAM_7186: public NV_memory
    {
    public:
        NVRAM_7186();

        /// @brief Метод интерфейса @ref i_memory.
        int read( void *buff, u_int count, u_int start_pos );

        /// @brief Метод интерфейса @ref i_memory.
        int write( void *buff, u_int count, u_int start_pos );
    };
//-----------------------------------------------------------------------------
#endif MINIOS7_H
