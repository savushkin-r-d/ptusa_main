/// @file g_device.h
/// @brief �������� �������� �������, ������� ������������ ��������� �
/// ����������� � �������� �� �������.
///
/// ����������� ������� ����������, ������.
///
/// @author  ������ ������� ���������.
///
/// @par �������� �������� �������������:
///
/// @c DRIVER   - ���������� ��� �������� � OS Windows.@n
/// @c WIN32    - ���������� ��� PAC � �� Windows.@n
/// @c LINUX_OS - ���������� ��� PAC � �� linux.@n ������ ��������� ����������
/// ����������� ������� ����� ������ (@ref int_2, @ref u_int_4, @ref u_int_2).
/// @c DEBUG_DEV_CMCTR - ����� ���������� ���������� � �������.@n
///
/// @par ������� ������:
/// @$Rev$.\n
/// @$Author$.\n
/// @$Date::                     $.
#ifndef GENERAL_DEVICE_H
#define GENERAL_DEVICE_H

#ifndef DRIVER

#include <stdlib.h>
#include <vector>

#include "smart_ptr.h"

#include "tcp_cmctr.h"

extern "C" {
#include "zlib.h"
    };

//-----------------------------------------------------------------------------
/// @brief ��������� ����������, ����������� ��������� ��� � ������ ������.
class i_Lua_save_device
    {
    public:
        /// @brief ���������� ������ ���������� � �����.
        ///
        /// @param buff [ out ] - ����� ������, ���� ����� ������������ ������.
        ///
        /// @return >= 0 - ���������� ���������� ����.
        virtual int save_device( char *buff ) = 0;

#ifdef RM_PAC
        /// @brief ���������� ������ ���������� � ����� ��� ���������� �������.
        ///
        /// @param buff [ out ] - ����� ������, ���� ����� ������������ ������.
        ///
        /// @return >= 0 - ���������� ���������� ����.
        virtual int rm_save_device( char *buff )
            {
            buff[ 0 ] = 0;
            return 0;
            }

        /// @brief ���������� ������ ���������� � ����� ��� ���������� �������.
        ///
        /// @param buff [ out ] - ����� ������, ���� ����� ������������ ������.
        ///
        /// @return >= 0 - ���������� ���������� ����.
        virtual int rm_save_device_state( char *buff ) = 0;
#endif // RM_PAC

        /// @brief ���������� ������ ������� � �������.
        virtual const char* get_name_in_Lua() const = 0;
    };
//-----------------------------------------------------------------------------
/// @brief ��������� ����������, ����������� �������� ������� �� �������.
class i_cmd_device
    {
    public:
        /// @brief ���������� �������� �������.
        ///
        /// @param prop [ in ] - ��� ��������.
        /// @param idx [ in ]  - ������ ��� ��������.
        /// @param val [ in ]  - ��������.
        ///
        /// @return 0 - ��.
        /// @return 1 - ������.
        virtual int set_cmd( const char *prop, u_int idx, double val ) = 0;

        /// @brief ���������� ��������� �������.
        ///
        /// @param prop [ in ] - ��� ��������.
        /// @param idx [ in ]  - ������ ��� ��������.
        /// @param val [ in ]  - ��������.
        ///
        /// @return 0 - ��.
        /// @return 1 - ������.
        virtual int set_cmd( const char *prop, u_int idx, char *val ) = 0;
    };

#endif // DRIVER
//-----------------------------------------------------------------------------
/// @brief ������������ ��������� - �������� ��� ���������� ������ PAC. ������
/// ��� �������� ���������� � ��� � �� ��������� �� ������ (PC).
class device_communicator
    {
    public:
        enum CMD
            {
            CMD_GET_INFO_ON_CONNECT = 10, ///< ������ ���. � PAC ����� ���������� �������.

            CMD_GET_DEVICES = 100,   ///< ������ ���. �� ����������� PAC.
            CMD_GET_DEVICES_STATES,  ///< ������ ���. � ��������� ��������� PAC.
            CMD_EXEC_DEVICE_COMMAND, ///< ���������� ������� ��� ����������.

            CMD_GET_PAC_ERRORS,      ///< ������ ���. �� ������� PAC.
            CMD_SET_PAC_ERROR_CMD,   ///< ���������� ������� ��� ������.

            // ��������� ����������� ����������.
            ///@brief ��������� ���������� ���������.
            ///
            /// ��� ������������ ��� �������� ��������� ����� �� ������� �������.
            CMD_GET_PARAMS,

            ///@brief �������������� ���������� ��������� �� ������� Lua.
            CMD_RESTORE_PARAMS,
            ///@brief ��������� ����������� ����� ����������.
            ///
            /// ��� ����� ������������ ��� ������������ ��������� ���������� PAC
            /// � �� ������ � ���� ������ � ��������� �����.
            CMD_GET_PARAMS_CRC,
            // ��������� ����������� ����������. -!>

            CMD_RM_GET_DEVICES = 200,   ///< ������ ��������� PAC �� PAC-�������.
            CMD_RM_GET_DEVICES_STATES,  ///< ������ ��������� ��������� PAC �� PAC-�������.
            };

#ifndef DRIVER
    private:
        /// ������������ ��������� ������.
        static auto_smart_ptr < device_communicator > instance;
        static char buff[ tcp_communicator::BUFSIZE ];

    public:
        /// @brief ��������� ������������� ���������� ������.
        static device_communicator* get_instance()
            {
            if ( instance.is_null() )
                {
                instance = new device_communicator();
                }

            return instance;
            }

        enum CONSTANTS
            {
            C_SERVICE_N = 1, ///< ����� ������� �������������.
            };

        /// @brief ����������, ���������� � ��� � �� ��������� ���������� ��
        /// ������ (PC).
        static std::vector< i_Lua_save_device* > dev;

    public:
        device_communicator()
            {
            }

        ~device_communicator()
            {
            }

        /// @brief ����� �� ������� ��������� ������.
        void print() const;

        /// @brief ���������� ����������.
        int add_device( i_Lua_save_device *dev );

        /// @brief ������ ��� ������ � device_communicator.
        static long write_devices_states_service( long len, u_char *data,
            u_char *outdata );
#endif // !DRIVER
    };
//-----------------------------------------------------------------------------
#ifndef DRIVER
#define G_DEVICE_CMMCTR device_communicator::get_instance()
#endif // !DRIVER

#endif // DEVICES_H
