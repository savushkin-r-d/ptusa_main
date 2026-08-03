/// @file base_mem.h
/// @brief Содержит описания классов, которые реализуют аппаратную часть PAC -
/// различную память - на абстрактном уровне.
///
/// @author  Иванюк Дмитрий Сергеевич.
///
/// @par Описание директив препроцессора:
///
/// @par Текущая версия:
/// @$Rev: 220 $.\n
/// @$Author: id $.\n
/// @$Date:: 2011-02-15 16:58:56#$.

#pragma once

#include "console.h"
#include "s_types.h"

#include "smart_ptr.h"
//-----------------------------------------------------------------------------
/// @brief Интерфейс доступа к памяти.
class i_memory
    {
    public:
        virtual ~i_memory()
            {
            }

        /// @brief Чтение массива байт.
        ///
        /// @param buf       - адрес буфера, куда будут считываться данные.
        /// @param count     - количество считываемых байт.
        /// @param start_pos - стартовый адрес.
        ///
        /// @return <  0 - ошибка.
        /// @return >= 0 - количество считанных байт.
        virtual int read( void *buf, u_int count, u_int start_pos ) = 0;

        /// @brief Безопасное сохранение данных.
        /// @param buff - адрес буфера, куда будут записываться данные.
        /// @return     - результат.
        virtual int safe_save( void* buff ) = 0;

        /// @brief Получение размера памяти в байтах.
        ///
        /// @return - размер памяти в байтах.
        virtual u_int get_size() const = 0;
    };
//-----------------------------------------------------------------------------
///\russian
/// @brief Работа напрямую с энергонезависимой ОЗУ. Абстракция от аппаратной
/// реализации памяти.
///\endif

class NV_memory : public i_memory
    {

    public:
        /// @param total_size           - общий размер памяти.
        /// @param available_start_pos  - начальный доступный адрес.
        /// @param available_end_pos    - конечный доступный адрес.
        NV_memory( u_int total_size, u_int available_start_pos,
            u_int available_end_pos );

        /// @brief Метод интерфейса @ref i_memory.
        u_int get_size() const
            {
            return total_size;
            }

        /// @brief Получение начального доступного для работы адреса памяти.
        ///
        /// @return - начальный доступный для работы адрес памяти.
        u_int get_available_start_pos() const
            {
            return available_start_pos;
            }

        /// @brief Получение конечного доступного для работы адреса памяти.
        ///
        /// @return - конечный доступный для работы адрес памяти.
        u_int get_available_end_pos() const
            {
            return available_end_pos;
            }

        virtual ~NV_memory() = default;

        virtual void init( void * NV_ram_data ) {}

    private:

        u_int total_size;           ///< Общий размер памяти.

        /// @brief Начальный доступный адрес.
        /// @details Для пропуска зарезервированной системной области.
        u_int available_start_pos;

        /// @brief Конечный доступный адрес.
        /// @details Для пропуска зарезервированной системной области.
        u_int available_end_pos;
    };
//-----------------------------------------------------------------------------
/// @brief Работа с блоком памяти.
class memory_range: public i_memory
    {
    friend class NV_memory_manager;

    public:
        /// @brief Метод интерфейса @ref i_memory.
        u_int get_size() const
            {
            return size;
            }

        virtual ~memory_range() = default;

        /// @brief Метод интерфейса @ref i_memory.
        int read( void *buf, u_int count, u_int start_pos );

        /// @brief Метод интерфейса @ref i_memory.
        int safe_save( void* buff ) override;

    private:
        i_memory    *memory;    ///< Указатель на объект памяти.
        u_int       start_pos;  ///< Начальный адрес.
        u_int       size;       ///< Размер блока памяти в байтах.

        /// @brief Закрытый конструктор.
        ///
        /// Создание объектов осуществляется через метод get_memory_block
        /// класса @ref NV_memory_manager.
        memory_range( i_memory *memory, u_int start_pos, u_int size );

        /// @brief Проверка на корректность параметров.
        ///
        /// @return - результат проверки
        ///    0 - Ок.
        ///    1 - Ошибка.
        int check_params( u_int count, u_int start_pos );
    };
