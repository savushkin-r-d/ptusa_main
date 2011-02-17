/// @file sys_7186.h
/// @brief Содержит описания классов, которые реализуют аппаратную часть 
/// PAC 7186EXD.
/// 
/// @author  Иванюк Дмитрий Сергеевич.
///
/// @par Описание директив препроцессора:
/// 
/// @par Текущая версия:
/// @$Rev$.\n
/// @$Author$.\n
/// @$Date::                     $.

#ifndef SYS_7186_H
#define SYS_7186_H

#include "sys.h"

typedef int             int_2;
typedef unsigned int    u_int_2;

typedef unsigned long   u_int_4;
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
/// @brief Работа с энергонезависимой ОЗУ. Представляет абстракцию от 
/// физического распределения памяти.
class NV_memory_manager_7186: public NV_memory_manager
    {    
    public:
        NV_memory_manager_7186();
    };
//-----------------------------------------------------------------------------
#endif SYS_7186_H