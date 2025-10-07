/// @file PAC_err.h
/// @brief Содержит описания классов, которые используются для организации
/// работы с критическими ошибками PAC.
///
/// Класс @ref PAC_critical_errors_manager, служащий для организации работы с
/// ошибками простых устройств, содержит всю необходимую информацию. Для сбора
/// всех ошибок служит структура @ref PAC_critical_errors_manager::critical_error.
///
/// @author  Иванюк Дмитрий Сергеевич.
///
/// @par Описание директив препроцессора:
/// @c PAC_ERRORS_H - включение данного файла в компиляцию только один раз.@n
///
/// @par Текущая версия:
/// @$Rev$.\n
/// @$Author$.\n
/// @$Date::                     $.

#ifndef PAC_ERRORS_H
#define PAC_ERRORS_H

#include <errno.h>

#include <string.h>
#include <vector>

#include "g_errors.h"
#include "smart_ptr.h"
#include "dtime.h"
#include "led.h"

//-----------------------------------------------------------------------------
//0, 1, n - нет связи с узлом I/O номер n.
//0, 2, n - нет связи с панелью номер n.
//0, 3, n - нет связи с Modbus устройством номер n.
//0, 4, 4 -
//0, 5, 5 - нет связи с сервером.
//
//1, 1 - ошибка работы с COM-портом WAGO.
//      1 - CRC error
//
//13 - ошибки времени работы:
//  1, n  - нажата аварийная кнопка номер n.
//-----------------------------------------------------------------------------
class PAC_critical_errors_manager
    {
    public:
        enum CONSTANTS
            {
            ALARM_CLASS_PRIORITY = 100,
            };

        enum ALARM_CLASS      ///< Класс тревоги.
            {
            AC_UNKNOWN,
            AC_NO_CONNECTION, ///< Ошибка связи.

            AC_COM_DRIVER,    ///< Ошибка работы с COM-портом.
            AC_RUNTIME_ERROR, ///< Ошибки во время работы.

            AC_NET,           ///< Ошибки сетевой работы.
            AC_SERVICE,		  /// Сервисное обслуживание устройства.
            };

        enum ALARM_SUBCLASS         ///< Подкласс тревоги.
            {
            AS_UNKNOWN,

            //AC_NO_CONNECTION,     ///< Ошибка связи.
            AS_IO_COUPLER = 1,      ///< Ошибки модулей I/O.
            AS_PANEL,               ///< Ошибки панелей EasyView.
            AS_MODBUS_DEVICE,       ///< Ошибки устройства, опрашиваемого по Modbus.
            AS_RFID_READER,         ///< Ошибки RFID-reader'а.

            AS_EASYSERVER = 5,      ///< Ошибки EasyServer.
            AS_REMOTE_PAC,          ///< Ошибки связи с удаленным PAC.

            //AC_RUNTIME_ERROR,     ///< Ошибки во время работы.
            AS_EMERGENCY_BUTTON = 1,///< Нажата аварийная кнопка.

            //AC_NET,               ///< Ошибки сетевой работы.
            AS_SOCKET_F = 1,///< Функция socket.
            AS_BIND_F,              ///< Функция bind.
            AS_SETSOCKOPT_F,
            AS_LISTEN_F,
            };

    public:
        enum GE_CONST
            {
            GE_ERROR_SIZE = 3,      ///< Размер одной ошибки, байт.
            };

        PAC_critical_errors_manager();

        void show_errors() const;
        void set_global_error( ALARM_CLASS eclass, ALARM_SUBCLASS p1,
            unsigned long param );
        void reset_global_error( ALARM_CLASS eclass, ALARM_SUBCLASS p1,
            unsigned long param );

        int save_as_Lua_str( char* str, u_int_2& id );

        static PAC_critical_errors_manager* get_instance();

        u_int get_id() const
            {
            return errors_id;
            }

        bool is_any_error() const
            {
            return !errors.empty();
            }

    private:
        static const char* get_alarm_group()
            {
            return "Авария";
            }

        const char* get_alarm_descr( ALARM_CLASS err_class,
            ALARM_SUBCLASS err_sub_class, int par, bool is_set );


        static auto_smart_ptr < PAC_critical_errors_manager > instance;

        struct critical_error
            {
            int             err_class;     ///< Класс ошибки.
            unsigned int    err_sub_class; ///< Подкласс ошибки.
            unsigned int    param;         ///< Параметр ошибки.

            critical_error( int err_class = 0, u_int err_sub_class = 0,
                u_int param = 0 );
            };

        std::vector< critical_error >  errors;

        u_int_2 errors_id;
    };
//-----------------------------------------------------------------------------
#endif // PAC_ERRORS_H
