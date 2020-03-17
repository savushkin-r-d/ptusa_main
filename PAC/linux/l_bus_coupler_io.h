/// @file l_wago.h
/// @brief Работа с I/O узлами для OC Linux.
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

#include "bus_coupler_io.h"

#include "dtime.h"
#include "PAC_err.h"
#include "l_tcp_cmctr.h"
//-----------------------------------------------------------------------------
/// @brief Работа с модулями ввода/вывода для OC Linux.
///
///
class io_manager_linux : public io_manager
    {
    protected:
        enum CONSTANTS
            {
            MAX_MODBUS_REGISTERS_PER_QUERY = 123,
            BUFF_SIZE = 262,
            PHOENIX_INPUTREGISTERS_STARTADDRESS = 8000,
            PHOENIX_HOLDINGREGISTERS_STARTADDRESS = 9000,
            };

        u_char buff[ BUFF_SIZE ];
        u_char* resultbuff;
        u_char* writebuff;

        /// @brief Инициализация соединения с узлом I/O.
        ///
        /// @param node - узел I/O, с которым осуществляется соединение.
        ///
        /// @return -   0 - ок.
        /// @return - < 0 - ошибка.
        int net_init( io_node *node );

        /// @brief Обмен с узлом I/O.
        ///
        /// @param node             - узел I/O, с которым осуществляется обмен.
        /// @param bytes_to_send    - размер данных для отсылки.
        /// @param bytes_to_receive - размер данных для получения.
        ///
        /// @return -   0 - ок.
        /// @return - < 0 - ошибка.
        int e_communicate( io_node *node, int bytes_to_send, int bytes_to_receive );

        int read_input_registers(io_node* node, unsigned int address, unsigned int quantity, unsigned char station = 0);
        int write_holding_registers(io_node* node, unsigned int address, unsigned int quantity, unsigned char station = 0);

        int read_inputs();
        int write_outputs();

    public:
        io_manager_linux();

        virtual ~io_manager_linux();

		/// @brief Отключение от узла.
		///
		/// @param node - узел, от которого отключаемся.
		void disconnect(io_node *node) override;
    };
//-----------------------------------------------------------------------------
#endif // WAGO_L_H
