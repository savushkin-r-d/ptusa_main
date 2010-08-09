/// @file sys_linux.h
/// @brief Содержит описания классов, которые реализуют аппаратную часть
/// PAC под ОС linux.
///
/// @author  Иванюк Дмитрий Сергеевич.
///
/// @par Описание директив препроцессора:
///
/// @par Текущая версия:
/// @$Rev: 65 $.\n
/// @$Author: id@BMK $.\n
/// @$Date:: 2010-05-06 12:38:39#$.

#ifndef SYS_LINUX
#define SYS_LINUX

#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>

#include "sys.h"

typedef short int            int_2;
typedef int                  int_4;
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
        enum CONSTANTS
            {
            C_MAX_FILE_NAME = 200
            };

        char file_name[ 200 ];

        static int file;

        SRAM( const char *file_name, u_int total_size, u_int available_start_pos,
            u_int available_end_pos );
        virtual ~SRAM();

        /// @brief Метод интерфейса @ref i_memory.
        int read( void *buff, u_int count, u_int start_pos );

        /// @brief Метод интерфейса @ref i_memory.
        int write( void *buff, u_int count, u_int start_pos );
    };
//-----------------------------------------------------------------------------
class data_file : public file
    {
    public:
        data_file();

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
#endif // SYS_LINUX
