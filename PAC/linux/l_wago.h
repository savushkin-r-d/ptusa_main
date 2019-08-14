/// @file l_wago.h
/// @brief ������ � Wago ��� OC Linux.
///
/// @author  ������ ������� ���������.
///
/// @par �������� �������� �������������:
///
/// @par ������� ������:
/// @$Rev: 223 $.\n
/// @$Author: id $.\n
/// @$Date:: 2011-02-17 09:39:32#$.

#ifndef WAGO_L_H
#define WAGO_L_H

#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>

#include "wago.h"

#include "dtime.h"
#include "PAC_err.h"
#include "l_tcp_cmctr.h"
//-----------------------------------------------------------------------------
/// @brief ������ � �������� �����/������ Wago ��� OC Linux.
///
///
class wago_manager_linux : public wago_manager
    {
    protected:
        enum CONST
            {
            BUFF_SIZE = 262,
            };

        u_char buff[ BUFF_SIZE ];

        /// @brief ������������� ���������� � ����� wago.
        ///
        /// @param node - ���� wago, � ������� �������������� ����������.
        ///
        /// @return -   0 - ��.
        /// @return - < 0 - ������.
        int net_init( wago_node *node );

        /// @brief ����� � ����� wago.
        ///
        /// @param node             - ���� wago, � ������� �������������� �����.
        /// @param bytes_to_send    - ������ ������ ��� �������.
        /// @param bytes_to_receive - ������ ������ ��� ���������.
        ///
        /// @return -   0 - ��.
        /// @return - < 0 - ������.
        int e_communicate( wago_node *node, int bytes_to_send, int bytes_to_receive );

        int read_inputs();
        int write_outputs();

    public:
        wago_manager_linux();

        virtual ~wago_manager_linux();

		/// @brief ���������� �� ���� wago.
		///
		/// @param node - ���� wago, �� �������� �����������.
		void disconnect(wago_node *node) override;
    };
//-----------------------------------------------------------------------------
#endif // WAGO_L_H
