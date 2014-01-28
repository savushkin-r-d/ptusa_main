/// @file mem_PFC200.h
/// @brief Содержит описания классов, которые реализуют аппаратную часть PAC
/// по работе с памятью.
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

#ifndef MEM_PFC200_H
#define MEM_PFC200_H

#include "base_mem.h"
//-----------------------------------------------------------------------------
/// @brief Работа с энергонезависимой ОЗУ (Static Memory).
///
/// Имеет ограничения на количество циклов записи/чтения - 1 миллион.
class eeprom_PFC200: public NV_memory
    {
    friend class NV_memory_manager;

    private:
        eeprom_PFC200( int total_size, int available_start_pos,
            int available_end_pos );
        
        virtual ~eeprom_PFC200();
        
        /// @brief Метод интерфейса @ref i_memory.
        int read( void *buff, u_int count, u_int start_pos );

        /// @brief Метод интерфейса @ref i_memory.
        int write( void *buff, u_int count, u_int start_pos );
        
        char * NV_ram_data; 	// Pointer to begin of NVRAM
    };
//-----------------------------------------------------------------------------
#endif // MEM_PFC200_H