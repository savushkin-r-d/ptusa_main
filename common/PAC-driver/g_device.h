/// @file g_device.h
/// @brief Содержит описания классов, которые используются совместно в
/// контроллере и драйвере на сервере.
///
/// Описываются базовые интерфейсы, классы.
///
/// @author  Иванюк Дмитрий Сергеевич.
///
/// @par Описание директив препроцессора:
/// @c DEBUG    - компиляция c выводом отладочной информации в консоль.@n@n
///
/// @c DRIVER   - компиляция для драйвера в OS Windows.@n
/// @c WIN32    - компиляция для PAC с ОС Windows.@n
/// @c LINUX_OS - компиляция для PAC с ОС linux.@n Данные директивы определяют
/// разрядность базовых типов данных (@ref int_2,
/// @ref u_int_4, @ref u_int_2).
///
/// @par Текущая версия:
/// @$Rev$.\n
/// @$Author$.\n
/// @$Date::                     $.
#ifndef GENERAL_DEVICE_H
#define GENERAL_DEVICE_H

#ifndef DRIVER
#include "sys.h"
#include "tcp_cmctr.h"
#include <vector>

#if defined LINUX_OS
#include "sys_linux.h"
#endif // defined LINUX_OS

#if defined WIN32
#include "sys_win.h"
#endif // defined WIN32

//-----------------------------------------------------------------------------
/// @brief Интерфейс устройства, позволяющий сохранить его в потоке байтов.
class i_Lua_save_device    
    {    
    public:
        /// @brief Сохранение самого устройства в буфер.
        ///
        /// @param buff [ out ] - адрес буфера, куда будут записываться данные.
        ///
        /// @return >= 0 - количество записанных байт.
        virtual int save_device( char *buff ) = 0; 

        /// @brief Отладочная печать объекта в консоль.
		virtual void print() const = 0;
    };
//-----------------------------------------------------------------------------
/// @brief Интерфейс устройства, позволяющий получать команды от сервера.
class i_cmd_device    
	{      
	public:
        /// @brief Выполнение числовой команды.
        ///
        /// @param prop [ in ] - имя свойства.
        /// @param idx [ in ]  - индекс для свойства.
        /// @param val [ in ]  - значение.
        ///
        /// @return 0 - ок.
        /// @return 1 - ошибка.
        virtual int set_cmd( const char *prop, u_int idx, double val ) = 0;

        /// @brief Выполнение строковой команды.
        ///
        /// @param prop [ in ] - имя свойства.
        /// @param idx [ in ]  - индекс для свойства.
        /// @param val [ in ]  - значение.
        ///
        /// @return 0 - ок.
        /// @return 1 - Ошибка.
        virtual int set_cmd( const char *prop, u_int idx, char *val ) = 0;
	};

#endif // DRIVER
//-----------------------------------------------------------------------------
/// @brief Коммуникатор устройств - содержит все устройства одного PAC. Служит
/// для передачи информации о них и их состоянии на сервер (PC).
class device_communicator             
    {   
    public:
        enum CMD
            {
            CMD_GET_INFO_ON_CONNECT = 10, ///< Запрос инф. о PAC перед дальнейшей работой.

            CMD_GET_DEVICES = 100,   ///< Запрос инф. об устройствах PAC.
            CMD_GET_DEVICES_STATES,  ///< Запрос инф. о состоянии устройств PAC.
            CMD_EXEC_DEVICE_COMMAND, ///< Выполнение команды для устройства.

            CMD_GET_PAC_ERRORS,      ///< Запрос инф. об ошибках PAC.
            CMD_SET_PAC_ERROR_CMD,   ///< Выполнение команды для ошибки.
            };
#ifdef DRIVER
    };
#else // DRIVER

    private:
        /// Единственный экземпляр класса.
        static auto_smart_ptr < device_communicator > instance;

    public:
        /// @brief Получение единственного экземпляра класса.
        static device_communicator* get_instance()
            {
            static char is_init = 0;
            if ( 0 == is_init )
            	{
                is_init = 1;
                instance = new device_communicator();
            	}

            return instance;
            }

        /// @brief Установка единственного экземпляра класса.
        static void set_instance( device_communicator* new_instance )
            {
            instance = new_instance;
            }

        enum CONSTANTS
            {
            C_SERVICE_N = 1, ///< Номер сервиса коммуникатора.            
            };

        /// @brief Устройства, информация о них и их состоянии передается на
        /// сервер (PC).
        static std::vector< i_Lua_save_device* > dev;

    public:
        device_communicator()
            {            
            }

        ~device_communicator()
            {
            }

        /// @brief Вывод на консоль устройств группы.
        void print() const;

        /// @brief Добавление устройства.
        int add_device( i_Lua_save_device *dev );

        /// @brief Сервис для работы с device_communicator.
        static long write_devices_states_service( long len, u_char *data,
            u_char *outdata );
    };
//-----------------------------------------------------------------------------
#define G_DEVICE_CMMCTR device_communicator::get_instance()

#endif // DRIVER

#endif // DEVICES_H
