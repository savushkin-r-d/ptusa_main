/// @file PAC_err.h
/// @brief �������� �������� �������, ������� ������������ ��� �����������
/// ������ � ������������ �������� PAC.
///
/// ����� @ref PAC_critical_errors_manager, �������� ��� ����������� ������ �
/// �������� ������� ���������, �������� ��� ����������� ����������. ��� �����
/// ���� ������ ������ ��������� @ref PAC_critical_errors_manager::critical_error.
///
/// @author  ������ ������� ���������.
///
/// @par �������� �������� �������������:
/// @c PAC_ERRORS_H - ��������� ������� ����� � ���������� ������ ���� ���.@n
///
/// @par ������� ������:
/// @$Rev$.\n
/// @$Author$.\n
/// @$Date::                     $.

#ifndef PAC_ERRORS_H
#define PAC_ERRORS_H

#include <errno.h>

#include <string.h>
#include <vector>

#include "errors.h"
#include "smart_ptr.h"
#include "dtime.h"
#include "led.h"

//-----------------------------------------------------------------------------
//0, 1, n - ��� ����� � ����� I/O ����� n.
//0, 2, n - ��� ����� � ������� ����� n.
//0, 3, n - ��� ����� � Modbus ����������� ����� n.
//0, 4, 4 -
//0, 5, 5 - ��� ����� � ��������.
//
//1, 1 - ������ ������ � COM-������ WAGO.
//      1 - CRC error
//
//13 - ������ ������� ������:
//  1, n  - ������ ��������� ������ ����� n.
//-----------------------------------------------------------------------------
class PAC_critical_errors_manager
    {
    public:
        enum CONSTANTS
            {
            ALARM_CLASS_PRIORITY = 100,
            };

        enum ALARM_CLASS      ///< ����� �������.
            {
            AC_UNKNOWN,
            AC_NO_CONNECTION, ///< ������ �����.

            AC_COM_DRIVER,    ///< ������ ������ � COM-������.
            AC_RUNTIME_ERROR, ///< ������ �� ����� ������.

            AC_NET,           ///< ������ ������� ������.
            AC_SERVICE,		  /// ��������� ������������ ����������.
            };

        enum ALARM_SUBCLASS         ///< �������� �������.
            {
            //AC_NO_CONNECTION,     ///< ������ �����.
            AS_IO_COUPLER = 1,      ///< ������ ������� I/O.
            AS_PANEL,               ///< ������ ������� EasyView.
            AS_MODBUS_DEVICE,       ///< ������ ����������, ������������� �� Modbus.
            AS_RFID_READER,         ///< ������ RFID-reader'�.

            AS_EASYSERVER = 5,      ///< ������ EasyServer.
            AS_REMOTE_PAC,          ///< ������ ����� � ��������� PAC.

            //AC_RUNTIME_ERROR,     ///< ������ �� ����� ������.
            AS_EMERGENCY_BUTTON = 1,///< ������ ��������� ������.

            //AC_NET,               ///< ������ ������� ������.
            AS_SOCKET_F = 1,///< ������� socket.
            AS_BIND_F,              ///< ������� bind.
            AS_SETSOCKOPT_F,
            AS_LISTEN_F,
            };

    public:
        enum GE_CONST
            {
            GE_ERROR_SIZE = 3,      ///< ������ ����� ������, ����.
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
            return "������";
            }

        const char* get_alarm_descr( ALARM_CLASS err_class,
            ALARM_SUBCLASS err_sub_class, int par, bool is_set );


        static auto_smart_ptr < PAC_critical_errors_manager > instance;

        struct critical_error
            {
            int             err_class;     ///< ����� ������.
            unsigned int    err_sub_class; ///< �������� ������.
            unsigned int    param;         ///< �������� ������.

            critical_error( int err_class = 0, u_int err_sub_class = 0,
                u_int param = 0 );
            };

        std::vector< critical_error >  errors;

        u_int_2 errors_id;
    };
//-----------------------------------------------------------------------------
#endif // PAC_ERRORS_H
