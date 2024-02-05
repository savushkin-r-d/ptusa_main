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

#ifdef WIN_OS
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#endif // WIN_OS

#include "bus_coupler_io.h"

#include "dtime.h"
#include "PAC_err.h"

//-----------------------------------------------------------------------------
/// @brief Работа с модулями ввода/вывода для OC Linux.
///
///
class uni_io_manager : public io_manager
    {
    private:
        enum CONSTANTS
            {
            MAX_MODBUS_REGISTERS_PER_QUERY = 123,
            BUFF_SIZE = 262,
            PHOENIX_INPUTREGISTERS_STARTADDRESS = 8000,
            PHOENIX_HOLDINGREGISTERS_STARTADDRESS = 9000,
            };

        u_char buff[ BUFF_SIZE ] = { 0 };
        u_char* resultbuff;
        u_char* writebuff;

        /// @brief Инициализация соединения с узлом I/O.
        ///
        /// @param node - узел I/O, с которым осуществляется соединение.
        ///
        /// @return -   0 - ок.
        /// @return - < 0 - ошибка.
        int net_init( io_node* node ) const;

        /// @brief Обмен с узлом I/O.
        ///
        /// @param node             - узел I/O, с которым осуществляется обмен.
        /// @param bytes_to_send    - размер данных для отсылки.
        /// @param bytes_to_receive - размер данных для получения.
        ///
        /// @return -   0 - ок.
        /// @return - < 0 - ошибка.
        int e_communicate( io_node* node, int bytes_to_send, int bytes_to_receive );

        int read_input_registers( io_node* node, unsigned int address,
            unsigned int quantity, unsigned char station = 0 );
        int write_holding_registers( io_node* node, unsigned int address,
            unsigned int quantity, unsigned char station = 0 );

        int read_inputs() override;
        int write_outputs() override;

    public:
        uni_io_manager();

        ~uni_io_manager() override = default;

        /// @brief Отключение от узла.
        ///
        /// @param node - узел, от которого отключаемся.
        void disconnect( io_node* node ) override;
    };
//-----------------------------------------------------------------------------
#endif // WAGO_L_H
