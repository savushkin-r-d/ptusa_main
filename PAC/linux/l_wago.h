/// @file l_wago.h
/// @brief Работа с Wago для OC Linux.
///
/// @author  Иванюк Дмитрий Сергеевич.
///
/// @par Описание директив препроцессора:
///
/// @par Текущая версия:
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
/// @brief Работа с модулями ввода/вывода Wago для OC Linux.
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

        /// @brief Инициализация соединения с узлом wago.
        ///
        /// @param node - узел wago, с которым осуществляется соединение.
        ///
        /// @return -   0 - ок.
        /// @return - < 0 - ошибка.
        int net_init( wago_node *node );

        /// @brief Обмен с узлом wago.
        ///
        /// @param node             - узел wago, с которым осуществляется обмен.
        /// @param bytes_to_send    - размер данных для отсылки.
        /// @param bytes_to_receive - размер данных для получения.
        ///
        /// @return -   0 - ок.
        /// @return - < 0 - ошибка.
        int e_communicate( wago_node *node, int bytes_to_send, int bytes_to_receive );

        int read_inputs();
        int write_outputs();

    public:
        wago_manager_linux();

        virtual ~wago_manager_linux();

		/// @brief Отключение от узла wago.
		///
		/// @param node - узел wago, от которого отключаемся.
		void disconnect(wago_node *node) override;
    };
//-----------------------------------------------------------------------------
#endif // WAGO_L_H
