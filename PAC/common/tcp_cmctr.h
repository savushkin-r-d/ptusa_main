/// @file tcp_cmctr.h
/// @brief �������� �������� �������, ������� ������������� ��� ������ �������
/// PAC-������ �� ��������� TCP.
///
/// @author  ������ ������� ���������.
///
/// @par �������� �������� �������������:
///
/// @par ������� ������:
/// @$Rev$.\n
/// @$Author$.\n
/// @$Date::                     $.

#ifndef TCP_CMCTR_H
#define TCP_CMCTR_H

#include <stdio.h>
#include <map>

#include "smart_ptr.h"

class tcp_client;

//-----------------------------------------------------------------------------
/// @brief ������� ����� ������������ - ����� ������� PAC-������.
class tcp_communicator
    {
    public:
        /// @brief ����������� ������� �������.
        typedef long int srv_proc( long int, u_char *, u_char * );
        typedef srv_proc *srv_ptr;

        /// @brief ��������� ������������� ���������� ������ ��� ������ �
        /// ��������������.
        ///
        /// @return - ��������� �� ������������ ������ ������ @ref
        /// tcp_communicator.
        static tcp_communicator* get_instance();

        static void init_instance( const char *name_rus, const char *name_eng );

        /// @brief �������� ������ ������� � ��������.
        virtual int evaluate() = 0;

        /// @brief ���������� ���������������� ������� �� ������ ������� -
        /// �������.
        ///
        /// @param srv_id - �����, �� ������� ����� ��������� ������.
        /// @param fk     - ��������� �� ������ ����������� ����� ������.
        srv_ptr reg_service( u_char srv_id, srv_ptr fk );

        /// @brief ��������� �������� ����� PAC.
        ///
        /// @return - ������� ��� PAC �� ������� �����.
        char* get_host_name_rus();

        /// @brief ��������� �������� ����� PAC.
        ///
        /// @return - ������� ��� PAC �� ���������� �����.
        char* get_host_name_eng();

        virtual int add_async_client(tcp_client* client);
        virtual int remove_async_client(tcp_client* client);

        virtual ~tcp_communicator();
        
        static void set_port( int new_port, int new_port_modbus )
            {
            port = new_port;
            port_modbus = new_port_modbus;
            }

        enum CONSTANTS
            {
            BUFSIZE     = 500 * 1024,      ///< ������ ������.

#ifdef LINUX_OS
            MAX_SOCKETS = 32,              ///< ������������ ���������� �������.
#endif // LINUX_OS
#ifdef WIN_OS
            MAX_SOCKETS = 32,              ///< ������������ ���������� �������.
#endif // WIN_OS
            QLEN        = MAX_SOCKETS - 1, ///< ������������ ���������� ����������.

            TC_MAX_HOST_NAME      = 70,
            TC_MAX_SERVICE_NUMBER = 16,
            };

    protected:
        static int port;                  ///< ����.
        static int port_modbus;

        tcp_communicator();

        //ERRORS DEFINITION
        enum ERRORS
            {
            ERR_RETRIVE       = 1,
            ERR_WRONG_SERVICE = 3,
            ERR_TRANSMIT      = 4,
            ERR_WRONG_CMD     = 5,
            };

        //COMMANDS DEFINITION
        enum COMMANDS
            {
            FRAME_SINGLE = 1,
            AKN_ERR      = 7,
            AKN_DATA     = 8,
            AKN_OK       = 12,
            };

        static auto_smart_ptr < tcp_communicator > instance;///< ��������� ������.

        srv_ptr services[ TC_MAX_SERVICE_NUMBER ];  ///< ������ ��������.

        char host_name_rus[ TC_MAX_HOST_NAME + 1] = { 0 }; ///< ������� ��� PAC.
        char host_name_eng[ TC_MAX_HOST_NAME + 1] = { 0 }; ///< ������� eng ��� PAC.

        int max_cycles;         ///< ������������ ���������� ������ ��������� ��������� ������� �� 1 ������.
        int glob_cmctr_ok;      ///< ���� ���������� ������ � ��������.

        u_int   in_buffer_count;        ///< ���������� ������ � ������.
        u_char  buf[ BUFSIZE ] = { 0 }; ///< �����.

        u_char pidx;            ///< ����� ������.
        int    net_id;          ///< ����� PAC.

        std::map<int, tcp_client*> *clients;

        void _ErrorAkn( u_char error );
        void _AknData( u_long len );
        void _AknOK();
    };
//-----------------------------------------------------------------------------
#define G_CMMCTR tcp_communicator::get_instance()

#endif //TCP_CMCTR_H