//-----------------------------------------------------------------------------
/// @brief Работа с объектом файловой системы. Представляет абстракцию от
/// физической реализации таковой.
class file
    {
    public:
        virtual ~file()
            {
            }

        /// @brief Открытие файла.
        ///
        /// @param file_name - имя файла.
        ///
        /// @return - результат
        ///    0   - Ок.
        ///    0 < - Ошибка.
        virtual int file_open( const char *file_name ) = 0;

        /// @brief Чтение из файла блока данных.
        ///
        /// @param buffer - буфер, куда поместить результат.
        /// @param count - количество считываемых байт.
        ///
        /// @return - количество считанных байт.
        virtual int file_read( void *buffer, int count ) = 0;

        /// @brief Чтение из файла строки.
        ///
        /// @return - указатель на начало строки.
        virtual char* fget_line() = 0;

        /// @brief Чтение из файла строки без изменения позиции указателя в
        /// файле.
        ///
        /// @return - указатель на начало строки.
        virtual char* pfget_line() = 0;

        /// @brief Закрытие файла.
        ///
        /// @return - результат
        ///    0   - Ок.
        ///    0 < - Ошибка.
        virtual void file_close() = 0;
    };
//-----------------------------------------------------------------------------
/// @brief Работа с энергонезависимой ОЗУ. Абстракция от физического
/// распределения памяти.
class NV_memory_manager
    {
    public:
        enum MEMORY_TYPE ///< Типы энергонезависимой памяти.
            {
            MT_NVRAM,
            MT_EEPROM
            };

        /// @brief Получение объекта для работы с блоком памяти.
        ///
        /// @param m_type - тип памяти, откуда будет выделятся память.
        /// @param count  - размер в байтах.
        ///
        /// @return =  0 - ошибка.
        /// @return >  0 - указатель на объект выделенного блока памяти.
        virtual memory_range* get_memory_block( MEMORY_TYPE m_type,
            u_int count );

        /// @brief Получение единственного экземпляра класса для работы с
        /// параметрами.
        ///
        /// @return - указатель на единственный объект класса @ref
        /// NV_memory_manager.
        static NV_memory_manager* get_instance();

        virtual ~NV_memory_manager();

        NV_memory_manager( const NV_memory_manager& ) = delete;
        NV_memory_manager( NV_memory_manager&& ) = delete;
        NV_memory_manager& operator=( const NV_memory_manager& ) = delete;
        NV_memory_manager& operator=( NV_memory_manager&& ) = delete;

    protected:
        /// Статический экземпляр класса для вызова методов.
        static auto_smart_ptr < NV_memory_manager > instance;

        NV_memory_manager();

        /// @brief Работа с энергонезависимой ОЗУ (non-volatile random access
        /// memory).
        ///
        /// Данная память питается от батарейки, не имеет ограничений на
        /// количество циклов записи/чтения.
        NV_memory *PAC_NVRAM;

        /// @brief Работа с энергонезависимой ОЗУ (Electrically Erasable
        /// Programmable Read-Only Memory).
        ///
        /// Имеет ограничения на количество циклов записи/чтения - 1 миллион.
        NV_memory *PAC_EEPROM;

        u_int last_NVRAM_pos;  ///< Индекс крайнего свободного элемента NVRAM.
        u_int last_EEPROM_pos; ///< Индекс крайнего свободного элемента EEPROM.
    };
//-----------------------------------------------------------------------------
/// @brief Работа с энергонезависимой ОЗУ (Static Memory).
///
/// Имеет ограничения на количество циклов записи/чтения - 1 миллион.
class SRAM : public NV_memory
    {
    friend class NV_memory_manager;

    private:
        enum CONSTANTS
            {
            C_MAX_FILE_NAME = 200
            };

        char file_name[ 200 ];
        char tmp_name[ 200 ];

        FILE* file{};

        SRAM( const char* file_name, u_int total_size, u_int available_start_pos,
            u_int available_end_pos );
        virtual ~SRAM();

        /// @brief Метод интерфейса @ref i_memory.
        int read( void* buff, u_int count, u_int start_pos );

        int safe_save( void* buff ) override;
    };
//-----------------------------------------------------------------------------
class data_file : public file
    {
    public:
        data_file();

        virtual ~data_file()
            {
            data_file::file_close();
            }

        int file_open( const char* file_name );

        int file_read( void* buffer, int count );

        char* fget_line();

        char* pfget_line();

        void file_close();

    private:
        enum CONSTANTS
            {
            C_MAX_BUFFER_SIZE = 200
            };

        FILE* f;
        char buf[ C_MAX_BUFFER_SIZE ];
    };
