/// @file sys_w750.h
/// @brief Содержит описания классов, которые реализуют аппаратную часть 
/// PAC WAGO 750.
/// 
/// @author  Иванюк Дмитрий Сергеевич.
///
/// @par Описание директив препроцессора:
/// 
/// @par Текущая версия:
/// @$Rev: 65 $.\n
/// @$Author: id@BMK $.\n
/// @$Date:: 2010-05-06 12:38:39#$.

#ifndef SYS_7186_H
#define SYS_7186_H

#include "sys.h"
#include <fcntl.h>
#include <unistd.h>

#include <sys/time.h>

typedef short int          int_2;
typedef int                int_4;
typedef unsigned short int u_int_2;

typedef unsigned int       u_int_4;
//-----------------------------------------------------------------------------
/// @brief Работа с энергонезависимой ОЗУ (Static Memory).
///
/// Имеет ограничения на количество циклов записи/чтения - 1 миллион.
class SRAM: public NV_memory 
    {
    friend class NV_memory_manager_W750;

    private:

        static int file;

        SRAM( u_int total_size, u_int available_start_pos, 
            u_int available_end_pos );
        virtual ~SRAM();

        /// @brief Метод интерфейса @ref i_memory.
        int read( void *buff, u_int count, u_int start_pos );

        /// @brief Метод интерфейса @ref i_memory.
        int write( void *buff, u_int count, u_int start_pos );
    };
//-----------------------------------------------------------------------------
/// @brief Работа с энергонезависимой ОЗУ. Представляет абстракцию от 
/// физического распределения памяти.
class NV_memory_manager_W750: public NV_memory_manager
    {    
    public:
        NV_memory_manager_W750();
    };
//-----------------------------------------------------------------------------
class file_w750 : public file
    {
    public:
        file_w750();

        int fopen( const char* file_name );

        int fread( void *buffer, int count );

        char* fget_line();

        char* pfget_line();

        void fclose();            

    private:
        enum CONSTANTS
            {
            C_MAX_BUFFER_SIZE = 200
            };

        int f;
        char buf[ C_MAX_BUFFER_SIZE ];
    };
//-----------------------------------------------------------------------------
#endif // SYS_7186_H
