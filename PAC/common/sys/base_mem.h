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

#include <cstddef>

#include "console.h"
#include "s_types.h"

#include "smart_ptr.h"
#include <filesystem>
//-----------------------------------------------------------------------------
/// @brief Интерфейс доступа к памяти.
class i_memory
    {
    public:
        virtual ~i_memory() = default;

        /// @brief Чтение массива байт.
        ///
        /// @param buff       - адрес буфера, куда будут считываться данные.
        /// @param count     - количество считываемых байт.
        /// @param start_pos - стартовый адрес.
        ///
        /// @return <  0 - ошибка.
        /// @return >= 0 - количество считанных байт.
        virtual int read( std::byte *buff, u_int count,
            u_int start_pos = 0 ) = 0;

        /// @brief Безопасное сохранение данных.
        /// @param buff - адрес буфера, куда будут записываться данные.
        /// @return     - результат.
        virtual int safe_save( const std::byte* buff ) = 0;

        /// @brief Получение размера памяти в байтах.
        ///
        /// @return - размер памяти в байтах.
        virtual u_int get_size() const = 0;

        virtual int zero_fill() = 0;
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
        u_int get_size() const override
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

        ~NV_memory() override = default;

        NV_memory( const NV_memory& ) = delete;
        NV_memory( NV_memory&& ) = delete;
        NV_memory& operator=( const NV_memory& ) = delete;
        NV_memory& operator=( NV_memory&& ) = delete;

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
        u_int get_size() const override
            {
            return size;
            }

        ~memory_range() override = default;

        memory_range( const memory_range& ) = delete;
        memory_range( memory_range&& ) = delete;
        memory_range& operator=( const memory_range& ) = delete;
        memory_range& operator=( memory_range&& ) = delete;

        /// @brief Метод интерфейса @ref i_memory.
        int read( std::byte *buf, u_int count, u_int start_pos = 0 ) override;

        /// @brief Метод интерфейса @ref i_memory.
        int safe_save( const std::byte* buff ) override;

        int zero_fill() override;

    private:
        i_memory *memory;        ///< Указатель на объект памяти.
        u_int    start_position; ///< Начальный адрес.
        u_int    size;           ///< Размер блока памяти в байтах.

        /// @brief Закрытый конструктор.
        ///
        /// Создание объектов осуществляется через метод get_memory_block
        /// класса @ref NV_memory_manager.
        memory_range( i_memory *memory, u_int start_pos, u_int size );

        /// @brief Проверка на корректность параметров.
        ///
        /// @return - результат проверки
        ///    0 - ОК.
        ///    1 - Ошибка.
        int check_params( u_int count, u_int start_pos );
    };
//-----------------------------------------------------------------------------
/// @brief Работа с энергонезависимой ОЗУ. Абстракция от физического
/// распределения памяти.
class NV_memory_manager
    {
    public:
        enum MEMORY_TYPE ///< Типы энергонезависимой памяти.
            {
            // Общий термин для энергонезависимой RAM.
            // Может быть батарейно-поддерживаемым SRAM, FRAM, MRAM и т.п.
            // Обычно быстрее, чем EEPROM, может иметь большую выносливость
            // при записи - ведёт себя как обычная оперативная память, но не
            // теряет данные при выключении.
            //
            // Может использоваться для более частых обновлений или для
            // системных параметров, где важна производительность и надёжность
            // записи.
            MT_NVRAM,

            // Энергонезависимая память, которую можно
            // электрически стирать и перезаписывать. Сохраняет данные без
            // питания, обычно медленные записи, имеет ограниченное число
            // циклов записи, обычно пишется блоками.
            //
            // Обычно используется для данных, которые нельзя часто
            // перезаписывать.
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

    public:
        SRAM( const std::filesystem::path& file_name,
            u_int total_size, u_int available_start_pos,
            u_int available_end_pos );

        SRAM( const std::filesystem::path& file_name,
            u_int total_size );
        ~SRAM() override;

        /// @brief Метод интерфейса @ref i_memory.
        int read( std::byte* buff, u_int count, u_int start_pos ) override;

        int safe_save( const std::byte* buff ) override;

        int zero_fill() override;

    private:

    SRAM( const SRAM& ) = delete;
    SRAM( SRAM&& ) = delete;
    SRAM& operator=( const SRAM& ) = delete;
    SRAM& operator=( SRAM&& ) = delete;

    std::filesystem::path file_path;
    std::filesystem::path tmp_path;

    FILE* file{};
    };
